/**********************************************************************
 *
 * Filename:    crc.c
 * 
 * Description: Slow and fast implementations of the CRC standards.
 *
 * Notes:       The parameters for each supported CRC standard are
 *              defined in the header file crc.h.  The implementations
 *              here should stand up to further additions to that list.
 *
 * 
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/
#pragma once
#ifndef CRC_H
#define CRC_H

#include <stdint.h>

#define CRC_NAME_32          "CRC-32"
#define POLYNOMIAL_32         0x04C11DB7
#define INITIAL_REMAINDER_32 0xFFFFFFFF
#define FINAL_XOR_VALUE_32   0xFFFFFFFF
#define REFLECT_DATA_32      TRUE
#define REFLECT_REMAINDER_32 TRUE
#define CHECK_VALUE_32       0xCBF43926
#define WIDTH_32    (8 * sizeof(uint32_t))
#define TOPBIT_32   (1 << (WIDTH_32 - 1))

#define CRC_NAME_16          "CRC-16"
#define POLYNOMIAL_16        0x8005
#define INITIAL_REMAINDER_16 0x0000
#define FINAL_XOR_VALUE_16   0x0000
#define REFLECT_DATA_16      TRUE
#define REFLECT_REMAINDER_16 TRUE
#define CHECK_VALUE_16       0xBB3D
#define WIDTH_16    (8 * sizeof(uint16_t))
#define TOPBIT_16   (1 << (WIDTH_16 - 1))

#define CRC_NAME_CCITT          "CRC-CCITT"
#define POLYNOMIAL_CCITT        0x1021
#define INITIAL_REMAINDER_CCITT 0xFFFF
#define FINAL_XOR_VALUE_CCITT   0x0000
#define REFLECT_DATA_CCITT      FALSE
#define REFLECT_REMAINDER_CCITT FALSE
#define CHECK_VALUE_CCITT       0x29B1
#define WIDTH_CCITT    (8 * sizeof(uint16_t))
#define TOPBIT_CCITT   (1 << (WIDTH_CCITT - 1))

void   crcInit_32(void);
uint32_t  crcSlow_32(uint8_t const message[], int32_t nBytes);
uint32_t  crcFast_32(uint8_t const message[], int32_t nBytes);

void   crcInit_16(void);
uint16_t  crcSlow_16(uint8_t const message[], int32_t nBytes);
uint16_t  crcFast_16(uint8_t const message[], int32_t nBytes);

void   crcInit_CCITT(void);
uint16_t  crcSlow_CCITT(uint8_t const message[], int32_t nBytes);
uint16_t  crcFast_CCITT(uint8_t const message[], int32_t nBytes);

#endif