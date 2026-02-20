//  Developed Libraries
#include "UDP_common.h"

//  Global Static Variables
static uint8_t errorArray[120] = {0};                                                               //  Error array to help print specific function

/*
    Function: Initialize UDP Client struct and connection.
    udp_info: Struct that hold file descriptor and addr information
    ip: IP address of the server that its connecting to in X.X.X.X (127.0.0.1)
    port: Port that it is using (Range: 0 - 65535)
*/
int32_t UDP_client_init(udp_info_t *udp_info, const uint8_t *ip, uint16_t port) {
    if (UDP_validate_ip(ip) < 0) {                                                                  //  Check for valid IP
        snprintf(errorArray, sizeof(errorArray), "%s: Invalid IP Address\n", __FUNCTION__);         //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if ((udp_info->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {                     //  Initialize client socket
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
    
    udp_info->addr_len = sizeof(addr_info);
    if (connect(udp_info->socket_fd, (struct sockaddr*)&addr_info, udp_info->addr_len) < 0) {       //  Connect to Server
        snprintf(errorArray, sizeof(errorArray), "%s: Connection Failed\n", __FUNCTION__);          //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    memcpy(&udp_info->addr_info, &addr_info, udp_info->addr_len);                                   //  Copy address information to udp_info
    tcflush(udp_info->socket_fd, TCIOFLUSH);                                                        //  Flush out any previous read/write messages

    return 1;                                                                                       //  Return good
}

/*
    Function: Send UDP client messages
    udp_info: Struct that hold file descriptor and addr information
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
*/
int32_t UDP_client_send(udp_info_t *udp_info, uint8_t *send_msg, uint32_t send_len) {
    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info
    memcpy(&addr_info, &udp_info->addr_info, udp_info->addr_len);                                   //  Copy addr_info to temp

    //  Send Message
    ssize_t sentBytes = sendto(udp_info->socket_fd, 
                                send_msg, 
                                send_len, 
                                0, 
                                (const struct sockaddr *) &addr_info, 
                                udp_info->addr_len);
    if (sentBytes < 0) {                                                                            //  If sentBytes flag is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Error Sending\n", __FUNCTION__);              //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    return 1;                                                                                       //  Return good
}

/*
    Function: Receive UDP client messages and have read as blocking
    udp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
*/
int32_t UDP_client_recv_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(udp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    int32_t ready = select(udp_info->socket_fd + 1, &reading, NULL, NULL, NULL);                    //  Wait until socket_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else {
        if (FD_ISSET(udp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            recvBytes = recvfrom(udp_info->socket_fd, recv_buff, recv_len, 0, NULL, NULL);          //  Read message and populate recv_buff and recvBytes
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Receive UDP client messages and have read as non blocking
    udp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
    secs: Timeout seconds
    usecs: Timeout useconds
*/
int32_t UDP_client_recv_soft_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(udp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    struct timeval timeout;                                                                         //  Initialize timeout struct
    timeout.tv_sec = secs;                                                                          //  Set timeout seconds
    timeout.tv_usec = usecs;                                                                        //  Set timeout useconds
    int32_t ready = select(udp_info->socket_fd + 1, &reading, NULL, NULL, &timeout);                //  Wait until socket_fd has a message or timeout
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else if (ready == 0) {                                                                          //  If socket_fd is not ready
        printf("%s: Timeout Occurred\n", __FUNCTION__);                                             //  Print Timeout
    }
    else {
        if (FD_ISSET(udp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            recvBytes = recvfrom(udp_info->socket_fd, recv_buff, recv_len, 0, NULL, NULL);          //  Read message and populate recv_buff and recvBytes
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Initialize UDP Server struct and allow all ethernet interfaces
    udp_info: Struct that hold file descriptor and addr information
    ip: IP address of the server that its connecting to in X.X.X.X (127.0.0.1)
    port: Port that it is using (Range: 0 - 65535)
*/
int32_t UDP_server_any_ip_init(udp_info_t *udp_info, uint16_t port) {
    if ((udp_info->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {                     //  Initialize server socket
        snprintf(errorArray, sizeof(errorArray), "%s: UDP Socket Failed", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info struct
    addr_info.sin_family = AF_INET;                                                                 //  Set address family to ipv4 address
    addr_info.sin_addr.s_addr = htonl(INADDR_ANY);                                                  //  Set ip address to any
    addr_info.sin_port = htons(port);                                                               //  Set port family to host to network short
    
    udp_info->addr_len = sizeof(addr_info);
    if ((bind(udp_info->socket_fd, (struct sockaddr *) &addr_info, udp_info->addr_len)) != 0) {     //  Bind socket to UDP incoming address requirements
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Bind Failed", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    memcpy(&udp_info->addr_info, &addr_info, udp_info->addr_len);                                   //  Copy address information to udp_info
    tcflush(udp_info->socket_fd, TCIOFLUSH);                                                        //  Flush out any previous read/write messages

    return 1;                                                                                       //  Return good
}

/*
    Function: Initialize UDP Server struct and allow one specific ethernet interface
    udp_info: Struct that hold file descriptor and addr information
    ip: IP address of the server that its connecting to in X.X.X.X (127.0.0.1)
    port: Port that it is using (Range: 0 - 65535)
*/
int32_t UDP_server_bind_ip_init(udp_info_t *udp_info, const uint8_t *ip, uint16_t port) {
    if (UDP_validate_ip(ip) < 0) {                                                                  //  Check for valid IP
        snprintf(errorArray, sizeof(errorArray), "%s: Invalid IP Address\n", __FUNCTION__);         //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if ((udp_info->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {                     //  Initialize server socket
        snprintf(errorArray, sizeof(errorArray), "%s: UDP Socket Failed", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info struct
    addr_info.sin_family = AF_INET;                                                                 //  Set address family to ipv4 address
    addr_info.sin_addr.s_addr = inet_addr(ip);                                                      //  Set ip address
    addr_info.sin_port = htons(port);                                                               //  Set port family to host to network short

    udp_info->addr_len = sizeof(addr_info);
    if ((bind(udp_info->socket_fd, (struct sockaddr *) &addr_info, udp_info->addr_len)) != 0) {     //  Bind socket to UDP incoming address requirements
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Bind Failed", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    memcpy(&udp_info->addr_info, &addr_info, udp_info->addr_len);                                   //  Copy address information to udp_info
    tcflush(udp_info->socket_fd, TCIOFLUSH);                                                        //  Flush out any previous read/write messages

    return 1;                                                                                       //  Return good
}

/*
    Function: Send UDP multicast messages
    udp_info: Struct that hold file descriptor and addr information
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
*/
int32_t UDP_server_send(udp_info_t *udp_info, uint8_t *send_msg, uint32_t send_len) {
    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info
    memcpy(&addr_info, &udp_info->addr_info, udp_info->addr_len);                                   //  Copy addr_info to temp

    //  Send Message
    ssize_t sentBytes = sendto(udp_info->socket_fd, 
                                send_msg, 
                                send_len, 
                                0, 
                                (const struct sockaddr *) &addr_info, 
                                udp_info->addr_len);  //  Send message using server socket
    if (sentBytes < 0) {                                                                            //  If sentBytes flag is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Error Sending\n", __FUNCTION__);              //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    return 1;                                                                                       //  Return good
}

/*
    Function: Receive UDP server messages, update client address, and have read as blocking
    udp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
*/
int32_t UDP_server_recv_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(udp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    int32_t ready = select(udp_info->socket_fd + 1, &reading, NULL, NULL, NULL);                    //  Wait until socket_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else {
        if (FD_ISSET(udp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            struct sockaddr_in addr_info = {0};                                                     //  Initialize temp addr_info
            memcpy(&addr_info, &udp_info->addr_info, udp_info->addr_len);                           //  Copy addr_info to temp

            //  Receive message and update udp client address
            recvBytes = recvfrom(udp_info->socket_fd, 
                                recv_buff, 
                                recv_len, 
                                0, 
                                (struct sockaddr *) &addr_info, 
                                &udp_info->addr_len);
            memcpy(&udp_info->addr_info, &addr_info, udp_info->addr_len);                           //  Copy new address to udp_info
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Receive UDP server messages, update client address, and have read as non blocking
    udp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
    secs: Timeout seconds
    usecs: Timeout useconds
*/
int32_t UDP_server_recv_soft_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(udp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    struct timeval timeout;                                                                         //  Initialize timeout struct
    timeout.tv_sec = secs;                                                                          //  Set timeout seconds
    timeout.tv_usec = usecs;                                                                        //  Set timeout useconds
    int32_t ready = select(udp_info->socket_fd + 1, &reading, NULL, NULL, &timeout);                //  Wait until socket_fd has a message or timeout
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else if (ready == 0) {                                                                          //  If socket_fd is not ready
        snprintf(errorArray, sizeof(errorArray), "%s: Timeout Occurred\n", __FUNCTION__);           //  Populate Error Array
    }
    else {
        if (FD_ISSET(udp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            struct sockaddr_in addr_info = {0};                                                     //  Initialize temp addr_info
            memcpy(&addr_info, &udp_info->addr_info, udp_info->addr_len);                           //  Copy addr_info to temp

            //  Receive message and update udp client address
            recvBytes = recvfrom(udp_info->socket_fd, 
                                recv_buff, 
                                recv_len, 
                                0, 
                                (struct sockaddr *) &addr_info, 
                                &udp_info->addr_len);
            memcpy(&udp_info->addr_info, &addr_info, udp_info->addr_len);                           //  Copy new address to udp_info
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Initialize UDP mulitcast struct and connection.
    udp_info: Struct that hold file descriptor and addr information
    ip: IP address of the group that its connecting to in X.X.X.X (127.0.0.1)
    port: Port that it is using (Range: 0 - 65535)
*/
int32_t UDP_multicast_init(udp_info_t *udp_info, const uint8_t *ip, uint16_t port) {
    if (UDP_validate_ip(ip) < 0) {                                                                  //  Check for valid IP
        snprintf(errorArray, sizeof(errorArray), "%s: Invalid IP Address\n", __FUNCTION__);         //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if ((udp_info->socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {                     //  Initialize multicast socket
        snprintf(errorArray, sizeof(errorArray), "%s: Socket Creation Failed\n", __FUNCTION__);     //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    uint8_t optval = 1;
    if (setsockopt(udp_info->socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {   //  Set Reuse Addr True
        snprintf(errorArray, sizeof(errorArray), "%s: Reuse Addr Failed\n", __FUNCTION__);          //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info struct
    addr_info.sin_family = AF_INET;                                                                 //  Set address family to ipv4 address
    addr_info.sin_addr.s_addr = inet_addr(ip);                                                      //  Set group ip address
    addr_info.sin_port = htons(port);                                                               //  Set port family to host to network short
    udp_info->addr_len = sizeof(addr_info);                                                         //  Get length of addr_info
    
    if (bind(udp_info->socket_fd, (struct sockaddr*) &addr_info, udp_info->addr_len) < 0) {
        snprintf(errorArray, sizeof(errorArray), "%s: Bind Error\n", __FUNCTION__);                 //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    struct ip_mreq multicast_info = {0};                                                            //  Initialzie temp multicast_info struct
    multicast_info.imr_multiaddr.s_addr = inet_addr(ip);                                            //  Set group ip address
    multicast_info.imr_interface.s_addr = htonl(INADDR_ANY);                                        //  IDK
    udp_info->multicast_len = sizeof(multicast_info);                                               //  Get length of multicast_info
    if (setsockopt(udp_info->socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &multicast_info, sizeof(multicast_info)) < 0) {  //  Set multicast option
        snprintf(errorArray, sizeof(errorArray), "%s: Multicast Failed\n", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    memcpy(&udp_info->addr_info, &addr_info, udp_info->addr_len);                                   //  Copy temp addr_info to addr_info
    memcpy(&udp_info->multicast_info, &multicast_info, udp_info->multicast_len);                    //  Copy temp multicast_info tp mulicast_info
    tcflush(udp_info->socket_fd, TCIOFLUSH);                                                        //  Flush out any previous read/write messages

    return 1;                                                                                       //  Return good
}

/*
    Function: Send UDP multicast messages
    udp_info: Struct that hold file descriptor and addr information
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
*/
int32_t UDP_multicast_send(udp_info_t *udp_info, uint8_t *send_msg, uint32_t send_len) {
    struct sockaddr_in addr_info = {0};                                                             //  Initialize temp addr_info
    memcpy(&addr_info, &udp_info->addr_info, udp_info->addr_len);                                   //  Copy addr_info to temp

    //  Send Message
    ssize_t sentBytes = sendto(udp_info->socket_fd, 
                                send_msg, 
                                send_len, 
                                0, 
                                (const struct sockaddr *) &addr_info, 
                                udp_info->addr_len);
    if (sentBytes < 0) {                                                                            //  If sentBytes flag is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Error Sending\n", __FUNCTION__);              //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    return 1;                                                                                       //  Return good
}

/*
    Function: Receive UDP multicast messages and have read as blocking
    udp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
*/
int32_t UDP_multicast_recv_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(udp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    int32_t ready = select(udp_info->socket_fd + 1, &reading, NULL, NULL, NULL);                    //  Wait until socket_fd has a message
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else {
        if (FD_ISSET(udp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            struct sockaddr_in addr_info = {0};                                                     //  Initialize temp addr_info
            memcpy(&addr_info, &udp_info->addr_info, udp_info->addr_len);                           //  Copy addr_info to temp

            //  Read message and populate recv_buff and recvBytes
            recvBytes = recvfrom(udp_info->socket_fd, 
                                recv_buff, 
                                recv_len, 
                                0, 
                                (struct sockaddr *) &addr_info, 
                                &udp_info->addr_len);
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Receive UDP multicast messages and have read as non blocking
    udp_info: Struct that hold file descriptor and addr information
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
    secs: Timeout seconds
    usecs: Timeout useconds
*/
int32_t UDP_multicast_recv_soft_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs) {
    fd_set reading;                                                                                 //  Initialize data struct for fd set
    FD_ZERO(&reading);                                                                              //  Set reading struct to 0
    FD_SET(udp_info->socket_fd, &reading);                                                          //  Set reading struct to monitor socket_fd
    int32_t recvBytes = -1;                                                                         //  Initialize recvBytes in error state
    struct timeval timeout;                                                                         //  Initialize timeout struct
    timeout.tv_sec = secs;                                                                          //  Set timeout seconds
    timeout.tv_usec = usecs;                                                                        //  Set timeout useconds
    int32_t ready = select(udp_info->socket_fd + 1, &reading, NULL, NULL, &timeout);                //  Wait until socket_fd has a message or timeout
    if (ready < 0) {                                                                                //  If ready is invalid
        snprintf(errorArray, sizeof(errorArray), "%s: Select() Failed\n", __FUNCTION__);            //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
    }
    else if (ready == 0) {                                                                          //  If socket_fd is not ready
        printf("%s: Timeout Occurred\n", __FUNCTION__);                                             //  Print Timeout
    }
    else {
        if (FD_ISSET(udp_info->socket_fd, &reading)) {                                              //  Check if the socket_fd is ready
            struct sockaddr_in addr_info = {0};                                                     //  Initialize temp addr_info
            memcpy(&addr_info, &udp_info->addr_info, udp_info->addr_len);                           //  Copy addr_info to temp

            //  Read message and populate recv_buff and recvBytes
            recvBytes = recvfrom(udp_info->socket_fd, 
                                recv_buff, 
                                recv_len, 
                                0, 
                                (struct sockaddr *) &addr_info, 
                                &udp_info->addr_len);
        }
        else {
            snprintf(errorArray, sizeof(errorArray), "%s: FD not ready\n", __FUNCTION__);           //  Populate Error Array
            perror(errorArray);                                                                     //  Print out this if it failed
        }
    }
    return recvBytes;                                                                               //  Return total recvBytes or error
}

/*
    Function: Close file descriptor
    udp_info: Struct that hold file descriptor and addr information
*/
void UDP_close(udp_info_t *udp_info) {
    close(udp_info->socket_fd);                                                                     //  Close client connection
}

/*
    Function: Check that the IP address is valid
    ip: IP address that will be validated
*/
int32_t UDP_validate_ip(const uint8_t *ip) {
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