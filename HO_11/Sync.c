#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 10
#define MAX_VALUE 10
 
pthread_mutex_t lock;
int i = 0;

// Funzione che esegue ogni thread
void* counter(void *arg){
    int t = *(int*)arg;

    // Acquisisco il lock
    pthread_mutex_lock(&lock);
    
    for (int j = 0; j < MAX_VALUE; j++){
        printf("Thread %d: i = %d\n", t, i);
        i++;
    }

    // Rilascio il lock
    pthread_mutex_unlock(&lock);

    pthread_exit(NULL);
}

int main(){
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&lock, NULL);

    // Creo i thread
    for (int t = 0; t < NUM_THREADS; t++){
        pthread_create(&threads[t], NULL, counter, (int*)&t);
    }

    // Attendo la terminazione dei thread
    for (int t = 0; t < NUM_THREADS; t++){
        pthread_join(threads[t], NULL);
    }

    printf("Final i = %d\n", i);

    pthread_exit(NULL);
    return 0;
}