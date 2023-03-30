#pragma once

#include <stdio.h>

extern void _debug_log(int level, const char *func, int line, const char *format, ...);

#define logger_error(format, ...)             \
    _debug_log(1, __func__, __LINE__, format, \
               ##__VA_ARGS__)

#define logger_info(format, ...)              \
    _debug_log(0, __func__, __LINE__, format, \
               ##__VA_ARGS__)
