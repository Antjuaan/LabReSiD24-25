#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 10
#define MAX_VALUE 10000


// Variabile globale condivisa
int i = 0;

// Funzione eseguita dai thread
void* counter(void *arg){
    int t = *(int*)arg;
    for (int j = 0; j < MAX_VALUE; j++){
        printf("Thread %d: i = %d\n", t, i);
        i++;
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t threads[NUM_THREADS];

    // Creo i thread
    for (int t = 0; t < NUM_THREADS; t++){
        pthread_create(&threads[t], NULL, counter, (int*)&t);
    }

    // Aspetto che i thread finiscano
    for (int t = 0; t < NUM_THREADS; t++){
        pthread_join(threads[t], NULL);
    }

    printf("Final i = %d\n", i);

    pthread_exit(NULL);
    return 0;
}