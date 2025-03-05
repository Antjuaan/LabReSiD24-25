#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7500
#define BUFF_SIZE 128

// Funzione per generare la sequenza di Fibonacci
int next_fibonacci(int *a, int *b, int *first, int *second) {
    if (*first) {
        *first = 0;
        return 0;
    }
    if (*second) {
        *second = 0;
        return 1;
    }

    int next = *a + *b;
    *a = *b;
    *b = next;
    return next;
}


int main() {
    int fdsocket, fdclient;
    struct sockaddr_in server_addr, client_addr;
    socklen_t clilen = sizeof(client_addr);
    char buffer[BUFF_SIZE];

    // Creo il socket
    if ((fdsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Binding
    if (bind(fdsocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed!");
        exit(EXIT_FAILURE);
    }

    // Metto il server in ascolto
    if (listen(fdsocket, 1) < 0) {
        perror("Listen failed!");
        exit(EXIT_FAILURE);
    }

    printf("Server bloccante in ascolto sulla porta: %d\n", PORT);

    while (1) {
        // Accetto una connessione in modo bloccante
        if ((fdclient = accept(fdsocket, (struct sockaddr *)&client_addr, &clilen)) < 0) {
            perror("Accept failed!");
            continue;
        }

        printf("Client connesso\n");

        // Inizializzo la sequenza di Fibonacci per ogni nuova connessione
        int fib1 = 0, fib2 = 1;
        int first = 1;
        int second = 1;

        // Leggo i dati che mi arrivano dal client
        while (1) {
            int n = read(fdclient, buffer, BUFF_SIZE - 1);
            if (n <= 0) {
                printf("Client disconnesso\n");
                break;
            }

            buffer[n] = '\0';
            int client_number = atoi(buffer);

            // Ottengo il prossimo numero della sequenza di Fibonacci
            int next = next_fibonacci(&fib1, &fib2, &first, &second);

            printf("Ricevuto: %d, Aspettato: %d\n", client_number, next);

            if (client_number == next) {
                write(fdclient, "OK\n", 3);
            } else {
                printf("Numero errato, chiusura connessione.\n");
                close(fdclient);
                break;
            }
        }
    }
    close(fdsocket);
    return 0;
}