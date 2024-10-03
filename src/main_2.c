#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_ELEVATORS 2
#define BUILDING_FLOORS 7 // Andares 0 a 6

sem_t limite_passageiros; // Semáforos

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Estrutura de um elevador
typedef struct {

  short building_floors;

  short current_floor;
  short next_floor;

  short passengers;
  
  short is_moving;
  
  short elevator_id;
  
  pthread_mutex_t lock;
  pthread_cond_t cond;

} Elevator;


typedef struct {
    Elevator elevators[NUM_ELEVATORS];
    pthread_mutex_t lock;
    pthread_cond_t request_cond;

} Building;


void *request(void *args);
void *elevator_thread(void *arg);

//Adicionado: sistema de sem�foro
void *enter_elevator(void *args);

int select_elevator(Building *building, short requested_floor);


int main() {

  sem_init(&limite_passageiros, 0, 4);

  Elevator args = {6, 0, 0, 0, 0, 1, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER}; //Elevador 1
  Elevator args2 = {6, 0, 0, 0, 0, 2, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER}; //Elevador 2

  Building building = {
        .elevators = {
            {6, 0, 0, 0, 0, 1, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER},  // Elevador 1
            {6, 0, 0, 0, 0, 2, PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER}   // Elevador 2
        },
        .lock = PTHREAD_MUTEX_INITIALIZER,
        .request_cond = PTHREAD_COND_INITIALIZER
    };

  srand(time(NULL)); //Adicionado

  pthread_t elevator_threads[NUM_ELEVATORS];
  pthread_t request_thread_id;

  //Threads para cada elevador
  for (int i = 0; i < NUM_ELEVATORS; i++) {
    pthread_create(&elevator_threads[i], NULL, elevator_thread, (void *)&building.elevators[i]);
    } 
     
  // realiza a requisi��o do elevador em um andar do pr�dio
  pthread_create(&request_thread_id, NULL, request, (void *)&building);
  
  //Codigo anterior  
  //pthread_create(&floor_thread1, NULL, &request, (void *)&args); //Elevador 1
  //pthread_create(&floor_thread2, NULL, &request, (void *)&args2); //Elevador 2
  //pthread_join(floor_thread1, NULL);
  //
  //pthread_create(&args_thread1, NULL, &move_elevator, (void *)&args);
  //thread_join(args_thread, NULL);

  //Esperar threads terminarem
  int i;
  for (i = 0; i < NUM_ELEVATORS; i++) {
        pthread_join(elevator_threads[i], NULL);
    }

  pthread_exit(NULL);
  return 0;
}

//Thread para um elevador gen�rico, SUBSTITUI 'move_elevator'
void *elevator_thread(void *args) {
	Elevator *elevator = (Elevator *)args;
	
	
	while (1) {
        pthread_mutex_lock(&elevator->lock);
        
        while (!elevator->is_moving) {
            pthread_cond_wait(&elevator->cond, &elevator->lock);
        }
    
        pthread_mutex_unlock(&elevator->lock);
        
    // Movimenta��o do elevador at� o andar requisitado
        while (elevator->current_floor != elevator->next_floor) {
            if (elevator->current_floor < elevator->next_floor) {
                printf("\t- Elevador %hi subindo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", elevator->elevator_id, elevator->current_floor, elevator->next_floor);
                elevator->current_floor++;
            } else {
                printf("\t- Elevador %hi descendo...\n\t- Andar Atual: %hi\n\t- Andar de Parada: %hi\n", elevator->elevator_id, elevator->current_floor, elevator->next_floor);
                elevator->current_floor--;
            }
            sleep(2);  // Simula o tempo de movimento do elevador -> 2 segundos
        }
        printf("Elevador %d chegou ao destino: Andar %d\n", elevator->elevator_id, elevator->current_floor);

        // Finaliza o movimento
        pthread_mutex_lock(&elevator->lock);
        elevator->is_moving = 0;
        pthread_mutex_unlock(&elevator->lock);
    }
    
    return NULL;
}

// request(pr�dio) --> realiza a requisi��o do elevador em um andar gerado
// aleatoriamente. Nesse caso, a estrutura de entrada ser� Building
void *request(void *args) {

  Building *building = (Building *)args;
  
  while(1){
  
  sleep(rand() % 30 + 1);  // Simula um tempo aleat�rio entre requisi��es
  
  short requested_floor = rand() % BUILDING_FLOORS;
  printf("-------------------------------------------------------\n\n");
  printf(">> Requisicao de elevador no andar %d\n", requested_floor);
  
  // Escolher qual elevador vai atender
  pthread_mutex_lock(&building->lock);
  
  int selected_elevator = select_elevator(building, requested_floor);
  
  if (selected_elevator != -1) {
        Elevator *elevator = &building->elevators[selected_elevator];
        
        pthread_mutex_lock(&elevator->lock); // Bloqueia o mutex do elevador
        elevator->next_floor = requested_floor;
        elevator->is_moving = 1;  // Indica que o elevador vai se mover
        
        // Envia o sinal para o elevador
        pthread_cond_signal(&elevator->cond);
        pthread_mutex_unlock(&elevator->lock); // Desbloqueia o mutex do elevador
        }
        
	pthread_mutex_unlock(&building->lock);
}
    return NULL;
    }
  


//FUNCAO ADICIONADA: SELECIONAR ELEVADOR
int select_elevator(Building *building, short requested_floor) {
    int selected_elevator = -1;
    int min_distance = BUILDING_FLOORS + 1;

    //Crit�rio de prioridade: elevador mais pr�ximo que n�o est� se movendo
    for (int i = 0; i < NUM_ELEVATORS; i++) {
        Elevator *elevator = &building->elevators[i];

        if (!elevator->is_moving) {
            int distance = abs(elevator->current_floor - requested_floor);
            if (distance < min_distance) {
                selected_elevator = i;
                min_distance = distance;
            }
        }
    }

    return selected_elevator;
}

//Fun��o para semaforo
void *enter_elevator(void *args) {
    sem_wait(&limite_passageiros); //Espera enquanto elevador est� lotado
    
    printf("Passageiro entrou no elevador.\n");
    sem_post(&limite_passageiros);  //Libera o espa�o ap�s o passageiro sair
    return NULL;
}

/*
void *move_elevator(void *args) {

  Elevator *move_args = (Elevator *)args;

  while(!(move_args->elevator_floor == move_args->elevator_next_floor)) {

    if (move_args->elevator_floor < move_args->elevator_next_floor) {
      printf("\t- Elevador subindo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", move_args->elevator_floor, move_args->elevator_next_floor);
      move_args->elevator_floor++;

    } else {
      printf("\t- Elevador descendo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", move_args->elevator_floor, move_args->elevator_next_floor);
      move_args->elevator_floor--;

    }
    sleep(1); // 1 segundo de espera por andar

  }
  printf("\t- Elevador chegou ao andar de parada: andar %d\n\n",
  move_args->elevator_floor);
  
  return NULL;
}*/
