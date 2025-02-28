#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 7600

int main() {
    int sock = 0;
    struct sockaddr_in server_addr;
    char *string = "[CLIENT] Hello from client!";
    char buffer[1024] = {0};

    // Creazione del socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("\n[CLIENT] Socket creation error\n");
        return -1;
    }

    // Configurazione dell'indirizzo del server
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Conversione dell'indirizzo IP
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        printf("\n[CLIENT] Invalid address/ Address not supported\n");
        return -1;
    }

    // Invio di un messaggio al server
    sendto(sock, string, strlen(string), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("[CLIENT] Message sent to server\n");

    // Ricezione di un messaggio dal server
    recvfrom(sock, buffer, 1024, 0, NULL, NULL);
    printf("[CLIENT] Message received from server: %s\n", buffer);

    close(sock);
    return 0;
}