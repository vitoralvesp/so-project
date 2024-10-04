/*

PROJETO DE SISTEMAS OPERACIONAIS
Tema: Simulação de Elevador
---
Integrantes:
Vitor Alves Pereira, 10410862
Eduardo Takashi Missaka, 10417877
Tiago Silveira Lopez, 10417600

*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Estrutura com os atributos do Elevador
typedef struct {
  short current_floor;
  short destination_floor;

} Elevator;

// Estrutura com os atributos do Pr�dio
typedef struct {
  short floors;
  short elevator_call;

} Building;

// Fun��es
void *request(void *args);
void *move_elevator(void *args);

// Escopo Global
pthread_mutex_t mutex;
sem_t elevator_available;
Elevator elevator = {0, 0};
Building building = {6, 0};

int main() {

  pthread_t elevator_call_thread; // thread da chamada do elevador
  pthread_t elevator_thread; // thread do elevador

  pthread_mutex_init(&mutex, NULL);
  sem_init(&elevator_available, 0, 1);
  srand(time(NULL));

  while (1) {

    // realiza a requisi��o do elevador em um andar do pr�dio
    pthread_create(&elevator_call_thread, NULL, &request, (void *)&building);
    pthread_join(elevator_call_thread, NULL);
    
    sem_wait(&elevator_available);

    // atribui o andar de chamada ao atributo de destino do elevador
    elevator.destination_floor = building.elevator_call;

    // move o elevador ao andar de destino/chamada
    pthread_create(&elevator_thread, NULL, &move_elevator, (void *)&elevator);
    pthread_join(elevator_thread, NULL);

    sem_post(&elevator_available);

    sleep(5);
    
  }

  // Encerrando as threads e o mutex
  pthread_mutex_destroy(&mutex);
  sem_destroy(&elevator_available);
  pthread_exit((void *)&building);
  pthread_exit((void *)&elevator);

  return 0;
}

// request(pr�dio) --> realiza a requisi��o do elevador em um andar gerado aleatoriamente
void *request(void *args) {
  pthread_mutex_lock(&mutex);
  short random_floor = rand() % ((Building *)args)->floors; // n�emros aleat�rios de 0 a 6
  printf("-------------------------------------------------------\n\n");
  printf(">> Requisicao do elevador no andar %d\n", random_floor);
  ((Building *)args)->elevator_call = random_floor; // Atualiza a chamada do elevador
  pthread_mutex_unlock(&mutex);

  return NULL;
}

// move_elevator() --> move o elevador para o andar chamado por meio do incremento ou decremento do atributo current_floor
void *move_elevator(void *args) {
  
  pthread_mutex_lock(&mutex);

  Elevator *elevator = (Elevator *)args;

  while(!(elevator->current_floor == elevator->destination_floor)) {

    if (elevator->current_floor < elevator->destination_floor) {
      printf("\t- Elevador subindo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", elevator->current_floor, elevator->destination_floor);
      elevator->current_floor++;

    } else {
      printf("\t- Elevador descendo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", elevator->current_floor, elevator->destination_floor);
      elevator->current_floor--;

    }
    sleep(1); // 1 segundo de espera por andar

  }
  printf("\t- Elevador chegou ao andar de parada: andar %d\n\n",
  elevator->current_floor);
  pthread_mutex_unlock(&mutex);
  
  return NULL;
}
