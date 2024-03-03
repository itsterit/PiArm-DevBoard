#ifndef _SIMPLELOG_
#define _SIMPLELOG_

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#define FORMATTER_LEN 31
#define LOG_OUT_BUF_LEN 255

/**
 * @brief выборочное отключение логов
 */
#define ALL_LOG_EN
#define VERBOSE_EN
#define INFO_EN
#define WARNING_EN
#define ERROR_EN
/**
 * @brief отключить подсветку
 */
#define VERBOSE_LOG_COLOR_EN
#define INFO_LOG_COLOR_EN
#define WARNING_LOG_COLOR_EN
#define ERROR_LOG_COLOR_EN

class SimpleLog
{
private:
    void (*_LogOutFun)(char *, uint8_t);
    void MsgFormatting(char *format);

public:
    SimpleLog(void (*)(char *, uint8_t));
    void LogD(char *format, ...);
    void LogV(char *format, ...);
    void LogI(char *format, ...);
    void LogW(char *format, ...);
    void LogE(char *format, ...);
};

#endif // !_SIMPLELOG_
