#include "SimpleLog.h"

char LogBuf[LOG_OUT_BUF_LEN];
char Buf[FORMATTER_LEN];
va_list factor;

SimpleLog::SimpleLog(void (*LogOutFun)(char *, uint8_t))
{
    _LogOutFun = LogOutFun;
}

void SimpleLog::LogD(char *format, ...)
{
#ifdef ALL_LOG_EN
    memset(LogBuf, 0, LOG_OUT_BUF_LEN);
    memset(Buf, 0, FORMATTER_LEN);
    va_start(factor, format);

    MsgFormatting(format);
    va_end(factor);

    _LogOutFun(LogBuf, (uint8_t)strlen(LogBuf));
#endif
}

void SimpleLog::LogV(char *format, ...)
{
#ifdef ALL_LOG_EN
#ifdef VERBOSE_EN
    memset(LogBuf, 0, LOG_OUT_BUF_LEN);
    memset(Buf, 0, FORMATTER_LEN);
    va_start(factor, format);

#ifdef VERBOSE_LOG_COLOR_EN
    strcat(LogBuf, "\033[1;36m");
#endif

    MsgFormatting(format);
    va_end(factor);

#ifdef VERBOSE_LOG_COLOR_EN
    strcat(LogBuf, "\033[0m");
#endif

    _LogOutFun(LogBuf, (uint8_t)strlen(LogBuf));
#endif
#endif
}

void SimpleLog::LogI(char *format, ...)
{
#ifdef ALL_LOG_EN
#ifdef INFO_EN
    memset(LogBuf, 0, LOG_OUT_BUF_LEN);
    memset(Buf, 0, FORMATTER_LEN);
    va_start(factor, format);

#ifdef INFO_LOG_COLOR_EN
    strcat(LogBuf, "\033[1;32m");
#endif

    MsgFormatting(format);
    va_end(factor);

#ifdef INFO_LOG_COLOR_EN
    strcat(LogBuf, "\033[0m");
#endif

    _LogOutFun(LogBuf, (uint8_t)strlen(LogBuf));
#endif
#endif
}

void SimpleLog::LogW(char *format, ...)
{
#ifdef ALL_LOG_EN
#ifdef WARNING_EN
    memset(LogBuf, 0, LOG_OUT_BUF_LEN);
    memset(Buf, 0, FORMATTER_LEN);
    va_start(factor, format);

#ifdef WARNING_LOG_COLOR_EN
    strcat(LogBuf, "\033[1;33m");
#endif

    MsgFormatting(format);
    va_end(factor);

#ifdef WARNING_LOG_COLOR_EN
    strcat(LogBuf, "\033[0m");
#endif

    _LogOutFun(LogBuf, (uint8_t)strlen(LogBuf));
#endif
#endif
}

void SimpleLog::LogE(char *format, ...)
{
#ifdef ALL_LOG_EN
#ifdef ERROR_EN
    memset(LogBuf, 0, LOG_OUT_BUF_LEN);
    memset(Buf, 0, FORMATTER_LEN);
    va_start(factor, format);

#ifdef ERROR_LOG_COLOR_EN
    strcat(LogBuf, "\033[1;31m");
#endif

    MsgFormatting(format);
    va_end(factor);

#ifdef ERROR_LOG_COLOR_EN
    strcat(LogBuf, "\033[0m");
#endif

    _LogOutFun(LogBuf, (uint8_t)strlen(LogBuf));
#endif
#endif
}

void SimpleLog::MsgFormatting(char *format)
{
    int d = 0;
    double f = 0;
    char *s = 0;

    for (char *cntr = format; *cntr; cntr++)
    {
        if (*cntr != '%')
        {
            sprintf(Buf, "%c", *cntr);
        }
        else
        {
            switch (*++cntr)
            {
            case 'd':
                d = va_arg(factor, int);
                sprintf(Buf, "%d", d);
                break;

            case 'f':
                f = va_arg(factor, double);
                sprintf(Buf, "%.2lf", f);
                break;

            case 'x':
                d = va_arg(factor, int);
                sprintf(Buf, "%x", d);
                break;

            case 's':
                s = va_arg(factor, char *);
                sprintf(Buf, "%s", s);
                break;

            case 'c':
                s = va_arg(factor, char *);
                sprintf(Buf, "%s", s);
                break;

            default:
                sprintf(Buf, "%c", *cntr);
            }
        }
        strcat(LogBuf, Buf);
    }
}