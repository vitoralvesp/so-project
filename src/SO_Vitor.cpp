#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h> // Inclua time.h para srand()

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
void *request(void *args); // Aceita um argumento
void *move_elevator(void *args); // Aceita um argumento

// Escopo Global
pthread_mutex_t mutex;
Elevator elevator = {0, 0, 0}; // Inicia o elevador
Building building = {6, 0, 0}; // Inicia o prédio

int main() {
    pthread_t elevator_call_thread; // thread da chamada do elevador
    pthread_t elevator_thread; // thread do elevador

    pthread_mutex_init(&mutex, NULL);
    srand(time(NULL)); // Inicializa a semente para o gerador de números aleatórios

    while (1) {
        // realiza a requisição do elevador em um andar do prédio
        pthread_create(&elevator_call_thread, NULL, request, (void *)&building); // Passa a estrutura building
        pthread_join(elevator_call_thread, NULL);

        // atribui o andar de chamada ao atributo de destino do elevador
        elevator.destination_floor = building.elevator_call;

        // move o elevador ao andar de destino/chamada
        pthread_create(&elevator_thread, NULL, move_elevator, (void *)&elevator); // Passa a estrutura elevator
        pthread_join(elevator_thread, NULL);

        sleep(10);
    }

    // Encerrando as threads e o mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}

// request(prédio) --> realiza a requisição do elevador em um andar gerado aleatoriamente
void *request(void *args) { // Agora aceita um argumento
    pthread_mutex_lock(&mutex);
    short random_floor = rand() % ((Building *)args)->floors; // Converte args para Building*
    printf("-------------------------------------------------------\n\n");
    printf(">> Requisição do elevador no andar %d\n", random_floor);
    ((Building *)args)->elevator_call = random_floor; // Atualiza a chamada do elevador
    pthread_mutex_unlock(&mutex);

    return NULL; // Retorna NULL
}

// move_elevator() --> move o elevador para o andar chamado por meio do incremento ou decremento do atributo current_floor
void *move_elevator(void *args) { // Agora aceita um argumento
    pthread_mutex_lock(&mutex);

    Elevator *elevator_ptr = (Elevator *)args; // Converte args para Elevator*

    while (!(elevator_ptr->current_floor == elevator_ptr->destination_floor)) {
        if (elevator_ptr->current_floor < elevator_ptr->destination_floor) {
            printf("\t- Elevador subindo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", elevator_ptr->current_floor, elevator_ptr->destination_floor);
            elevator_ptr->current_floor++;
        } else {
            printf("\t- Elevador descendo...\n\t\t- Andar Atual: %hi\n\t\t- Andar de Parada: %hi\n", elevator_ptr->current_floor, elevator_ptr->destination_floor);
            elevator_ptr->current_floor--;
        }
        sleep(1); // 1 segundo de espera por andar
    }
    printf("\t- Elevador chegou ao andar de parada: andar %d\n\n", elevator_ptr->current_floor);
    pthread_mutex_unlock(&mutex);

    return NULL; // Retorna NULL
}

