#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size */
#define SERIAL_PORT "/dev/ttyACM0" /* replace with your serial port */

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

int configure_serial_port(int fd) {
    struct termios options;
    tcgetattr(fd, &options);

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    tcsetattr(fd, TCSANOW, &options);
    return 0;
}

int main() {
    int serial_fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    if (serial_fd == -1) {
        perror("Unable to open serial port");
        return -1;
    }

    configure_serial_port(serial_fd);

    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        close(serial_fd);
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("Bind failed");
        close(sock_fd);
        close(serial_fd);
        return -1;
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; /* For Linux Kernel */
    dest_addr.nl_groups = 0; /* unicast */

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh) {
        perror("Failed to allocate memory for nlmsghdr");
        close(sock_fd);
        close(serial_fd);
        return -1;
    }

    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nlh), "Object Detected");

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    

    char buffer[256];
    while (1) {
        int n = read(serial_fd, buffer, 255);
        if (n > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
            if (strstr(buffer, "Object Detected") != NULL) {
                printf("Sending message to kernel\n");
                if (sendmsg(sock_fd, &msg, 0) < 0) {
                    perror("Send failed");
                    free(nlh);
                    close(sock_fd);
                    close(serial_fd);
                    return -1;
                }
            }
        }
        usleep(100000); // sleep for 100 ms
    }

    close(sock_fd);
    close(serial_fd);
    free(nlh); // Free allocated memory to avoid memory leak
    return 0;
}

