//  Developed libraries
#include "UART_common.h"

//  Global Static Variables
static uint8_t errorArray[120] = {0};                                                               //  Error array to help print specific function
static uint8_t parity = 0;

/*
    Function: Initialize UART struct and open UART device
    uart_info: Struct that hold file descriptor and uart information
    uart_device: uart device that will be used
    baud_rate: baud rate (bits per second) that will be used
*/
int32_t UART_init(uart_info_t *uart_info, const uint8_t *uart_device, uint32_t baud_rate) {
    memcpy(uart_info->uart_device, uart_device, strlen(uart_device) + 1);
    uart_info->user_baud_rate = baud_rate;

	if ((uart_info->uart_fd = open(uart_info->uart_device, O_RDWR | O_NOCTTY | O_SYNC)) < 0) {      //  Open UART device
        snprintf(errorArray, sizeof(errorArray), "%s: Open UART Device\n", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
	}

    struct termios options;                                                                         //  Initialize uart options struct
    if (tcgetattr (uart_info->uart_fd, &options) < 0) {                                             //  Get current attributes
        snprintf(errorArray, sizeof(errorArray), "%s: Get UART Attributes\n", __FUNCTION__);        //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    switch(uart_info->user_baud_rate) {
        case 4800:
            uart_info->uart_baud_rate = B4800;
        break;

        case 115200:
            uart_info->uart_baud_rate = B115200;
        break;

        case 230400:
            uart_info->uart_baud_rate = B230400;
        break;

        case 460800:
            uart_info->uart_baud_rate = B460800;
        break;

        case 921600:
            uart_info->uart_baud_rate = B921600;
        break;

        case 2000000:
            uart_info->uart_baud_rate = B2000000;
        break;

        case 4000000:
            uart_info->uart_baud_rate = B4000000;
        break;

        default:
            snprintf(errorArray, sizeof(errorArray), "%s: Invalid Baud Rate\n", __FUNCTION__);      //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
            return -1;                                                                              //  Return error
        break;
    }

    cfsetospeed (&options, uart_info->uart_baud_rate);                                              //  Set outbound baud rate using baudRate
    cfsetispeed (&options, uart_info->uart_baud_rate);                                              //  Set inbound baud rate using baudRate

    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;                                             //  8-bit chars
    options.c_iflag &= ~IGNBRK;                                                                     //  disable break processing
    options.c_lflag = 0;                                                                            //  no signaling chars, no echo,
    options.c_oflag = 0;                                                                            //  no remapping, no delays
    options.c_cc[VMIN]  = 0;                                                                        //  read doesn't block
    options.c_cc[VTIME] = 5;                                                                        //  0.5 seconds read timeout
    options.c_iflag &= ~(IXON | IXOFF | IXANY);                                                     //  shut off xon/xoff ctrl
    options.c_cflag |= (CLOCAL | CREAD);                                                            //  ignore modem controls,
    options.c_cflag &= ~(PARENB | PARODD);                                                          //  shut off parity
    options.c_iflag &= ~(INLCR | ICRNL);                                                            //  Turn off carriage
    options.c_cflag |= parity;                                                                      //  IDK
    options.c_cflag &= ~CSTOPB;                                                                     //  IDK

    if (tcsetattr (uart_info->uart_fd, TCSANOW, &options) < 0) {                                    //  Set new attributes
        snprintf(errorArray, sizeof(errorArray), "%s: Set UART Attributes\n", __FUNCTION__);        //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    tcflush(uart_info->uart_fd, TCIOFLUSH);                                                         //  Flush out any previous read/write messages
    return 1;                                                                                       //  Return good
}

/*
    Function: Close UART file descriptor
    uart_info: Struct that hold file descriptor and uart information
    uart_device: uart device that will be used
    baud_rate: baud rate (bits per second) that will be used
*/
void UART_close(uart_info_t *uart_info) {
    close(uart_info->uart_fd);                                                                      //  Close serial socket
}

/*
    Function: Send UART messages
    uart_info: Struct that hold file descriptor and uart information
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
*/
int32_t UART_send(uart_info_t *uart_info, uint8_t *send_msg, uint32_t send_len) {
    ssize_t sentBytes = write(uart_info->uart_fd, send_msg, send_len);                              //  Write to socket
    if (sentBytes < 0) {                                                                            //  If sentBytes flag is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Error Sending\n", __FUNCTION__);              //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    return 1;                                                                                       //  Return good
}

/*
    Function: Receive UART messages and have read as blocking
    uart_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
*/
int32_t UART_recv_blocking(uart_info_t *uart_info, uint8_t *recv_msg, uint32_t msglen) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(uart_info->uart_fd, &reading);                                                           //  Set reading struct to monitor uart_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    int32_t ready = select(uart_info->uart_fd + 1, &reading, NULL, NULL, NULL);                     //  Wait until uart_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else {
        if (FD_ISSET(uart_info->uart_fd, &reading)) {                                               //  Check if the uart_fd is ready
            recvBytes = read(uart_info->uart_fd, recv_msg, msglen);                                 //  Read message and populate recv_buff and recvBytes
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Receive UART messages and have read as non blocking
    uart_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
    secs: Timeout seconds
    usecs: Timeout useconds
*/
int32_t UART_recv_soft_blocking(uart_info_t *uart_info, uint8_t *recv_msg, uint32_t msglen, uint32_t secs, uint32_t usecs) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(uart_info->uart_fd, &reading);                                                           //  Set reading struct to monitor uart_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    struct timeval timeout;                                                                         //  Initialize timeout struct
    timeout.tv_sec = secs;                                                                          //  Set timeout seconds
    timeout.tv_usec = usecs;                                                                        //  Set timeout useconds
    int32_t ready = select(uart_info->uart_fd + 1, &reading, NULL, NULL, &timeout);                 //  Wait until uart_fd has a message or timeout
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else if (ready == 0) {                                                                          //  If uart_fd is not ready
        printf("%s: Timeout Occurred\n", __FUNCTION__);                                             //  Print Timeout
    }
    else {
        if (FD_ISSET(uart_info->uart_fd, &reading)) {                                               //  Check if the uart_fd is ready
            recvBytes = read(uart_info->uart_fd, recv_msg, msglen);                                 //  Read message and populate recv_buff and recvBytes
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}