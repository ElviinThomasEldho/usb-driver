#include <linux/netlink.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define NETLINK_USER 31
#define MAX_PAYLOAD 1024 /* maximum payload size */

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

int main() {
    sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
    if (sock_fd < 0) {
        perror("Failed to create socket");
        return -1;
    }

    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = getpid(); /* self pid */

    if (bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr)) < 0) {
        perror("Bind failed");
        close(sock_fd);
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
        return -1;
    }

    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid = getpid();
    nlh->nlmsg_flags = 0;

    strcpy(NLMSG_DATA(nlh), "Hello");

    iov.iov_base = (void *)nlh;
    iov.iov_len = nlh->nlmsg_len;
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;


    /*printf("Waiting for message from kernel\n");

    /* Read message from kernel 
    if (recvmsg(sock_fd, &msg, 0) < 0) {
        perror("Receive failed");
        free(nlh);
        close(sock_fd);
        return -1;
    }

    printf("Received message payload: %s\n", (char *)NLMSG_DATA(nlh));
    */
    
    int i = 1000;
    while (i > 0) {
        printf("Hello, World!\n");
        i--;
        int rand_interval = rand() % 5 + 1;
        if(rand_interval == 1) {
	    printf("Sending message to kernel\n");
	    if (sendmsg(sock_fd, &msg, 0) < 0) {
		perror("Send failed");
		free(nlh);
		close(sock_fd);
		return -1;
	    }
        } 
        
        sleep(1); // sleep for 1 second
    }

    close(sock_fd);
    free(nlh); // Free allocated memory to avoid memory leak
    return 0;
}

