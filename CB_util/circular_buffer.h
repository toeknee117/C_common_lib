#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H 1

#include <string.h>
#include <assert.h>
#include <stdlib.h>

typedef struct
{
    unsigned char * buffer;
    unsigned int    size;
    unsigned int    start;
    unsigned int    end;
} circular_buffer_t;


/**
 * @brief Create a new circular buffer than can store size bytes
 * @param size
 * @return
 */
circular_buffer_t *VW_CircularBufferCreate(unsigned int size);

/**
 * @brief Destroy a circular buffer, freeing all associated resources.
 * @param buffer
 */
void VW_CircularBufferDestroy(circular_buffer_t * buffer);
/**
 * @brief Calculate how much data is filled into buffer.
 * @param buffer
 * @return
 */
unsigned int VW_CircularBufferFilled(circular_buffer_t * buffer, int *roll_over);
/**
 * @brief Calculate how much room is left for adding into buffer.
 * @param buffer
 * @return
 */
unsigned int VW_CircularBufferLeft(circular_buffer_t * buffer);
/**
 * @brief Length is maxlength coming in, returns actual length coming out
 * @param buffer
 * @param length
 * @return
 */
unsigned char *VW_CircularReadChunk(circular_buffer_t * buffer, unsigned int *length);
/**
 * @brief Requests a chunk of length *length to write to, returns a writeable
 * chunk of length *length---Where length might be changed by the call.
 * You can continue using this call to fill up tto m184_BufferLeft,
 * after which it will return lengths of zero.
 * You then are obligated to fill up the returned buffer before anybody else
 * accesses it.
 * @param buffer
 * @param length
 * @return
 */
unsigned char *VW_CircularWriteChunk(circular_buffer_t * buffer,
                                     unsigned int *      length);
/**
 * @brief Deletes length from the end of the buffer
 * @param buffer
 * @param length
 */
void VW_CircularTrim(circular_buffer_t * buffer, unsigned int length);

/**
 * @brief Reverses a read operation.
 * @param buffer
 * @param length
 */

void VW_CircularUndoRead(circular_buffer_t * buffer,
                         unsigned int        length);

/**
 * @brief Read from buffer data of size "length" and stuff into destination.
 * It is assumed that you do not pass a value for length greater that
 * what would be returned by CircularBufferFilled for the buffer.
 * @param buffer
 * @param destination
 * @param length
 */

void VW_CircularReadBuffer(circular_buffer_t * buffer,
                           unsigned char *     destination,
                           unsigned int        length);

/**
 * @brief Write to buffer data of size "length" from source. It is assumed that
 * you do not pass a value for length greater that what would be returned
 * by CircularBufferLeft for the buffer.
 * @param buffer
 * @param source
 * @param length
 */
void VW_CircularWriteBuffer(circular_buffer_t * buffer,
                            unsigned char *     source,
                            unsigned int        length);


/**
 * @brief Transfers data from source circular buffer to destination circular buffer.
 * Of course it assumed that length <= min(CircularBufferLeft(destination),
 * CircularBufferFilled(source)).
 * @param destination
 * @param source
 * @param length
 */
void VW_CircularTransferBuffer(circular_buffer_t * destination,
                               circular_buffer_t * source,
                               unsigned int        length);


/**
 * @brief Deletes the contents of a buffer.
 * @param buffer
 */
void VW_CircularClear(circular_buffer_t * buffer);

#endif /* CIRCULAR_BUFFER_H */

