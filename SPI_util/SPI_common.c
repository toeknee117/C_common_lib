//  Developed Libraries
#include "SPI_common.h"

//  Global Static Variables
static uint8_t errorArray[120] = {0};                                                               //  Error array to help print specific function

/*
    Function: Initialize SPI struct
    spi_info: Struct that hold file descriptor and general information
    device: spi device that will be used
    mode: spi mode that will be used
    bits_per_word: Number of bits in each word (byte)
    speed_hz: Transfer speed in hz
    delay_us: delay in useconds
    chip_select_change: decides if chip select can change during transfer
*/
int32_t SPI_init(spi_info_t *spi_info, const char *device, uint8_t mode, uint8_t bits_per_word, uint32_t speed_hz, uint16_t delay_us, uint8_t chip_select_change) {
    if (spi_info->fd = open(device, O_RDWR)) {                                                      //  Open SPI device
        snprintf(errorArray, sizeof(errorArray), "%s: Open SPI Device Failed\n", __FUNCTION__);     //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (mode == SPI_MODE_0 || mode == SPI_MODE_1 || mode == SPI_MODE_2 || mode == SPI_MODE_3) {     //  Check SPI Mode selected
        spi_info->mode = mode;                                                                      //  Set SPI Mode
    }
    else {
        snprintf(errorArray, sizeof(errorArray), "%s: Invalid SPI Mode\n", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    spi_info->bits_per_word = bits_per_word;                                                        //  Set bits per word

    if (speed_hz >= MIN_SPI_SPEED_HZ && speed_hz <= MAX_SPI_SPEED_HZ) {                             //  Check SPI Speed
        spi_info->speed_hz = speed_hz;                                                              //  Set SPI Speed
    }
    else {
        snprintf(errorArray, sizeof(errorArray), "%s: Invalid SPI Speed\n", __FUNCTION__);          //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    spi_info->delay_us = delay_us;                                                                  //  Set SPI delay
    spi_info->chip_select_change;                                                                   //  Set chip select

    if (ioctl(spi_info->fd, SPI_IOC_WR_MODE, &spi_info->mode) < 0) {                                //  If set SPI write mode failed
        snprintf(errorArray, sizeof(errorArray), "%s: Write Mode\n", __FUNCTION__);                 //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (ioctl(spi_info->fd, SPI_IOC_RD_MODE, &spi_info->mode) < 0) {                                //  If set SPI read mode failed
        snprintf(errorArray, sizeof(errorArray), "%s: Read Mode\n", __FUNCTION__);                  //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (ioctl(spi_info->fd, SPI_IOC_WR_BITS_PER_WORD, &spi_info->mode) < 0) {                       //  If set SPI write bitsPerWord failed
        snprintf(errorArray, sizeof(errorArray), "%s: Write Bits Per Word\n", __FUNCTION__);        //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (ioctl(spi_info->fd, SPI_IOC_RD_BITS_PER_WORD, &spi_info->mode) < 0) {                       //  If set SPI read bitsPerWord failed
        snprintf(errorArray, sizeof(errorArray), "%s: Read Bits Per Word\n", __FUNCTION__);         //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (ioctl(spi_info->fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_info->mode) < 0) {                        //  If set SPI write write speed failed
        snprintf(errorArray, sizeof(errorArray), "%s: Write Speed\n", __FUNCTION__);                //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (ioctl(spi_info->fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_info->mode) < 0) {                        //  If set SPI read write speed failed
        snprintf(errorArray, sizeof(errorArray), "%s: Read Speed\n", __FUNCTION__);                 //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    
    return 1;                                                                                       //  Return good
}

/*
    Function: Send and Receive Data
    spi_info: Struct that hold file descriptor and general information
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
    recv_buff: Receive Message Buffer
    recv_len: Receive Message Buffer Length
*/
int32_t SPI_trasnfer(spi_info_t *spi_info, uint8_t *send_msg, uint32_t send_len, uint8_t *recv_buff, uint32_t recv_len) {
    uint8_t txdata[128] = {0};                                                                      //  Initialize transmit data array
    uint8_t rxdata[128] = {0};                                                                      //  Initialize receive data array
    memcpy(txdata, send_msg, send_len);                                                             //  Populate tx array with send message

    //  Initailize SPI message transfer struct
    struct spi_ioc_transfer messageTransfer = {
        .tx_buf = (unsigned long) txdata,
        .rx_buf = (unsigned long) rxdata,
        .len = sizeof(rxdata),
        .speed_hz = spi_info->speed_hz,
        .bits_per_word = spi_info->bits_per_word,
        .delay_usecs = spi_info->delay_us,
        .cs_change = spi_info->chip_select_change,
    };

    if (ioctl(spi_info->fd, SPI_IOC_MESSAGE(1), &messageTransfer)) {                                //  Start SPI Transfer
        snprintf(errorArray, sizeof(errorArray), "%s: SPI Transfer Fail\n", __FUNCTION__);          //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    memcpy(recv_buff, rxdata, recv_len);                                                            //  Copy rx data into recv buff
    return 1;                                                                                       //  Return good
}

/*
    Function: Close SPI Device
    spi_info: Struct that hold file descriptor and general information
*/
void SPI_close(spi_info_t *spi_info) {
    close(spi_info->fd);                                                                            //  Close SPI device
}