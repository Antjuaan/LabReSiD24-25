#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 7600

int main() {
    // Un solo file descriptor a differenza di TCP
    int server_fd;

    // Due struct per memorizzare gli indirizzi del server e del client
    struct sockaddr_in server_addr, client_addr;
    int addrlen = sizeof(client_addr);

    // Buffer per la ricezione e l'invio dei messaggi
    char buffer[1024] = {0};
    char *string = "[SERVER] Hello, message received!";

    // Creazione del socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configurazione dell'indirizzo del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Binding del socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Ricezione del messaggio dal client
    recvfrom(server_fd, buffer, 1024, 0, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen);
    printf("%s\n", buffer);

    // Invio di una risposta al client
    sendto(server_fd, string, strlen(string), 0, (struct sockaddr *)&client_addr, addrlen);
    printf("Message sent\n");

    // Chiusura del socket
    close(server_fd);
    return 0;
}