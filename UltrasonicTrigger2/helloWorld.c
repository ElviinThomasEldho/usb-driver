#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

int main() {
    while (1) {
        printf("I am still alive\n");
        usleep(1000000); // sleep for 100 ms
    }
    return 0;
}

