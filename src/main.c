/*

PROJETO DE SISTEMAS OPERACIONAIS
Tema: Simulação de Elevador
---
Integrantes:
Vitor Alves Pereira, 10410862
Eduardo Takashi Missaka, 10417877
Tiago Silveira Lopez, 10418683

*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

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

  pthread_t elevator_call_thread; // thread da chamada do elevador
  pthread_t elevator_thread; // thread do elevador

  pthread_mutex_init(&mutex, NULL);

  while (1) {

    // realiza a requisição do elevador em um andar do prédio
    pthread_create(&elevator_call_thread, NULL, &request, NULL);
    pthread_join(elevator_call_thread, NULL);

    // atribui o andar de chamada ao atributo de destino do elevador
    elevator.destination_floor = building.elevator_call;

    // move o elevador ao andar de destino/chamada
    pthread_create(&elevator_thread, NULL, &move_elevator, NULL);
    pthread_join(elevator_thread, NULL);

    sleep(10);
    
  }

  // Encerrando as threads e o mutex
  pthread_mutex_destroy(&mutex);
  pthread_exit((void *)&building);
  pthread_exit((void *)&elevator);

  return 0;
}

// request(prédio) --> realiza a requisição do elevador em um andar gerado aleatoriamente
void *request() {
  pthread_mutex_lock(&mutex);
  short random_floor = rand() % building.floors;
  printf("-------------------------------------------------------\n\n");
  printf(">> Requisição do elevador no andar %d\n", random_floor);
  building.elevator_call = random_floor;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

// move_elevator() --> move o elevador para o andar chamado por meio do incremento ou decremento do atributo current_floor
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
