#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888

int main() {
    int sockfd;
    struct sockaddr_in servaddr;
    char message[] = "Hello from user-space!";

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    if (send(sockfd, message, strlen(message), 0) == -1) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }

    printf("Message sent to kernel module\n");

    close(sockfd);
    return 0;
}

