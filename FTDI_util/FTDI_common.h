#pragma once
#ifndef FTDI_COMMON_H
#define FTDI_COMMON_H

//  Standard Libraries
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <ftd2xx.h>
#include <assert.h>

//  Delcare Functions
int32_t ftdi_init(uint32_t baudRate);
int32_t ftdi_send(uint8_t *send_msg, uint32_t send_len);
int32_t ftdi_receive(uint8_t *recv_buff);
void ftdi_close();
void ftdi_purge();

#endif