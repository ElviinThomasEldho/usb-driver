#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define SYSFS_PATH "/sys/kernel/my_module/my_value"

int main() {
    int fd;
    char buf[256];

    fd = open(SYSFS_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open sysfs attribute");
        return -1;
    }

    // Read from sysfs attribute
    read(fd, buf, sizeof(buf));
    printf("Current value: %s", buf);

    // Write to sysfs attribute
    sprintf(buf, "42\n");
    write(fd, buf, sizeof(buf));

    close(fd);
    return 0;
}

