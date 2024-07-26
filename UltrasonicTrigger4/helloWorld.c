#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string.h> // Include for memset

int main() {
    const char *device = "/dev/ttyUSB0"; // Replace with your Arduino device path
    int serial_port = open(device, O_RDWR);
    
    if (serial_port < 0) {
        perror("Failed to open serial device");
        return 1;
    }
    
    struct termios tty;
    struct termios tty_old;
    memset(&tty, 0, sizeof(tty)); // Clears structure memory
    
    if (tcgetattr(serial_port, &tty) != 0) {
        perror("Error in tcgetattr");
        return 1;
    }
    
    tty_old = tty;
    
    // Set baud rate - 9600 bps
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    
    // Setting other serial port parameters
    tty.c_cflag |= (CLOCAL | CREAD); // Enable receiver and set local mode
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8; // 8-bit characters
    tty.c_cflag &= ~PARENB; // No parity bit
    tty.c_cflag &= ~CSTOPB; // 1 stop bit
    
    // Applying settings
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        perror("Error in tcsetattr");
        return 1;
    }
    
    // Infinite loop to send a single character 'A'
    char single_char = 'A'; // Character to send
    while (1) {
        write(serial_port, &single_char, 1);
        usleep(600000); // Delay to control the rate of sending
    }
    
    close(serial_port);
    
    return 0;
}

