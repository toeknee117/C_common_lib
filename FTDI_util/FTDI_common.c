//  Developed libraries
#include "FTDI_common.h"

//  Global Static Variables
static uint8_t errorArray[120] = {0};                                                               //  Error array to help print specific function
static FT_HANDLE FTDI_FD = NULL;                                                                //  File descriptor for the FTDI
static FT_STATUS FTDI_STATUS = FT_OK;                                                           //  Flag for FTDI status

/*
    Function: Initialize FTDI drivers and device
    user_baudRate: Set baud rate of FTDI device
*/
int32_t ftdi_init(uint32_t user_baudRate) {
    system("sudo rmmod ftdi_sio");                                                              //  Remove Linux driver
    system("sudo rmmod usbserial");                                                             //  Remove Linux driver
    DWORD driverVersion = 0;                                                                    //  Initialize driver version variable
    uint8_t portNum = 0;                                                                        // First device found
    FTDI_STATUS = FT_Open(portNum, &FTDI_FD);                                                   //  Open the device
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        snprintf(errorArray, sizeof(errorArray), "%s: FT_Open(%d) Error %d\n", __FUNCTION__, portNum, (int32_t) FTDI_STATUS);   //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        printf("On Linux, use lsmod to check if ftdi_sio (and usbserial) are present.\n");      //  Print this out
        printf("If so, unload them using rmmod\n");                                             //  Print this out
        printf("sudo rmmod ftdi_sio\n");                                                        //  Print this out
        printf("sudo rmmod usbserial\n");                                                       //  Print this out
        return -1;                                                                              //  Return error
    }

	FTDI_STATUS = FT_ResetDevice(FTDI_FD);                                                      //  Reset device
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        snprintf(errorArray, sizeof(errorArray), "%s: FT_ResetDevice %d\n", __FUNCTION__, (int32_t) FTDI_STATUS);   //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }
    
    FTDI_STATUS = FT_SetFlowControl(FTDI_FD, FT_FLOW_RTS_CTS, 0, 0);                            //  Set flow control, this is needed for higher baud rates
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        snprintf(errorArray, sizeof(errorArray), "%s: FT_SetFlowControl %d\n", __FUNCTION__, (int32_t) FTDI_STATUS);    //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }

    FTDI_STATUS = FT_SetDataCharacteristics(FTDI_FD, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE);//  Set characteristics
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        snprintf(errorArray, sizeof(errorArray), "%s: FT_SetDataCharacteristics %d\n", __FUNCTION__, (int32_t) FTDI_STATUS);    //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }
    
    FTDI_STATUS = FT_SetBaudRate(FTDI_FD, user_baudRate);                                       //  Set baud rate
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        snprintf(errorArray, sizeof(errorArray), "%s: FT_SetBaudRate %d\n", __FUNCTION__, (int32_t) FTDI_STATUS);   //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }
    ftdi_purge();                                                                               //  Clear all buffers
    FT_SetTimeouts(FTDI_FD,5000,0);                                                          //  Set read 100ms and write timeout to 0s
}

/*
    Function: Send FTDI messages
    send_buff: Send Message Buffer
    send_len: Send Message Buffer Length
*/
int32_t ftdi_send(uint8_t *send_msg, uint32_t send_len) {
    FTDI_STATUS = FT_SetRts(FTDI_FD);                                                           //  Set ready to send
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        snprintf(errorArray, sizeof(errorArray), "%s: FT_SetRts %d\n", __FUNCTION__, (int32_t) FTDI_STATUS);    //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }

    uint32_t bytesWritten = 0;                                                                  //  Initialize bytes that have been written
    FTDI_STATUS = FT_Write(FTDI_FD, send_msg, send_len, &bytesWritten);                         //  Send message
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        snprintf(errorArray, sizeof(errorArray), "%s: FT_Write %d\n", __FUNCTION__, (int32_t) FTDI_STATUS); //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }
    
    if (bytesWritten != send_len) {                                                             //  If bytes written does not match message length
        snprintf(errorArray, sizeof(errorArray), "%s: FT_Write %d\n", __FUNCTION__, (int32_t) FTDI_STATUS); //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }

}

/*
    Function: Receive FTDI messages
    recv_buff: Receive Message Buffer
*/
int32_t ftdi_receive(uint8_t *recv_msg) {
    int32_t bytesRead = -1;                                                                     //  Initialize bytes that have been read
    DWORD RxBytes;                                                                              //  How many bytes to in recv queue
    DWORD EventDWord;                                                                           //  Current event status
    DWORD TxBytes;                                                                              //  How many bytes to in send queue
    FT_GetStatus(FTDI_FD,&RxBytes,&TxBytes,&EventDWord);                                        //  Get Statuses
    if (RxBytes == 0) {                                                                         //  If no bytes in recv queue
        RxBytes = 10;                                                                           //  Set recv queue bytes to 10 so it can be soft blocking
    }
    FTDI_STATUS = FT_Read(FTDI_FD, recv_msg, RxBytes, &bytesRead);                              //  Read message
    if (FTDI_STATUS != FT_OK) {                                                                 //  If FTDI status is not ok
        printf("%s: FT_Read returned %d.\n", __FUNCTION__, (int32_t) FTDI_STATUS);              //  Print this out
        snprintf(errorArray, sizeof(errorArray), "%s: FT_Read %d\n", __FUNCTION__, (int32_t) FTDI_STATUS); //  Populate Error Array
        perror(errorArray);                                                                     //  Print out this if it failed
        return -1;                                                                              //  Return error
    }
    return bytesRead;                                                                           //  Return total bytes read
}

/*
    Function: Close FTDI drivers and device
*/
void ftdi_close() {
    if (FTDI_FD != NULL) {                                                                      //  If FTDI is not NULL
        FT_Close(FTDI_FD);                                                                      //  Close FTDI port
        FTDI_FD = NULL;                                                                         //  Set FTDI to NULL
    }
}

/*
    Function: Purge all tx and rx buffers
*/
void ftdi_purge() {
    FTDI_STATUS = FT_Purge(FTDI_FD, FT_PURGE_RX | FT_PURGE_TX);                                 //  Purge all tx and rx buffers
}