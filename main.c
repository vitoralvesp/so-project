#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {

  short building_floors;

  short elevator_floor;
  short elevator_next_floor;

  short passengers;

} Args;

// requisition(prédio) --> realiza a requisição do elevador em um andar gerado
// aleatoriamente
void *request(void *args);

//
void *move_elevator(void *args);

int main() {

  Args args = {6, 0, 0, 0};

  pthread_t floor_thread;
  pthread_t args_thread;

  while (1) {

    // realiza a requisição do elevador em um andar do prédio
    pthread_create(&floor_thread, NULL, &request, (void *)&args);
    pthread_join(floor_thread, NULL);

    //
    pthread_create(&args_thread, NULL, &move_elevator, (void *)&args);
    pthread_join(args_thread, NULL);
    
  }

  pthread_exit(NULL);
  return 0;
}

// request(prédio) --> realiza a requisição do elevador em um andar gerado
// aleatoriamente
void *request(void *args) {

  Args *request_args = (Args *)args;
  
  short random_floor = rand() % request_args->building_floors;
  printf("-------------------------------------------------------\n\n");
  printf(">> Requisição do elevador no andar %d\n", random_floor);
  request_args->elevator_next_floor = random_floor;

  return NULL;
}

//
void *move_elevator(void *args) {

  Args *move_args = (Args *)args;

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
}
