#pragma once
#ifndef UART_COMMON_H
#define UART_COMMON_H

//  Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <string.h>
#include <stdint.h>
#include <sys/select.h>

//  UART Struct
typedef struct _uart_info_t {
    int32_t uart_fd;
    uint32_t user_baud_rate;
    speed_t uart_baud_rate;
    uint8_t uart_device[120];
} uart_info_t, *p_uart_info_t;

//  Delcare Functions
int32_t UART_init(uart_info_t *uart_info, const uint8_t *uart_device, uint32_t baud_rate);
void UART_close(uart_info_t *uart_info);
int32_t UART_send(uart_info_t *uart_info, uint8_t *send_msg, uint32_t send_len);
int32_t UART_recv_blocking(uart_info_t *uart_info, uint8_t *recv_msg, uint32_t msglen);
int32_t UART_recv_soft_blocking(uart_info_t *uart_info, uint8_t *recv_msg, uint32_t msglen, uint32_t secs, uint32_t usecs);

#endif