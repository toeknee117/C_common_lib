/**********************************************************************
 *
 * Filename:    crc.c
 * 
 * Description: Slow and fast implementations of the CRC standards.
 *
 * Notes:       The parameters for each supported CRC standard are
 *              defined in the header file uint32_t.h.  The implementations
 *              here should stand up to further additions to that list.
 *
 * 
 * Copyright (c) 2000 by Michael Barr.  This software is placed into
 * the public domain and may be used for any purpose.  However, this
 * notice must not be changed or removed and no warranty is either
 * expressed or implied by its publication or distribution.
 **********************************************************************/

#include "crc.h"


/*
 * Derive parameters from the standard-specific parameters in uint32_t.h.
 */

static int32_t initialized_32 = 0;


/*********************************************************************
 *
 * Function:    crcSlow_32()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
uint32_t  crcSlow_32(uint8_t const message[], int32_t nBytes)
{
    uint32_t          remainder = INITIAL_REMAINDER_32;
    int32_t            byte;
    uint8_t  bit;

	if(initialized_32 == 0)
	{
		crcInit_32();
		initialized_32=1;
	}
    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH_32 - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT_32)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL_32;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder ^ FINAL_XOR_VALUE_32);

}   /* crcSlow_32() */


static uint32_t crcTable_32[256];


/*********************************************************************
 *
 * Function:    crcInit_32()
 * 
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:       This function must be rerun any time the CRC standard
 *              is changed.  If desired, it can be run "offline" and
 *              the table results stored in an embedded system's ROM.
 *
 * Returns:     None defined.
 *
 *********************************************************************/
void crcInit_32(void)
{
    uint32_t          remainder;
    int32_t            dividend;
    uint8_t  bit;


    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH_32 - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT_32)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL_32;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable_32[dividend] = remainder;
    }

}   /* crcInit_32() */


/*********************************************************************
 *
 * Function:    crcFast_32()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:       crcInit_32() must be called first.
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
uint32_t  crcFast_32(uint8_t const message[], int32_t nBytes)
{
    uint32_t          remainder = INITIAL_REMAINDER_32;
    uint8_t  data;
    int32_t            byte;

	if(initialized_32 == 0)
	{
		crcInit_32();
		initialized_32=1;
	}

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH_32 - 8));
        remainder = crcTable_32[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder ^ FINAL_XOR_VALUE_32);

}   /* crcFast_32() */

/*
 * Derive parameters from the standard-specific parameters in uint16_t.h.
 */

static int32_t initialized_16 = 0;


/*********************************************************************
 *
 * Function:    crcSlow_16()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
uint16_t  crcSlow_16(uint8_t const message[], int32_t nBytes)
{
    uint16_t          remainder = INITIAL_REMAINDER_16;
    int32_t            byte;
    uint8_t  bit;

	if(initialized_16 == 0)
	{
		crcInit_16();
		initialized_16=1;
	}
    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH_16 - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT_16)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL_16;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder ^ FINAL_XOR_VALUE_16);

}   /* crcSlow_16() */


static uint16_t crcTable_16[256];


/*********************************************************************
 *
 * Function:    crcInit_16()
 * 
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:       This function must be rerun any time the CRC standard
 *              is changed.  If desired, it can be run "offline" and
 *              the table results stored in an embedded system's ROM.
 *
 * Returns:     None defined.
 *
 *********************************************************************/
void crcInit_16(void)
{
    uint16_t          remainder;
    int32_t            dividend;
    uint8_t  bit;


    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH_16 - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT_16)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL_16;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable_16[dividend] = remainder;
    }

}   /* crcInit_16() */


/*********************************************************************
 *
 * Function:    crcFast_16()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:       crcInit_16() must be called first.
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
uint16_t  crcFast_16(uint8_t const message[], int32_t nBytes)
{
    uint16_t          remainder = INITIAL_REMAINDER_16;
    uint8_t  data;
    int32_t            byte;

	if(initialized_16 == 0)
	{
		crcInit_16();
		initialized_16=1;
	}

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH_16 - 8));
        remainder = crcTable_16[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder ^ FINAL_XOR_VALUE_16);

}   /* crcFast_16() */

static int32_t initialized_ccitt = 0;


/*********************************************************************
 *
 * Function:    crcSlow_CCITT()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
uint16_t  crcSlow_CCITT(uint8_t const message[], int32_t nBytes)
{
    uint16_t          remainder = INITIAL_REMAINDER_CCITT;
    int32_t            byte;
    uint8_t  bit;

	if(initialized_ccitt == 0)
	{
		crcInit_CCITT();
		initialized_ccitt=1;
	}
    /*
     * Perform modulo-2 division, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        /*
         * Bring the next byte into the remainder.
         */
        remainder ^= (message[byte] << (WIDTH_CCITT - 8));

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT_CCITT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL_CCITT;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }

    /*
     * The final remainder is the CRC result.
     */
    return (remainder ^ FINAL_XOR_VALUE_CCITT);

}   /* crcSlow_CCITT() */


static uint16_t crcTable_ccitt[256];


/*********************************************************************
 *
 * Function:    crcInit_CCITT()
 * 
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:       This function must be rerun any time the CRC standard
 *              is changed.  If desired, it can be run "offline" and
 *              the table results stored in an embedded system's ROM.
 *
 * Returns:     None defined.
 *
 *********************************************************************/
void crcInit_CCITT(void)
{
    uint16_t          remainder;
    int32_t            dividend;
    uint8_t  bit;


    /*
     * Compute the remainder of each possible dividend.
     */
    for (dividend = 0; dividend < 256; ++dividend)
    {
        /*
         * Start with the dividend followed by zeros.
         */
        remainder = dividend << (WIDTH_CCITT - 8);

        /*
         * Perform modulo-2 division, a bit at a time.
         */
        for (bit = 8; bit > 0; --bit)
        {
            /*
             * Try to divide the current data bit.
             */
            if (remainder & TOPBIT_CCITT)
            {
                remainder = (remainder << 1) ^ POLYNOMIAL_CCITT;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }

        /*
         * Store the result into the table.
         */
        crcTable_ccitt[dividend] = remainder;
    }

}   /* crcInit_CCITT() */


/*********************************************************************
 *
 * Function:    crcFast_CCITT()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:       crcInit_CCITT() must be called first.
 *
 * Returns:     The CRC of the message.
 *
 *********************************************************************/
uint16_t  crcFast_CCITT(uint8_t const message[], int32_t nBytes)
{
    uint16_t          remainder = INITIAL_REMAINDER_CCITT;
    uint8_t  data;
    int32_t            byte;

	if(initialized_ccitt == 0)
	{
		crcInit_CCITT();
		initialized_ccitt=1;
	}

    /*
     * Divide the message by the polynomial, a byte at a time.
     */
    for (byte = 0; byte < nBytes; ++byte)
    {
        data = message[byte] ^ (remainder >> (WIDTH_CCITT - 8));
        remainder = crcTable_ccitt[data] ^ (remainder << 8);
    }

    /*
     * The final remainder is the CRC.
     */
    return (remainder ^ FINAL_XOR_VALUE_CCITT);

}   /* crcFast_CCITT() */
