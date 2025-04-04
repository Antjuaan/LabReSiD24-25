#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#define MAX_NUMBER 100000
#define MAX_THREAD 4

int rangePerThread = MAX_NUMBER / MAX_THREAD;

// Struct per passare più parametri al thread
typedef struct {
    int start;
    int core_id;
} thread_params;

// Verifica se un numero è primo
bool is_prime(int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

// Funzione che esegue ogni thread
void* check_primes(void* arg) {
    thread_params* data = (thread_params*) arg;
    int start = data->start;
    int end = start + rangePerThread;
    int core = data->core_id;

    // Imposta affinità del thread
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) != 0) {
        perror("Errore nell'impostare l'affinità");
    }

    int actual_core = sched_getcpu();
    printf("Thread per range [%d, %d) in esecuzione sul core %d\n", start, end, actual_core);

    // Cerca numeri primi nel range
    for (int i = start; i < end; i++) {
        if (is_prime(i)) {
            printf("%d is prime (thread su core %d)\n", i, actual_core);
        }
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t threads[MAX_THREAD];
    thread_params thread_data[MAX_THREAD];

    for (int i = 0; i < MAX_THREAD; i++) {
        thread_data[i].start = i * rangePerThread;
        thread_data[i].core_id = i;

        if (pthread_create(&threads[i], NULL, check_primes, &thread_data[i]) != 0) {
            perror("Errore creazione thread");
            return 1;
        }
    }

    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}