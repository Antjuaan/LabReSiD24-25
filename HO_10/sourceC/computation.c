#include <pthread.h>
#include "structDatas.h"

// Funzione per la moltiplicazione
void* multiply(void* arg){
    Datas* data = (Datas*)arg;
    data->result = data->a * data->b;

    pthread_exit(NULL);
}

// Funzione per la somma
void* add(void* arg){
    Datas* data = (Datas*)arg;
    data->result = data->a + data->b;

    pthread_exit(NULL);
}

// Funzione per la sottrazione
void* subtract(void* arg){
    Datas* data = (Datas*)arg;
    data->result = data->a - data->b;

    pthread_exit(NULL);
}