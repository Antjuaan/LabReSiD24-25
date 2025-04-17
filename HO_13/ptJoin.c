#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Inizializzazione del mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Inizializzazione della struttura dati in cui il thread attende
pthread_cond_t queue = PTHREAD_COND_INITIALIZER;

// Variabile condizione
int cond = 0;

// Funzione che esegue il thread figlio
void* thread_func(void* arg){
    pthread_mutex_lock(&lock);
    printf("Thread figlio in esecuzione...\n");

    sleep(2);

    printf("Thread figlio terminato!\n");
    cond = 1;
    pthread_cond_signal(&queue);
    pthread_mutex_unlock(&lock);
    return NULL;
}

// Thread padre
int main(){
    pthread_t thread;

    pthread_mutex_lock(&lock);
    printf("Creazione del thread figlio...\n");
    pthread_create(&thread, NULL, thread_func, NULL);
    
    // Attende il completamento del thread figlio, dunque che la condizione si verifichi
    while(!cond){
        printf("Thread padre in attesa...\n");
        pthread_cond_wait(&queue, &lock);
    }
    
    printf("Thread padre svegliato!\n");
    pthread_mutex_unlock(&lock);
    
    return 0;
}