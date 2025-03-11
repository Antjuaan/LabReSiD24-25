#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>

#define PORT 8080
#define MAX_EVENTS 10
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
    struct epoll_event event, events[MAX_EVENTS];
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
    if (listen(fdsocket, MAX_EVENTS) < 0) {
        perror("Listening failed!");
        exit(EXIT_FAILURE);
    }

    // Imposto il socket non bloccante
    set_nonblock(fdsocket);
    printf("Server con epoll() in ascolto sulla porta %d\n", PORT);

    // Creo l'istanza di epoll
    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("Epoll creation failed!");
        exit(EXIT_FAILURE);
    }

    // Specifico il tipo di evento da monitorare
    event.events = EPOLLIN;
    event.data.fd = fdsocket;

    // Aggiungo il socket del server all'istanza di epoll
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fdsocket, &event) == -1) {
        perror("Epoll control failed!");
        exit(EXIT_FAILURE);
    }

    // Ciclo principale
    while (1) {
        // Aspetto che si verifichi un evento I/O su uno dei socket
        int num_events = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("Epoll wait failed!");
            exit(EXIT_FAILURE);
        }

        // Ciclo sugli eventi I/O ritoranti da epoll_wait
        for (int i = 0; i < num_events; i++) {
            // Se l'evento è relativo al socket del server allora è una richiesta di connessione
            if (events[i].data.fd == fdsocket) {
                newsocket = accept(fdsocket, (struct sockaddr *)&client_addr, &clilen);
                if (newsocket == -1) {
                    perror("Accept failed!");
                    exit(EXIT_FAILURE);
                }

                // Imposto il socket non bloccante
                set_nonblock(newsocket);

                printf("Client %d connesso\n", newsocket);

                // Specifico il tipo di evento da monitorare
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = newsocket;

                // Aggiungo il socket del client all'istanza di epoll
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, newsocket, &event) == -1) {
                    perror("Epoll control failed!");
                    exit(EXIT_FAILURE);
                }
            } else {
                // Se l'evento è associato ad un socket di un client allora è una richiesta di lettura
                int fd = events[i].data.fd;
                int nread = read(fd, buffer, BUFFER_SIZE);
                if (nread == -1) {
                    perror("Read failed!");
                    exit(EXIT_FAILURE);
                } else if (nread == 0) {
                    // Il client ha chiuso la connessione
                    printf("Client %d disconnesso\n", fd);
                    close(fd);
                } else {
                    // Stampo il messaggio ricevuto e invio un "ACK" al client
                    buffer[nread] = '\0';
                    send(fd, "ACK\n", 4, 0);
                    printf("Client %d: %s", fd, buffer);
                }
            }
        }
        
    }
}