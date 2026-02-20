#pragma once
#pragma pack(push, 1)               //  Struct byte package format. (removes all struct bytes)
#ifndef SPI_COMMON_H
#define SPI_COMMON_H

//  Standard Libraries
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define MIN_SPI_SPEED_HZ (3800)
#define MAX_SPI_SPEED_HZ (250000000)

//  SPI Information Struct
typedef struct _spi_info_t {
    int32_t fd;
    uint8_t mode;
    uint8_t bits_per_word;
    uint32_t speed_hz;
    uint16_t delay_us;
    uint8_t chip_select_change;
} spi_info_t, *p_spi_info_t;

//  Declare Functions
int32_t SPI_init(spi_info_t *spi_info, const char *device, uint8_t mode, uint8_t bits_per_word, uint32_t speed_hz, uint16_t delay_us, uint8_t chip_select_change);
int32_t SPI_trasnfer(spi_info_t *spi_info, uint8_t *send_msg, uint32_t send_len, uint8_t *recv_buff, uint32_t recv_len);
void SPI_close(spi_info_t *spi_info);

#endif