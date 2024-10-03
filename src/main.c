#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

// Estrutura com os atributos do Elevador
typedef struct {
  short current_floor;
  short destination_floor;
  short passengers_capacity;

} Elevator;

// Estrutura com os atributos do Prédio
typedef struct {
  short floors;
  short elevator_call;
  short passengers;

} Building;

// Funções
void *request();
void *move_elevator();

// Escopo Global
pthread_mutex_t mutex;
Elevator elevator = {0, 0};
Building building = {6, 0};


int main() {

  pthread_t floor_thread;
  pthread_t args_thread;

  pthread_mutex_init(&mutex, NULL);

  while (1) {

    // realiza a requisição do elevador em um andar do prédio
    pthread_create(&floor_thread, NULL, &request, NULL);
    pthread_join(floor_thread, NULL);

    elevator.destination_floor = building.elevator_call;

    pthread_create(&args_thread, NULL, &move_elevator, NULL);
    pthread_join(args_thread, NULL);

    sleep(1);
    
  }

  pthread_mutex_destroy(&mutex);
  pthread_exit(NULL);
  return 0;
}

// request(prédio) --> realiza a requisição do elevador em um andar gerado
// aleatoriamente
void *request() {
  pthread_mutex_lock(&mutex);
  short random_floor = rand() % building.floors;
  printf("-------------------------------------------------------\n\n");
  printf(">> Requisição do elevador no andar %d\n", random_floor);
  building.elevator_call = random_floor;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

//
void *move_elevator() {
  pthread_mutex_lock(&mutex);

  while(!(elevator.current_floor == elevator.destination_floor)) {

    if (elevator.current_floor < elevator.destination_floor) {
      printf("\t- Elevador subindo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", elevator.current_floor, elevator.destination_floor);
      elevator.current_floor++;

    } else {
      printf("\t- Elevador descendo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", elevator.current_floor, elevator.destination_floor);
      elevator.current_floor--;

    }
    sleep(1); // 1 segundo de espera por andar

  }
  printf("\t- Elevador chegou ao andar de parada: andar %d\n\n",
  elevator.current_floor);
  pthread_mutex_unlock(&mutex);
  
  return NULL;
}
