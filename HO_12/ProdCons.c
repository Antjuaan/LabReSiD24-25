#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5
#define ELEMENTS 10

// Buffer condiviso
int buffer[BUFFER_SIZE];

// Successiva posizione libera nel buffer
int in = 0;

// Prima posizione piena nel buffer
int out = 0;

// Numero di posizioni vuote nel buffer
sem_t empty;

// Numero di posizioni piene nel buffer
sem_t full;

// Mutex per proteggere l'accesso al buffer
pthread_mutex_t lock;


// Funzione del produttore
void* producer(void* arg) {
    for (int i = 0; i < ELEMENTS; i++) {
        // Elemento prodotto
        int elem = i;

        // Attende che ci sia almeno una posizione vuota nel buffer - decrementa il contatore delle posizioni vuote
        sem_wait(&empty);

        // Acquisisce il lock per accedere al buffer
        pthread_mutex_lock(&lock);

        // Inserisce l'elemento nel buffer
        buffer[in] = elem;
        printf("Produttore: inserito %d nella posizione %d\n", elem, in);

        // Aggiorna la posizione di inserimento
        in = (in + 1) % BUFFER_SIZE;

        // Rilascia il lock
        pthread_mutex_unlock(&lock);

        // Incrementa il contatore delle posizioni piene
        sem_post(&full);

        sleep(1);
    }
    return NULL;
}


// Funzione del consumatore
void* consumer(void* arg) {
    for (int i = 0; i < ELEMENTS; i++) {
        // Attende che ci sia almeno un elemento da consumare - decrementa il contatore delle posizioni piene
        sem_wait(&full);

        // Acquisisce il lock per accedere al buffer
        pthread_mutex_lock(&lock);

        // Legge l'elemento dal buffer
        int elem = buffer[out];
        printf("Consumatore: estratto %d dalla posizione %d\n", elem, out);

        // Aggiorna la posizione di estrazione
        out = (out + 1) % BUFFER_SIZE;

        // Rilascia il lock
        pthread_mutex_unlock(&lock);

        // Incrementa il contatore delle posizioni vuote
        sem_post(&empty);

        sleep(1);
    }
    return NULL;
}


// Main
int main() {
    pthread_t prod_thread, cons_thread;

    // Inizializzazione dei semafori e del mutex
    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    pthread_mutex_init(&lock, NULL);

    // Creazione dei thread produttore e consumatore
    pthread_create(&prod_thread, NULL, producer, NULL);
    pthread_create(&cons_thread, NULL, consumer, NULL);

    // Attesa della terminazione dei thread
    pthread_join(prod_thread, NULL);
    pthread_join(cons_thread, NULL);

    // Distruzione dei semafori e del mutex
    sem_destroy(&empty);
    sem_destroy(&full);
    pthread_mutex_destroy(&lock);

    return 0;
}