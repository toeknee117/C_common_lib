#pragma once
#pragma pack(push, 1)               //  Struct byte package format. (removes all struct bytes)
#ifndef UDP_COMMON_H
#define UDP_COMMON_H

//  Standard Libraries
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <ctype.h>
#include <termios.h>

//  UDP Information Struct
typedef struct _udp_info_t {
    int32_t socket_fd;
    struct sockaddr_in addr_info;
    socklen_t addr_len;
    struct ip_mreq multicast_info;
    socklen_t multicast_len;
} udp_info_t, *p_udp_info_t;

//  Declare Functions
int32_t UDP_client_init(udp_info_t *udp_info, const uint8_t *ip, uint16_t port);
int32_t UDP_client_send(udp_info_t *udp_info, uint8_t *send_msg, uint32_t send_len);
int32_t UDP_client_recv_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len);
int32_t UDP_client_recv_soft_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs);
int32_t UDP_server_any_ip_init(udp_info_t *udp_info, uint16_t port);
int32_t UDP_server_bind_ip_init(udp_info_t *udp_info, const uint8_t *ip, uint16_t port);
int32_t UDP_server_send(udp_info_t *udp_info, uint8_t *send_msg, uint32_t send_len);
int32_t UDP_server_recv_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len);
int32_t UDP_server_recv_soft_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs);
int32_t UDP_multicast_init(udp_info_t *udp_info, const uint8_t *ip, uint16_t port);
int32_t UDP_multicast_send(udp_info_t *udp_info, uint8_t *send_msg, uint32_t send_len);
int32_t UDP_multicast_recv_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len);
int32_t UDP_multicast_recv_soft_blocking(udp_info_t *udp_info, uint8_t *recv_buff, uint32_t recv_len, uint32_t secs, uint32_t usecs);
void UDP_close(udp_info_t *udp_info);
int32_t UDP_validate_ip(const uint8_t *ip);

#endif