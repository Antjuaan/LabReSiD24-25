#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 5

// Semaforo per la barriera
sem_t barrier;

// Funzione eseguita dai thread
void* threadFunc(void* arg) {
    int id = *(int*)arg;

    printf("Thread %d: in attesa di partire...\n", id);

    // Metto i thread in attesa
    sem_wait(&barrier);

    printf("Thread %d: partito!\n", id);
    return NULL;
}

// Funzione principale
int main() {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Semaforo inizializzato a 0
    sem_init(&barrier, 0, 0);

    // Creo i thread
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, threadFunc, &ids[i]);
    }

    // Aspetto un attimo prima di sbloccare i thread
    sleep(2);
    printf("MAIN: Tutti i thread possono partire!\n");

    // Sblocco tutti i thread con N sem_post()
    for (int i = 0; i < NUM_THREADS; i++) {
        sem_post(&barrier);
    }

    // Aspetto la fine dei thread
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Distruggo il semaforo
    sem_destroy(&barrier);
    return 0;
}