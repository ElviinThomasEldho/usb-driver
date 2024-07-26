#include <stdio.h>
#include <unistd.h> // for sleep()

int main() {
    while (1) {
        printf("Hello, World!\n");
        sleep(1); // sleep for 1 second
    }
    return 0;
}

