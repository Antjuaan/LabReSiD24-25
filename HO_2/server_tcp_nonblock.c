#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7500
#define BUFFER_SIZE 128


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


// Funzione per settare il socket in modalità non bloccante
void set_nonblock(int fdsocket) {
    int flags = fcntl(fdsocket, F_GETFL, 0);
    if (flags == -1) {
        perror("Fcntl failed!");
        exit(EXIT_FAILURE);
    }

    if (fcntl(fdsocket, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("Fcntl failed!");
        exit(EXIT_FAILURE);
    }
}


int main() {
    int fdsocket, fdclient;
    struct sockaddr_in server_addr, client_addr;
    socklen_t clilen = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

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

    // Setto il socket in modalità non bloccante
    set_nonblock(fdsocket);
    printf("Server non bloccante in ascolto sulla porta: %d\n", PORT);

    while (1) {
        // Accetto la connessione
        if ((fdclient = accept(fdsocket, (struct sockaddr *)&client_addr, &clilen)) < 0) {
            if (errno == EWOULDBLOCK) {
                // Nessuna connessione in arrivo, continuo il ciclo
                continue;
            } else {
                perror("Accept failed!");
                exit(EXIT_FAILURE);
            }
        }

        printf("Client connesso!\n");

        // Setto il socket del client in modalità non bloccante
        set_nonblock(fdclient);

        // Inizializzo la sequenza di Fibonacci per ogni nuova connessione
        int fib1 = 0, fib2 = 1;
        int first = 1;
        int second = 1;

        // Leggo i dati inviati dal client con attesa attiva
        while(1) {
            int n = recv(fdclient, buffer, BUFFER_SIZE - 1, 0);
            if (n > 0){
                buffer[n] = '\0';

                int client_number = atoi(buffer);
                int next = next_fibonacci(&fib1, &fib2, &first, &second);

                printf("Ricevuto: %d, Aspettato: %d\n", client_number, next);

                if (client_number == next) {
                    write(fdclient, "OK\n", 3);
                } else {
                    printf("Numero errato, chiusura connessione.\n");
                    close(fdclient);
                    break;
                }
            } else if (n == 0) {
                printf("Client disconnesso\n");
                break;
            } else {
                if (errno == EWOULDBLOCK) {
                    // Nessun dato in arrivo, continuo il ciclo
                    continue;
                } else {
                    perror("Recv failed!");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}