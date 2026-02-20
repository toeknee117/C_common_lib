//  Developed Libraries
#include "log_common.h"

//  Global Static Variables
static uint8_t errorArray[120] = {0};                                                               //  Error array to help print specific function

/*
    Function: Initialize log information and thread to send print statements to a file
    log_info: Struct that hold file descriptor and log file information
    log_path: Path to the log file
    max_size: Maximum size of log file
*/
int32_t log_print_init(log_info_t *log_info, const uint8_t *log_path, uint16_t max_size, uint8_t log_level) {
    FILE *file = fopen(log_path, "w");                                                              //  Open file clear its contents
    fclose(file);                                                                                   //  Close file
    if ((log_info->fd = open(log_path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644)) < 0) {       //  Open file descriptor
        snprintf(errorArray, sizeof(errorArray), "%s: Open FD\n", __FUNCTION__);                    //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    log_info->max_size = max_size;                                                                  //  Set maximum size
    log_info->log_level = log_level;                                                                //  Set log level
    dup2(log_info->fd, STDOUT_FILENO);

    if (pthread_cond_init(&log_info->monitorCond, NULL) != 0) {                                     //  Initialize thread condition
        snprintf(errorArray, sizeof(errorArray), "%s: Thread Condition\n", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (pthread_mutex_init(&log_info->monitorLock, NULL) != 0) {                                    //  Initialize thread lock
        snprintf(errorArray, sizeof(errorArray), "%s: Thread Lock\n", __FUNCTION__);                //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (pthread_create(&log_info->monitorThread, NULL, monitorFileSize, &log_info) < 0) {           //  Create Thread with log_info args
        snprintf(errorArray, sizeof(errorArray), "%s: Thread Create\n", __FUNCTION__);              //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    return 1;                                                                                       //  Return good
}

/*
    Function: Initialize log information and thread to send string to a file
    log_info: Struct that hold file descriptor and log file information
    log_path: Path to the log file
    max_size: Maximum size of log file
*/
int32_t log_str_init(log_info_t *log_info, const uint8_t *log_path, uint16_t max_size, uint8_t log_level) {
    FILE *file = fopen(log_path, "w");                                                              //  Open file clear its contents
    fclose(file);                                                                                   //  Close file
    if ((log_info->fd = open(log_path, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644)) < 0) {       //  Open file descriptor
        snprintf(errorArray, sizeof(errorArray), "%s: Open FD\n", __FUNCTION__);                    //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }
    log_info->max_size = max_size;                                                                  //  Set maximum size
    log_info->log_level = log_level;                                                                //  Set log level

    if (pthread_cond_init(&log_info->monitorCond, NULL) != 0) {                                     //  Initialize thread condition
        snprintf(errorArray, sizeof(errorArray), "%s: Thread Condition\n", __FUNCTION__);           //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (pthread_mutex_init(&log_info->monitorLock, NULL) != 0) {                                    //  Initialize thread lock
        snprintf(errorArray, sizeof(errorArray), "%s: Thread Lock\n", __FUNCTION__);                //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    if (pthread_create(&log_info->monitorThread, NULL, monitorFileSize, &log_info) < 0) {           //  Create Thread with log_info args
        snprintf(errorArray, sizeof(errorArray), "%s: Thread Create\n", __FUNCTION__);              //  Populate Error Array
        perror(errorArray);                                                                         //  Print out this if it failed
        return -1;                                                                                  //  Return error
    }

    return 1;                                                                                       //  Return good
}

/*
    Function: Run thread that will monitor file size
    args: Arguements as a pointer
*/
void *monitorFileSize(void *args) {
    p_log_info_t p_info = (p_log_info_t) args;                                                      //  Create pointer to log info struct
    p_info->monitorThread_Flag = 1;                                                                 //  Set monitor flag high
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);                                            //  Enable thread cancelation
    do {
        pthread_mutex_lock(&p_info->monitorLock);                                                   //  Lock monitor
        pthread_cond_wait(&p_info->monitorCond, &p_info->monitorLock);                              //  Thread waits here until signal is reached
        pthread_mutex_unlock(&p_info->monitorLock);                                                 //  Unlock monitor
        struct stat st;
        if (fstat(p_info->fd, &st) == 0 && st.st_size > p_info->max_size) {
            off_t start = st.st_size - p_info->max_size;
            lseek(p_info->fd, start, SEEK_SET);
            char *buf = malloc(p_info->max_size);
            ssize_t nr = read(p_info->fd, buf, p_info->max_size);
            if (nr > 0) {
                ssize_t off = 0;
                while (off < nr && buf[off] != '\n') off++;
                if (off < nr) off++;
                lseek(p_info->fd, 0, SEEK_SET);
                write(p_info->fd, buf + off, nr - off);
                ftruncate(p_info->fd, nr - off);
            }
            free(buf);
        }
    } while (p_info->monitorThread_Flag);                                                           //  While monitor flag is high
    pthread_exit(NULL);                                                                             //  Close Thread
}

/*
    Function: log using fatal flag
    log_info: Struct that hold file descriptor and log file information
    fmt: orignal string
    ...: arguements to the string
*/
void log_fatal(log_info_t *log_info, const uint8_t *fmt, ...) {
    if (log_info->log_level >= LOG_FATAL) {
        va_list ap;
        va_start(ap, fmt);
        uint8_t ts[20];
    get_timestamp(ts, sizeof(ts));
    uint8_t *msg = fmt_to_buffer(fmt, ap);
    if (!msg) {
        return;
    }
    dprintf(log_info->fd, "[%s] LOG_FATAL: %s\n", ts, msg);
    free(msg);
    pthread_mutex_lock(&log_info->monitorLock);
    pthread_cond_signal(&log_info->monitorCond);
    pthread_mutex_unlock(&log_info->monitorLock);
        va_end(ap);
    }
}

/*
    Function: log using error flag
    log_info: Struct that hold file descriptor and log file information
    fmt: orignal string
    ...: arguements to the string
*/
void log_error(log_info_t *log_info, const uint8_t *fmt, ...) {
    if (log_info->log_level >= LOG_ERROR) {
        va_list ap;
        va_start(ap, fmt);
        uint8_t ts[20];
    get_timestamp(ts, sizeof(ts));
    uint8_t *msg = fmt_to_buffer(fmt, ap);
    if (!msg) {
        return;
    }
    dprintf(log_info->fd, "[%s] LOG_ERROR: %s\n", ts, msg);
    free(msg);
    pthread_mutex_lock(&log_info->monitorLock);
    pthread_cond_signal(&log_info->monitorCond);
    pthread_mutex_unlock(&log_info->monitorLock);
        va_end(ap);
    }
}

/*
    Function: log using warn flag
    log_info: Struct that hold file descriptor and log file information
    fmt: orignal string
    ...: arguements to the string
*/
void log_warn(log_info_t *log_info, const uint8_t *fmt, ...) {
    if (log_info->log_level >= LOG_WARN) {
        va_list ap;
        va_start(ap, fmt);
        uint8_t ts[20];
        get_timestamp(ts, sizeof(ts));
        uint8_t *msg = fmt_to_buffer(fmt, ap);
        if (!msg) {
            return;
        }
        dprintf(log_info->fd, "[%s] LOG_WARN: %s\n", ts, msg);
        free(msg);
        pthread_mutex_lock(&log_info->monitorLock);
        pthread_cond_signal(&log_info->monitorCond);
        pthread_mutex_unlock(&log_info->monitorLock);
        va_end(ap);
    }
}

/*
    Function: log using info flag
    log_info: Struct that hold file descriptor and log file information
    fmt: orignal string
    ...: arguements to the string
*/
void log_info(log_info_t *log_info, const uint8_t *fmt, ...) {
    if (log_info->log_level >= LOG_INFO) {
        va_list ap;
        va_start(ap, fmt);
        uint8_t ts[20];
        get_timestamp(ts, sizeof(ts));
        uint8_t *msg = fmt_to_buffer(fmt, ap);
        if (!msg) {
            return;
        }
        dprintf(log_info->fd, "[%s] LOG_INFO: %s\n", ts, msg);
        free(msg);
        pthread_mutex_lock(&log_info->monitorLock);
        pthread_cond_signal(&log_info->monitorCond);
        pthread_mutex_unlock(&log_info->monitorLock);
        va_end(ap);
    }
}

/*
    Function: log using debug flag
    log_info: Struct that hold file descriptor and log file information
    fmt: orignal string
    ...: arguements to the string
*/
void log_debug(log_info_t *log_info, const uint8_t *fmt, ...) {
    if (log_info->log_level >= LOG_DEBUG) {
        va_list ap;
        va_start(ap, fmt);
        uint8_t ts[20];
        get_timestamp(ts, sizeof(ts));
        uint8_t *msg = fmt_to_buffer(fmt, ap);
        if (!msg) {
            return;
        }
        dprintf(log_info->fd, "[%s] LOG_DEBUG: %s\n", ts, msg);
        free(msg);
        pthread_mutex_lock(&log_info->monitorLock);
        pthread_cond_signal(&log_info->monitorCond);
        pthread_mutex_unlock(&log_info->monitorLock);
        va_end(ap);
    }
}

/*
    Function: log using debug ex0 flag
    log_info: Struct that hold file descriptor and log file information
    fmt: orignal string
    ...: arguements to the string
*/
void log_debug_ex0(log_info_t *log_info, const uint8_t *fmt, ...) {
    if (log_info->log_level >= LOG_DEBUG_EX0) {
        va_list ap;
        va_start(ap, fmt);
        uint8_t ts[20];
        get_timestamp(ts, sizeof(ts));
        uint8_t *msg = fmt_to_buffer(fmt, ap);
        if (!msg) {
            return;
        }
        dprintf(log_info->fd, "[%s] LOG_DEBUG_EX0: %s\n", ts, msg);
        free(msg);
        pthread_mutex_lock(&log_info->monitorLock);
        pthread_cond_signal(&log_info->monitorCond);
        pthread_mutex_unlock(&log_info->monitorLock);
        va_end(ap);
    }
}

/*
    Function: log using debug ex1 flag
    log_info: Struct that hold file descriptor and log file information
    fmt: orignal string
    ...: arguements to the string
*/
void log_debug_ex1(log_info_t *log_info, const uint8_t *fmt, ...) {
    if (log_info->log_level >= LOG_DEBUG_EX1) {
        va_list ap;
        va_start(ap, fmt);
        uint8_t ts[20];
        get_timestamp(ts, sizeof(ts));
        uint8_t *msg = fmt_to_buffer(fmt, ap);
        if (!msg) {
            return;
        }
        dprintf(log_info->fd, "[%s] LOG_DEBUG_EX1: %s\n", ts, msg);
        free(msg);
        pthread_mutex_lock(&log_info->monitorLock);
        pthread_cond_signal(&log_info->monitorCond);
        pthread_mutex_unlock(&log_info->monitorLock);
        va_end(ap);
    }
}

/*
    Function: Close file descriptor and close thread
    log_info: Struct that hold file descriptor and log file information
*/
void log_close(log_info_t *log_info) {
    if (log_info->monitorThread_Flag) {
        log_info->monitorThread_Flag = 0;
        pthread_cancel(log_info->monitorThread);
        pthread_join(log_info->monitorThread, NULL);
    }
    close(log_info->fd);
}

/*
    Function: Get current time for timestamp and insert it to the front of buffer
    buf: origninal buffer string
    sz: size of buffer string
*/
void get_timestamp(uint8_t *buf, size_t sz) {
    time_t now = time(NULL);
    struct tm tm;
    localtime_r(&now, &tm);
    strftime(buf, sz, "%Y-%m-%d %H:%M:%S", &tm);
}

/*
    Function: Populate buffer using original string and arguements
    fmt: orignal string
    ap: arguements to string
*/
uint8_t *fmt_to_buffer(const uint8_t *fmt, va_list ap) {
    va_list ap_copy;
    va_copy(ap_copy, ap);
    int32_t len = vsnprintf(NULL, 0, fmt, ap_copy);
    va_end(ap_copy);
    if (len < 0) {
        return NULL;
    }
    uint8_t *buf = malloc(len + 1);
    if (!buf) {
        return NULL;
    }
    vsnprintf(buf, len + 1, fmt, ap);
    return buf;
}
