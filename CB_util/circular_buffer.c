#include <stdio.h>
#include "debug.h"
#include "circular_buffer.h"

volatile int logCircularBuffer=0;

#ifndef min
#define min(a,b)    ((a) < (b) ? (a) : (b))
#endif


/*****************************************************************************/
/* Create a new circular buffer than can store size bytes */
/*****************************************************************************/
circular_buffer_t *VW_CircularBufferCreate(unsigned int size)
{
    circular_buffer_t *buffer = malloc(sizeof(circular_buffer_t));
    assert(buffer != NULL);
    buffer->size = size + 1;
    buffer->buffer = malloc(buffer->size);
    assert(buffer->buffer != NULL);
    buffer->start = buffer->end = 0;
    return buffer;
}


/*****************************************************************************/
/* Destroy a circular buffer, freeing all associated resources. */
/*****************************************************************************/
void VW_CircularBufferDestroy(circular_buffer_t * buffer)
{
    free(buffer->buffer);
    free(buffer);
}


/*****************************************************************************/
/* Calculate how much data is filled into buffer. */
/*****************************************************************************/
unsigned int VW_CircularBufferFilled(circular_buffer_t * buffer, int *roll_over)
{

    int returnValue=(buffer->end)-(buffer->start);
    if (roll_over) *roll_over=0;
    if (returnValue < 0)
    {
        if (logCircularBuffer)
        {
            LOG_INFO("Buffer rolled over");
        }
        returnValue += buffer->size;
        if (roll_over) *roll_over=1;
    }
    returnValue %= buffer->size;
    if (logCircularBuffer && (returnValue != 0) && (returnValue != 130))
    {
        LOG_INFO("VW_CircularBufferFilled: buffer start: %d, buffer end: %d", buffer->start, buffer->end);
        printf("Returning %d\n", returnValue);

        fflush(stdout);
    }
    return returnValue;
}


/*****************************************************************************/
/* Calculate how much room is left for adding into buffer. */
/*****************************************************************************/
unsigned int VW_CircularBufferLeft(circular_buffer_t * buffer)
{
    int returnValue = ((buffer->start) - (buffer->end)) - 1;
    if (returnValue < 0)
    {
        returnValue += buffer->size;
    }
    returnValue %= buffer->size;
    return (unsigned int) returnValue;
}


/*****************************************************************************/
/* Length is maxlength coming in, returns actual length coming out */
/*****************************************************************************/
unsigned char *VW_CircularReadChunk(circular_buffer_t * buffer,
                                    unsigned int *      length)
{
    unsigned int start = buffer->start;
    unsigned int end = (start + *length) % buffer->size;
    unsigned char * destination;

    *length = min(*length,VW_CircularBufferFilled(buffer,NULL));

    if (end >= start)
    {
        *length = end - start;
        destination = buffer->buffer + start;
        buffer->start = end;
        return destination;
    }
    else
    {
        *length = buffer->size - start;
        destination = buffer->buffer + start;
        buffer->start = 0;
        return destination;
    }
}


/*****************************************************************************/
/* Requests a chunk of length *length to write to, returns a writeable
 * chunk of length *length---Where length might be changed by the call
 * You can continue using this call to fill up to m184_BufferLeft,
 * after which it will return lengths of zero.
 * You then are obligated to fill up the returned buffer before anybody
 * accesses it.
 */
/*****************************************************************************/
unsigned char *VW_CircularWriteChunk(circular_buffer_t * buffer,
                                     unsigned int *      length)
{
    unsigned int start = buffer->end;
    unsigned int end = (start + *length) % buffer->size;
    unsigned char *destination;
    *length = min(*length, VW_CircularBufferLeft(buffer));

    if (end >= start)
    {
        *length = end-start;
        destination = buffer->buffer + start;
        buffer->end = end;
        return destination;
    }
    else
    {
        *length = buffer->size - start;
        destination = buffer->buffer + start;
        buffer->end = 0;
        return destination;
    }
}


/*****************************************************************************/
/* Deletes the current contents of a buffer. */
/*****************************************************************************/
void VW_CircularClear(circular_buffer_t * buffer)
{
    VW_CircularTrim(buffer, VW_CircularBufferFilled(buffer,NULL));
}


/*****************************************************************************/
/* Deletes length from the end of the buffer */
/*****************************************************************************/
void VW_CircularTrim(circular_buffer_t * buffer, unsigned int length)
{
    buffer->end = (buffer->end + buffer->size - length) % buffer->size;
}


/*****************************************************************************/
/* Reverses a read operation. */
/*****************************************************************************/
void VW_CircularUndoRead(circular_buffer_t * buffer,
                         unsigned int        length)
{
    buffer->start = (buffer->start + buffer->size - length) % buffer->size;
}


/*****************************************************************************/
/* Read from buffer data of size "length" and stuff into destination.
 * It is assumed that you do not pass a value for length greater that
 * what would be returned by CircularBufferFilled for the buffer.
 */
/*****************************************************************************/
void VW_CircularReadBuffer(circular_buffer_t * buffer,
                           unsigned char *     destination,
                           unsigned int        length)
{
    
    unsigned char * source = buffer->buffer;
    unsigned int start = buffer->start;
    unsigned int end = (start + length) % buffer->size;
    
    //if (VW_CircularBufferFilled(buffer) < length) return;
    assert(VW_CircularBufferFilled(buffer, NULL) >= length);
    if (end >= start)
    {
        unsigned int size = end - start;
        memcpy(destination, source + start, size);
    }
    else
    {
        unsigned int firstSize = buffer->size - start;
        unsigned int secondSize = end;
        memcpy(destination, source + start, firstSize);
        memcpy(destination + firstSize, source + 0, secondSize);
    }
    buffer->start = end;
}


/*****************************************************************************/
/* Write to buffer data of size "length" from source. It is assumed that
 * you do not pass a value for length greater that what would be returned
 * by CircularBufferLeft for the buffer.
 */
/*****************************************************************************/
void VW_CircularWriteBuffer(circular_buffer_t * buffer,
                            unsigned char *     source,
                            unsigned int        length)
{
    unsigned char * destination = buffer->buffer;
    unsigned int start = buffer->end;
    unsigned int end = (start + length) % buffer->size;

    //if (VW_CircularBufferFilled(buffer,NULL) < length) return;
    assert(VW_CircularBufferLeft(buffer) >= length);
    if (end >= start)
    {
        unsigned int size = end - start;
        memcpy(destination+start, source, size);
    }
    else
    {
        unsigned int firstSize = buffer->size - start;
        unsigned int secondSize = end;
        memcpy(destination + start, source, firstSize);
        memcpy(destination, source + firstSize, secondSize);
    }
    buffer->end = end;

    if (logCircularBuffer)
    {
        LOG_INFO("VW_CircularWriteBuffer: buffer start: %d, buffer end: %d\n", buffer->start, buffer->end);
        fflush(stdout);
    }


}


/*****************************************************************************/
/* Transfers data from source circular buffer to destination circular buffer.
 * Of course it assumed that length <= min(CircularBufferLeft(destination),
 * CircularBufferFilled(source)).
 */
/*****************************************************************************/
void VW_CircularTransferBuffer(circular_buffer_t * destination,
                               circular_buffer_t * source,
                               unsigned int        length)
{
    unsigned char * sourceBuffer = source->buffer;
    unsigned int readStart = source->start;
    unsigned int readEnd = (readStart + length) % source->size;
    
    if (readEnd >= readStart)
    {
        unsigned int size = readEnd - readStart;
        VW_CircularWriteBuffer(destination, sourceBuffer + readStart, size);
    }
    else
    {
        unsigned int firstSize = source->size - readStart;
        unsigned int secondSize = readEnd;
        VW_CircularWriteBuffer(destination, sourceBuffer + readStart, firstSize);
        VW_CircularWriteBuffer(destination, sourceBuffer + 0, secondSize);
    }
    source->start = readEnd;
    /* Debugging is already handled */
}

