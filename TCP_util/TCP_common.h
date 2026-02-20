#pragma once
#pragma pack(push, 1)               //  Struct byte package format. (removes all struct bytes)
#ifndef TCP_COMMON_H
#define TCP_COMMON_H

//  Standard Libraries
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <termios.h>
#include <ctype.h>

//  TCP Misc.
#define MAX_CLIENT_CONNECTIONS              (1)

//  TCP Information Struct
typedef struct _tcp_info_t {
    int32_t socket_fd;
    socklen_t addr_len;
    socklen_t client_addr_len;
    int32_t client_fd;
    uint8_t client_known;
    struct sockaddr_in addr_info;
    struct sockaddr_in client_addr_info;
} tcp_info_t, *p_tcp_info_t;


//  Declare Functions
int32_t TCP_client_init(tcp_info_t *tcp_info, const uint8_t *ip, uint16_t port);
int32_t TCP_client_send(tcp_info_t *tcp_info, uint8_t *send_msg, uint32_t send_len);
int32_t TCP_client_recv_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len);
int32_t TCP_client_recv_soft_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs);
int32_t TCP_server_any_ip_init(tcp_info_t *tcp_info, uint16_t port);
int32_t TCP_server_bind_ip_init(tcp_info_t *tcp_info, const uint8_t *ip, uint16_t port);
int32_t TCP_server_send(tcp_info_t *tcp_info, uint8_t *send_msg, uint32_t send_len);
int32_t TCP_server_recv_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len);
int32_t TCP_server_recv_soft_blocking(tcp_info_t *tcp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs);
int32_t TCP_server_accept_blocking(tcp_info_t *tcp_info);
int32_t TCP_server_accept_soft_blocking(tcp_info_t *tcp_info, uint32_t secs, uint32_t usecs);
void TCP_close(tcp_info_t *tcp_info);
int32_t TCP_validate_ip(const uint8_t *ip);

#endif