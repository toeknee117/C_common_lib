//  Developed Libraries
#include "TCP_common.h"

static uint8_t errorArray[120] = {0};                                                               //  Error array to help print specific function

/*
    Function: Initialize TCP Client struct and connection.
    tcp_info: Struct that hold file descriptor and addr information
    ip: IP address of the server that its connecting to in X.X.X.X (127.0.0.1)
    port: Port that it is using (Range: 0 - 65535)
*/
int32_t TCP_client_init(tcp_info_t *tcp_info, const uint8_t *ip, uint16_t port) {
    if (TCP_validate_ip(ip) < 0) {                                                                  //  Check for valid IP
        snprintf(errorArray, sizeof(errorArray), "%s: Invalid IP Address\n", __FUNCTION__);         //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if ((tcp_info->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {                    //  Initialize client socket
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Creation Failed\n", __FUNCTION__);     //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info struct
    addr_info.sin_family = AF_INET;                                                                 //  Set address family to ipv4 address
    addr_info.sin_port = htons(port);                                                               //  Set port family to host to network short

    if (inet_pton(AF_INET, ip, &addr_info.sin_addr) <= 0) {                                         //  Convert IPv4 and IPv6 addresses from text to binary form
        snprintf(errorArray, sizeof(errorArray), "%s: Invalid IP Address\n", __FUNCTION__);         //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    tcp_info->addr_len = sizeof(addr_info);
    if (connect(tcp_info->socket_fd, (struct sockaddr*)&addr_info, tcp_info->addr_len) != 0) {      //  Connect to Server
        snprintf(errorArray, sizeof(errorArray), "%s: Connection Failed\n", __FUNCTION__);          //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    memcpy(&tcp_info->addr_info, &addr_info, tcp_info->addr_len);                                   //  Copy address information to tcp_info
    tcflush(tcp_info->socket_fd, TCIOFLUSH);                                                        //  Flush out any previous read/write messages

    return 1;                                                                                       //  Return good
}

/*
    Function: Send TCP client messages
    tcp_info: Struct that hold file descriptor and addr information
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
*/
int32_t TCP_client_send(tcp_info_t *tcp_info, uint8_t *send_msg, uint32_t send_len) {
    ssize_t sentBytes = send(tcp_info->socket_fd, send_msg, send_len, 0);                           //  Send message using client to server socket
    if (sentBytes < 0) {                                                                            //  If sentBytes flag is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Error Sending", __FUNCTION__);                //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    return 1;                                                                                       //  Return good
}

/*
    Function: Receive TCP client messages and have read as blocking
    tcp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
*/
int32_t TCP_client_recv_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(tcp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    int32_t ready = select(tcp_info->socket_fd + 1, &reading, NULL, NULL, NULL);                    //  Wait until socket_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else {
        if (FD_ISSET(tcp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            recvBytes = recv(tcp_info->socket_fd, recv_buff, recv_len, 0);                          //  Read message and populate recv_buff and recvBytes
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Receive TCP client messages and have read as non blocking
    tcp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
    secs: Timeout seconds
    usecs: Timeout useconds
*/
int32_t TCP_client_recv_soft_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(tcp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    struct timeval timeout;                                                                         //  Initialize timeout struct
    timeout.tv_sec = secs;                                                                          //  Set timeout seconds
    timeout.tv_usec = usecs;                                                                        //  Set timeout useconds
    int32_t ready = select(tcp_info->socket_fd + 1, &reading, NULL, NULL, &timeout);                //  Wait until socket_fd has a message or timeout
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else if (ready == 0) {                                                                          //  If socket_fd is not ready
        printf("%s: Timeout Occurred\n", __FUNCTION__);                                             //  Print Timeout
    }
    else {
        if (FD_ISSET(tcp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            recvBytes = recv(tcp_info->socket_fd, recv_buff, recv_len, 0);                          //  Read message and populate recv_buff and recvBytes
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Initialize TCP Server struct and allow all ethernet interfaces
    tcp_info: Struct that hold file descriptor and addr information
    ip: IP address of the server that its connecting to in X.X.X.X (127.0.0.1)
    port: Port that it is using (Range: 0 - 65535)
*/
int32_t TCP_server_any_ip_init(tcp_info_t *tcp_info, uint16_t port) { 
    if ((tcp_info->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {                    //  Initialize server socket
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Creation Failed\n", __FUNCTION__);     //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info struct
    addr_info.sin_family = AF_INET;                                                                 //  Set address family to ipv4 address
    addr_info.sin_addr.s_addr = htonl(INADDR_ANY);                                                  //  Set ip address to any and host to network long
    addr_info.sin_port = htons(port);                                                               //  Set port family to host to network short

    tcp_info->addr_len = sizeof(addr_info);
    if ((bind(tcp_info->socket_fd, (struct sockaddr*) &addr_info, tcp_info->addr_len)) !=0) {       //  Bind socket to TCP incoming address requirements
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Bind Failed", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if ((listen(tcp_info->socket_fd, MAX_CLIENT_CONNECTIONS)) != 0) {                               //  Have server ready to listen up to 1 connections
        snprintf(errorArray, sizeof(errorArray), "%s: Listen Failed", __FUNCTION__);                //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    memcpy(&tcp_info->addr_info, &addr_info, tcp_info->addr_len);                                   //  Copy address information to tcp_info
    tcflush(tcp_info->socket_fd, TCIOFLUSH);                                                        //  Flush out any previous read/write messages

    return 1;                                                                                       //  Return good
}


/*
    Function: Initialize TCP Server struct and allow one specific ethernet interface
    tcp_info: Struct that hold file descriptor and addr information
    ip: IP address of the server that its connecting to in X.X.X.X (127.0.0.1)
    port: Port that it is using (Range: 0 - 65535)
*/
int32_t TCP_server_bind_ip_init(tcp_info_t *tcp_info, const uint8_t *ip, uint16_t port) { 
    if ((tcp_info->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {                    //  Initialize server socket
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Creation Failed\n", __FUNCTION__);     //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info struct
    addr_info.sin_family = AF_INET;                                                                 //  Set address family to ipv4 address
    addr_info.sin_addr.s_addr = inet_addr(ip);                                                      //  Set ip address
    addr_info.sin_port = htons(port);                                                               //  Set port family to host to network short

    tcp_info->addr_len = sizeof(addr_info);
    if ((bind(tcp_info->socket_fd, (struct sockaddr*) &addr_info, tcp_info->addr_len)) !=0) {       //  Bind socket to TCP incoming address requirements
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Bind Failed", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if ((listen(tcp_info->socket_fd, MAX_CLIENT_CONNECTIONS)) != 0) {                               //  Have server ready to listen up to 1 connections
        snprintf(errorArray, sizeof(errorArray), "%s: Listen Failed", __FUNCTION__);                //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    memcpy(&tcp_info->addr_info, &addr_info, tcp_info->addr_len);                                   //  Copy address information to tcp_info
    tcflush(tcp_info->socket_fd, TCIOFLUSH);                                                        //  Flush out any previous read/write messages

    return 1;                                                                                       //  Return good
}

/*
    Function: Send TCP multicast messages
    tcp_info: Struct that hold file descriptor and addr information
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
*/
int32_t TCP_server_send(tcp_info_t *tcp_info, uint8_t *send_msg, uint32_t send_len) {
    ssize_t sentBytes = send(tcp_info->client_fd, send_msg, send_len, 0);                           //  Send message using server to client socket
    if (sentBytes < 0) {                                                                            //  If sentBytes flag is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Error Sending\n", __FUNCTION__);              //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    return 1;                                                                                       //  Return good
}

/*
    Function: Receive TCP server messages and have read as blocking
    tcp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
*/
int32_t TCP_server_recv_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(tcp_info->client_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    int32_t ready = select(tcp_info->client_fd + 1, &reading, NULL, NULL, NULL);                    //  Wait until socket_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else {
        if (FD_ISSET(tcp_info->client_fd, &reading)) {                                              //  Check if the socket_fd is ready
            recvBytes = recv(tcp_info->client_fd, recv_buff, recv_len, 0);                          //  Read message and populate recv_buff and recvBytes
            if (recvBytes <= 0) {                                                                   //  If invalid recvBytes, client disconnected
                close(tcp_info->client_fd);                                                         //  Close client socket fd
                tcp_info->client_known = 0;                                                         //  Set clientKnown to false
            }
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Receive TCP server messages and have read as non blocking
    tcp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
    secs: Timeout seconds
    usecs: Timeout useconds
*/
int32_t TCP_server_recv_soft_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(tcp_info->client_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    struct timeval timeout;                                                                         //  Initialize timeout struct
    timeout.tv_sec = secs;                                                                          //  Set timeout seconds
    timeout.tv_usec = usecs;                                                                        //  Set timeout useconds
    int32_t ready = select(tcp_info->client_fd + 1, &reading, NULL, NULL, &timeout);                //  Wait until socket_fd has a message or timeout
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else if (ready == 0) {                                                                          //  If socket_fd is not ready
        printf("%s: Timeout Occurred\n", __FUNCTION__);                                             //  Print Timeout
    }
    else {
        if (FD_ISSET(tcp_info->client_fd, &reading)) {                                              //  Check if the socket_fd is ready
            recvBytes = recv(tcp_info->client_fd, recv_buff, recv_len, 0);                          //  Read message and populate recv_buff and recvBytes
            if (recvBytes <= 0) {                                                                   //  If invalid recvBytes, client disconnected
                close(tcp_info->client_fd);                                                         //  Close client socket fd
                tcp_info->client_known = 0;                                                         //  Set clientKnown to false
            }
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: TCP server accept clients (blocking) and update client address
    tcp_info: Struct that hold file descriptor and addr information
*/
int32_t TCP_server_accept_blocking(tcp_info_t *tcp_info) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(tcp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t acceptFlag = -1;                                                                         //  Initialize acceptFlag in error state
    int32_t ready = select(tcp_info->socket_fd + 1, &reading, NULL, NULL, NULL);                    //  Wait until socket_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else {
        if (FD_ISSET(tcp_info->socket_fd, &reading)) {
            struct sockaddr_in addr_info = {0};                                                     //  Initialize temp addr_info struct
            tcp_info->client_addr_len = sizeof(addr_info);
            if ((tcp_info->client_fd = accept(tcp_info->socket_fd, (struct sockaddr*) &addr_info, &tcp_info->client_addr_len)) < 0) { //  Accept the cleint connection
                snprintf(errorArray, sizeof(errorArray), "%s: Accept Failed", __FUNCTION__);        //  Populate Error Array
                perror(errorArray);                                                                 //  Print out this if it failed
            }
            else {
                tcp_info->client_known = 1;                                                         //  Set client known to true
                memcpy(&tcp_info->client_addr_info, &addr_info, tcp_info->client_addr_len);         //  Copy addr info to struct
                acceptFlag = 1;                                                                     //  Set acceptFlag to good
            }
        }
    }
    return acceptFlag;                                                                              //  Return accept flag
}

/*
    Function: TCP server accept clients (non-blocking) and update client address
    tcp_info: Struct that hold file descriptor and addr information
*/
int32_t TCP_server_accept_soft_blocking(tcp_info_t *tcp_info, uint32_t secs, uint32_t usecs) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(tcp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t acceptFlag = -1;                                                                         //  Initialize acceptFlag in error state
    int32_t ready = select(tcp_info->socket_fd + 1, &reading, NULL, NULL, NULL);                    //  Wait until socket_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else if (ready == 0) {                                                                          //  If socket_fd is not ready
        printf("%s: Timeout Occurred\n", __FUNCTION__);                                             //  Print Timeout
    }
    else {
        if (FD_ISSET(tcp_info->socket_fd, &reading)) {
            struct sockaddr_in addr_info = {0};                                                     //  Initialize temp addr_info struct
            tcp_info->client_addr_len = sizeof(addr_info);
            if ((tcp_info->client_fd = accept(tcp_info->socket_fd, (struct sockaddr*) &addr_info, &tcp_info->client_addr_len)) < 0) { //  Accept the cleint connection
                snprintf(errorArray, sizeof(errorArray), "%s: Accept Failed", __FUNCTION__);        //  Populate Error Array
                perror(errorArray);                                                                 //  Print out this if it failed
            }
            else {
                tcp_info->client_known = 1;                                                         //  Set client known to true
                memcpy(&tcp_info->client_addr_info, &addr_info, tcp_info->client_addr_len);         //  Copy addr info to struct
                acceptFlag = 1;                                                                     //  Set acceptFlag to good
            }
        }
    }
    return acceptFlag;                                                                              //  Return accept flag
}

/*
    Function: Close file descriptors
    tcp_info: Struct that hold file descriptor and addr information
*/
void TCP_close(tcp_info_t *tcp_info) {
    close(tcp_info->client_fd);                                                                     //  Close client socket
    close(tcp_info->socket_fd);                                                                     //  Close server socket
}

/*
    Function: Check that the IP address is valid
    ip: IP address that will be validated
*/
int32_t TCP_validate_ip(const uint8_t *ip) {
    if (ip == NULL) {
        return 0;
    }
    // Duplicate input so strtok can modify it
    uint8_t *copy = strdup(ip);
    if (!copy) {
        return 0;
    }

    int32_t dots = 0;
    uint8_t *token = strtok(copy, ".");
    while (token) {
        size_t len = strlen(token);
        if (len == 0 || len > 3) {
            free(copy);
            return 0;
        }

        // Check every character is a digit
        for (size_t i = 0; i < len; ++i) {
            if (!isdigit((uint8_t)token[i])) {
                free(copy);
                return 0;
            }
        }

        // Convert to integer
        int32_t num = atoi(token);
        if (num < 0 || num > 255) {
            free(copy);
            return 0;
        }

        token = strtok(NULL, ".");
        if (token) dots++;
    }

    free(copy);
    // There must be exactly 3 dots → 4 parts
    return (dots == 3);
}