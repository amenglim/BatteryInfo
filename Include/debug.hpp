/*******************************************************************************
*                         COPYRIGHT NOTICE
*                   "Copyright 2021 Nova Biomedical Corporation"
*             This program is the property of Nova Biomedical Corporation
*                 200 Prospect Street, Waltham, MA 02454-9141
*             Any unauthorized use or duplication is prohibited
********************************************************************************
*
*  Title            -
*  Source Filename  -
*  Author           -
*  Description      -
*  Location         -
*
*******************************************************************************/


#pragma once

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define UNUSED(x) (void)(x)
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define MAX_PKT_LEN (8192)
#define MAX_STR_LEN (512)
#define MAX_BUF_LEN (256)

#ifdef __cplusplus
constexpr char *DBG_STR[] =
#else
const char *DBG_STR[] =
#endif
{
    (char *)"\t",
    (char *)"INFO 4",
    (char *)"INFO 3",
    (char *)"INFO 2",
    (char *)"INFO 1",
    (char *)"WARN",
    (char *)"ERROR",
    (char *)"\t"
};

enum
{
    DBG_ALL,
    DBG_INFO4,
    DBG_INFO3,
    DBG_INFO2,
    DBG_INFO1,
    DBG_WARN,
    DBG_ERR,
    DBG_NONE
};


/* sets local and global debug level to print. defaults to DBG_WARN and DBG_ALL respectively */
/* can be overridden on a file-by-file based */
#define DBGLVL  DBG_ALL
#define GLBL_DBGLVL DBG_ALL

/* print debug messages to stdout */
#define DBGPRT(lvl, fmt,  ...)                                                                                                                  \
    do {                                                                                                                                        \
        if ((lvl >= GLBL_DBGLVL) && (lvl >= DBGLVL))                                                                                            \
        {                                                                                                                                       \
            struct timespec my_t;                                                                                                               \
            clock_gettime(CLOCK_MONOTONIC, &my_t);                                                                                              \
            printf("[%8lu.%09lu] %-8s:%-20s  %5d: " fmt , my_t.tv_sec, my_t.tv_nsec, DBG_STR[lvl], __FILENAME__, __LINE__, ##__VA_ARGS__);      \
        }                                                                                                                                       \
    } while (0)


/* default number of columns DBGPRT_ARR will display */
/* can be overridden on a file-by-file based */
#define DBG_ARR_WIDTH   8
/* print debug array in canonical form to stdout */
#define DBGPRT_ARR(lvl, arr_, len_)                                                                                                             \
    do {                                                                                                                                        \
        if ((lvl >= GLBL_DBGLVL) && (lvl >= DBGLVL))                                                                                            \
        {                                                                                                                                       \
            int line_len_ = DBG_ARR_WIDTH;                                                                                                      \
            int a_idx_ = 0;                                                                                                                     \
            char ascii_arr_[line_len_] = {0};                                                                                                   \
            struct timespec my_t;                                                                                                               \
            clock_gettime(CLOCK_MONOTONIC, &my_t);                                                                                              \
            while (a_idx_ < (len_))                                                                                                             \
            {                                                                                                                                   \
                printf("[%8lu.%09lu] %-8s:%-20s  %5d: ", my_t.tv_sec, my_t.tv_nsec, DBG_STR[lvl], __FILENAME__, __LINE__);                      \
                for (int l_idx_ = 0; l_idx_ < line_len_; l_idx_++)                                                                              \
                {                                                                                                                               \
                    if (a_idx_ < (len_))                                                                                                        \
                    {                                                                                                                           \
                        ascii_arr_[l_idx_] = arr_[a_idx_];                                                                                      \
                        printf("%02x ", arr_[a_idx_++]);                                                                                        \
                    }                                                                                                                           \
                    else                                                                                                                        \
                    {                                                                                                                           \
                        printf("   ");                                                                                                          \
                    }                                                                                                                           \
                    if (l_idx_ == ((line_len_ / 2) -1))                                                                                         \
                    {                                                                                                                           \
                        printf(" ");                                                                                                            \
                    }                                                                                                                           \
                }                                                                                                                               \
                printf("\t");                                                                                                                   \
                for (int l_idx_ = 0; l_idx_ < line_len_; l_idx_++)                                                                              \
                {                                                                                                                               \
                    printf("%c ", ((ascii_arr_[l_idx_] < 0x20) || (ascii_arr_[l_idx_] > 0x7f)) ? '.' : ascii_arr_[l_idx_]);                     \
                    if (l_idx_ == ((line_len_ / 2) -1))                                                                                         \
                    {                                                                                                                           \
                        printf(" ");                                                                                                            \
                    }                                                                                                                           \
                }                                                                                                                               \
                printf("\n");                                                                                                                   \
                memset(ascii_arr_, 0, sizeof(ascii_arr_));                                                                                      \
            }                                                                                                                                   \
        }                                                                                                                                       \
    } while (0)                                                                                                                                 \












