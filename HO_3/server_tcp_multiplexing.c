#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

// Funzione per rendere un socket non bloccante
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
    int fdsocket, newsocket;
    int fdclients[MAX_CLIENTS] = {0};
    struct sockaddr_in server_addr, client_addr;
    socklen_t clilen = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Creo il socket del server
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
        perror("Binding failed!");
        exit(EXIT_FAILURE);
    }

    // Metto il server in ascolto
    if (listen(fdsocket, MAX_CLIENTS) < 0) {
        perror("Listening failed!");
        exit(EXIT_FAILURE);
    }

    // Imposto il socket non bloccante
    set_nonblock(fdsocket);
    printf("Server con select() in ascolto sulla porta %d\n", PORT);

    // Insieme dei file descriptor da monitorare
    fd_set readfds;
    int maxfd = fdsocket;

    while (1){
        FD_ZERO(&readfds);
        FD_SET(fdsocket, &readfds);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (fdclients[i] > 0) {
                FD_SET(fdclients[i], &readfds);
                if (fdclients[i] > maxfd) {
                    maxfd = fdclients[i];
                }
            }
        }

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        // Monitoro i file descriptor
        if (select(maxfd + 1, &readfds, NULL, NULL, &timeout) < 0) {
            perror("Select failed!");
            exit(EXIT_FAILURE);
        }

        // Controllo se ci sono nuove connessioni
        if (FD_ISSET(fdsocket, &readfds)) {
            if ((newsocket = accept(fdsocket, (struct sockaddr *)&client_addr, &clilen)) < 0) {
                perror("Accept failed!");
                exit(EXIT_FAILURE);
            }

            // Imposto il nuovo socket non bloccante
            set_nonblock(newsocket);

            printf("Client %d connesso\n", newsocket);

            // Aggiungo il nuovo socket all'array dei client
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (fdclients[i] == 0) {
                    fdclients[i] = newsocket;
                    break;
                }
            }
        }

        // Controllo i client connessi per vedere se ci sono dati da leggere
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (FD_ISSET(fdclients[i], &readfds)) {
                int n = read(fdclients[i], buffer, BUFFER_SIZE);
                if (n == 0) {
                    printf("Client %d disconnesso\n", fdclients[i]);
                    close(fdclients[i]);
                    fdclients[i] = 0;
                } else {
                    buffer[n] = '\0';
                    send(fdclients[i], "ACK\n", 4, 0);
                    printf("Client %d: %s", fdclients[i], buffer);
                }
            }
        }
    }
}