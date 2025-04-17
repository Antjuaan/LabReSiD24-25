#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREADS 4


// Inizializzazione del mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Inizializzazione della struttura dati in cui il thread attende
pthread_cond_t queue = PTHREAD_COND_INITIALIZER;

// Variabile condizione
int cond = 0;


// Funzione che esegue ogni thread
void* thread_func(void* arg){
    int id = *(int*)arg;

    pthread_mutex_lock(&lock);
    printf("Thread %d in attesa...\n", id);

    while(!cond){
        pthread_cond_wait(&queue, &lock);
    }
    
    printf("Thread %d svegliato!\n", id);
    pthread_mutex_unlock(&lock);
    printf("Thread %d terminato!\n", id);
    return NULL;
}

// Thread main
int main(){
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    // Creazione dei thread
    for(int i = 0; i < NUM_THREADS; i++){
        ids[i] = i;
        pthread_create(&threads[i], NULL, thread_func, &ids[i]);
    }
    
    pthread_mutex_lock(&lock);
    cond = 1;
    printf("Condizione soddisfatta, sveglio i thread...\n");
    pthread_cond_broadcast(&queue);
    pthread_mutex_unlock(&lock);

    // Attesa del completamento dei thread
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}