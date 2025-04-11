#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_THREADS 5

// Definizione del semaforo
sem_t sem;

void* threadFunc(void *arg){
    int i = *(int*)arg;

    // Acquisisco il semaforo
    sem_wait(&sem);

    // Sezione critica
    printf("Thread %d: entra nella sezione critica\n", i);
    sleep(2);
    printf("Thread %d: esce dalla sezione critica\n", i);

    // Rilascio il semaforo
    sem_post(&sem);

    pthread_exit(NULL);
}

int main(){
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Inizializzo il semaforo con un valore iniziale di 2
    sem_init(&sem, 0, 2);

    for (int i = 0; i < NUM_THREADS; i++){
        ids[i] = i;
        pthread_create(&threads[i], NULL, threadFunc, (void*)&ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&sem);
    return 0;
}