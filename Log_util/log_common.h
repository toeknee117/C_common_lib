#pragma once
#pragma pack(push, 1)               //  Struct byte package format. (removes all struct bytes)
#ifndef LOG_COMMON_H
#define LOG_COMMON_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

/*
---------------------------------------------------------------------------------
Defined Variables
---------------------------------------------------------------------------------
*/
#define LOG_FATAL                           (1)
#define LOG_ERROR                           (2)
#define LOG_WARN                            (3)
#define LOG_INFO                            (4)
#define LOG_DEBUG                           (5)
#define LOG_DEBUG_EX0                       (6)
#define LOG_DEBUG_EX1                       (7)

#define LOG_PATH_SIZE                       (120)

/*
---------------------------------------------------------------------------------
Structs
---------------------------------------------------------------------------------
*/ 
//  Log Information Struct
typedef struct _log_info_t {
    int32_t fd;
    uint8_t file_path[LOG_PATH_SIZE];
    uint16_t max_size;
    uint8_t log_level;
    pthread_t monitorThread;
    uint8_t monitorThread_Flag;
    pthread_cond_t monitorCond;
    pthread_mutex_t monitorLock;
} log_info_t, *p_log_info_t;

/*
---------------------------------------------------------------------------------
Functions
---------------------------------------------------------------------------------
*/
int32_t log_print_init(log_info_t *log_info, const uint8_t *log_path, uint16_t max_size, uint8_t log_level);
int32_t log_str_init(log_info_t *log_info, const uint8_t *log_path, uint16_t max_size, uint8_t log_level);
void *monitorFileSize(void *args);
void log_fatal(log_info_t *log_info, const uint8_t *fmt, ...);
void log_error(log_info_t *log_info, const uint8_t *fmt, ...);
void log_warn(log_info_t *log_info, const uint8_t *fmt, ...);
void log_info(log_info_t *log_info, const uint8_t *fmt, ...);
void log_debug(log_info_t *log_info, const uint8_t *fmt, ...);
void log_debug_ex0(log_info_t *log_info, const uint8_t *fmt, ...);
void log_debug_ex1(log_info_t *log_info, const uint8_t *fmt, ...);
void log_close(log_info_t *log_info);
void get_timestamp(uint8_t *buf, size_t sz);
uint8_t *fmt_to_buffer(const uint8_t *fmt, va_list ap);

#endif
