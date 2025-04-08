#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "structDatas.h"

// Dichiarazione delle funzioni di computazione
void* multiply(void *arg);
void* add(void *arg);
void* subtract(void *arg);


// Funzione principale
int main(){
    // Definizione dei thread
    pthread_t thread1, thread2, thread3;
    
    // Inizializzazione dei dati con la struct
    Datas data1 = {2, 6, 0};
    Datas data2 = {1, 4, 0};
    Datas data3 = {5, 2, 0};
    Datas data4 = {0, 0, 0};
    Datas data5 = {0, 0, 0};

    // Creazione dei thread
    pthread_create(&thread1, NULL, multiply, (void*)&data1);
    pthread_create(&thread2, NULL, add, (void*)&data2);
    pthread_create(&thread3, NULL, subtract, (void*)&data3);

    // Attesa della terminazione dei thread
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    
    // Assegno a data4 a e b i risultati di data2 e data3
    data4.a = data2.result;
    data4.b = data3.result;

    // Assegno a data5 a il risultato di data1
    data5.a = data1.result;

    // Creazione del thread per la moltiplicazione
    pthread_create(&thread1, NULL, multiply, (void*)&data4);
    
    // Attesa della terminazione del thread
    pthread_join(thread1, NULL);

    // Assegno a data5 b il risultato di data4
    data5.b = data4.result;

    // Creazione del thread per la somma
    pthread_create(&thread2, NULL, add, (void*)&data5);

    // Attesa della terminazione del thread
    pthread_join(thread2, NULL);

    // Stampa del risultato
    printf("Risultato finale: %d\n", data5.result);

    // Terminazione dei thread
    pthread_exit(NULL);
    return 0;
}