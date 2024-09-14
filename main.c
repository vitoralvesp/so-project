#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


// Elevator --> estrutura para conter os atributos do elevador
typedef struct {
  short floor;
  short nextFloor;
  short passengers;

} Elevator;

// Building --> estrutura para conter os atributos do prédio
typedef struct {
  short floors;
  short elevators;
  short floor_passengers;

} Building;

//
typedef struct {
  Building building;
  Elevator elevator;

} Args;


// requisition(prédio) --> realiza a requisição do elevador em um andar gerado aleatoriamente
void *request(void *args);

//
void *move_elevator(void *args);


int main() {

  Building building = {6, 1, 0}; // Prédio de 6 andares e 1 elevador
  Elevator elevator;
  Args args = {building, elevator};
  
  pthread_t floor_thread;  
  pthread_t elevator_thread;  
  
  
  while(1) {

    elevator.floor = 0;
    elevator.nextFloor = 0;
    elevator.passengers = 0;

    printf(">> [INICIO] Elevador está no andar %d\n", elevator.floor);

    // realiza a requisição do elevador em um andar do prédio
    pthread_create(&floor_thread, NULL, &request, (void *) &args);
    pthread_join(floor_thread, NULL);

    //
    pthread_create(&elevator_thread, NULL, &move_elevator, (void *) &args);
    pthread_join(elevator_thread, NULL);

    printf(">> [FIM] Elevador está no andar %d\n", elevator.floor);
    
  }

  pthread_exit(NULL);
  return 0;
  
}

// request(prédio) --> realiza a requisição do elevador em um andar gerado aleatoriamente
void *request(void *args) {

  Building *building = (Building *)args;
  Elevator *elevator = (Elevator *)args;
  
  short random_floor = rand() % building->floors;
  printf("---\n>> Requisição do elevador no andar %d\n", random_floor);
  elevator->nextFloor = random_floor;

  return NULL;
  
}

//
void *move_elevator(void *args) {

  Building *building = (Building *)args;
  Elevator *elevator = (Elevator *)args;

  while(!(elevator->floor == elevator->nextFloor)) {

    if (elevator->floor < elevator->nextFloor) {
      printf(">> Elevador subindo...\n\t- Andar Atual: %d\n\t- Andar de Parada: %d\n\n", elevator->floor, elevator->nextFloor);
      elevator->floor++;
      
    } else {
      printf(">> Elevador descendo...\n\t- Andar Atual: %d\n\t- Andar de Parada: %d\n\n", elevator->floor, elevator->nextFloor);
      elevator->floor--;
      
    }
    sleep(1); // 1 segundo de espera por andar
    
  }
  printf(">> Elevador chegou ao andar de parada: andar %d\n\n", elevator->floor);
  
  return NULL;
}