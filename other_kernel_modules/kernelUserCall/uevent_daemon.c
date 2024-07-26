#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define UEVENT_BUFFER_SIZE 2048

int main() {
    int sock;
    struct sockaddr_nl sa;
    char buf[UEVENT_BUFFER_SIZE];
    int len;

    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_groups = 1;

    sock = socket(AF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    if (bind(sock, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    while (1) {
        len = recv(sock, buf, sizeof(buf), 0);
        if (len < 0) {
            perror("recv");
            break;
        }

        buf[len] = '\0';

        // Check if the event is from "call"
        if (strstr(buf, "call") != NULL) {
            printf("Uevent received: %s\n", buf);

            // Execute the user space application
            system("/home/pseudoku/Desktop/GenRobotics/kernelUserCall");
        }
    }

    close(sock);
    return 0;
}

