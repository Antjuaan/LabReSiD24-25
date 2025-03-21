#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MULTICAST_GROUP "239.0.0.1"
#define PORT 12345
#define MAX_MESSAGES 5

int main() {
    int sockfd;
    int msg_count = 0;
    char buffer[256];

    struct ip_mreq multicast_req;
    struct sockaddr_in local_addr;

    // Creo un socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed!");
        exit(EXIT_FAILURE);
    }

    // Permetto a più processi di utilizzare la stessa porta
    int reuse = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        perror("Setsockopt failed!");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Setto l'indirizzo locale
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(PORT);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associa l'indirizzo locale al socket
    if (bind(sockfd, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("Bind failed!");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Setto l'indirizzo multicast
    multicast_req.imr_multiaddr.s_addr = inet_addr(MULTICAST_GROUP);
    multicast_req.imr_interface.s_addr = htonl(INADDR_ANY);

    // Aggiunge il socket al gruppo multicast
    if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_req, sizeof(multicast_req)) < 0) {
        perror("Setsockopt failed!");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Riceve i dati dal gruppo multicast
    while (msg_count < MAX_MESSAGES) {
        int n = recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0, NULL, NULL);
        if (n < 0) {
            perror("Recvfrom failed!");
            break;
        }
        buffer[n] = '\0';
        printf("Received [%d]: %s\n", msg_count + 1, buffer);
        msg_count++;
    }

    // Abbandona il gruppo multicast dopo 5 messaggi
    if (setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &multicast_req, sizeof(multicast_req)) < 0) {
        perror("Setsockopt IP_DROP_MEMBERSHIP failed!");
    } else {
        printf("Abbandonato il gruppo multicast dopo %d messaggi.\n", MAX_MESSAGES);
    }

    close(sockfd);
    return 0;
}