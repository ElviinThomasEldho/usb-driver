#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <net/sock.h>
#include <linux/slab.h>

#define PORT 8888

static struct socket *server_socket = NULL;
static struct task_struct *server_thread_task = NULL;

static int server_thread(void *arg) {
    struct socket *new_socket;
    struct sockaddr_in saddr;
    struct kvec iov;
    struct msghdr msg;
    int len;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(PORT);

    kernel_accept(server_socket, &new_socket, 0);

    /* Example of receiving data */
    iov.iov_base = kmalloc(128, GFP_KERNEL);
    iov.iov_len = 128;

    memset(&msg, 0, sizeof(msg));

    len = kernel_recvmsg(new_socket, &msg, &iov, 1, 128, 0);

    if (len > 0) {
        printk(KERN_INFO "Received message from user-space: %s\n", (char *)iov.iov_base);
        
        // Run the user-space application
        char *argv[] = {"/home/elviin/Desktop/USB Read Write/USB Read Write.x86_64", NULL};
        static char *envp[] = {
            "HOME=/",
            "PATH=/sbin:/bin:/usr/sbin:/usr/bin", NULL
        };

        call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    }

    kfree(iov.iov_base);
    sock_release(new_socket);

    return 0;
}

static int __init server_init(void) {
    struct sockaddr_in saddr;
    int error;

    error = sock_create(AF_INET, SOCK_STREAM, IPPROTO_TCP, &server_socket);
    if (error < 0) {
        printk(KERN_ERR "Error creating socket\n");
        return error;
    }

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(PORT);

    error = kernel_bind(server_socket, (struct sockaddr *)&saddr, sizeof(saddr));
    if (error < 0) {
        printk(KERN_ERR "Error binding socket\n");
        sock_release(server_socket);
        return error;
    }

    error = kernel_listen(server_socket, 5);
    if (error < 0) {
        printk(KERN_ERR "Error listening on socket\n");
        sock_release(server_socket);
        return error;
    }

    server_thread_task = kthread_run(server_thread, NULL, "server_thread");
    if (IS_ERR(server_thread_task)) {
        printk(KERN_ERR "Failed to create server thread\n");
        sock_release(server_socket);
        return PTR_ERR(server_thread_task);
    }

    return 0;
}

static void __exit server_exit(void) {
    if (server_thread_task) {
        kthread_stop(server_thread_task);
        server_thread_task = NULL;
    }
    
    if (server_socket) {
        // Properly close the socket before releasing
        kernel_sock_shutdown(server_socket, SHUT_RDWR);
        sock_release(server_socket);
        server_socket = NULL;
    }
}

module_init(server_init);
module_exit(server_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel module for socket communication with user-space execution");

