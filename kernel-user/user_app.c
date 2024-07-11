int main() {
    int fd;
    char write_buf[] = "Hello from user space!";
    char read_buf[256] = {0};
    ssize_t ret;

    printf("Opening the device...\n");
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open the device");
        return errno;
    }

    printf("Writing to the device...\n");
    ret = write(fd, write_buf, strlen(write_buf));
    if (ret < 0) {
        perror("Failed to write to the device");
        close(fd);
        return errno;
    }
    printf("Wrote message to the device: %s\n", write_buf);

    printf("Reading from the device...\n");
    ret = read(fd, read_buf, sizeof(read_buf) - 1);
    if (ret < 0) {
        perror("Failed to read from the device");
        close(fd);
        return errno;
    }
    printf("Read message from the device: %s\n", read_buf);

    printf("Closing the device...\n");
    close(fd);

    return 0;
}

