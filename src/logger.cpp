#include <thread>
#include <cstring>
#include <sys/time.h>
#include <cstdarg>
#include <mutex>
#include "logger.h"

namespace SS
{
    Logger::Logger()
    {
        m_count = 1;
        m_isAsync = false;
    }

    Logger::~Logger()
    {
        if(m_fp != NULL)
            fclose(m_fp);
        if(m_buf)
            delete[](m_buf);
    }


    bool Logger::init(const char* filename, LOGLEVEL level, int filesize, int maxQueueSize)
    {
        if(maxQueueSize >= 1)
        {
            m_isAsync = true;
            m_logQueue = new SQueue<std::string>(maxQueueSize);
        }

        m_logBufSize = 50000;
        m_buf = new char[m_logBufSize];
        memset(m_buf, 0, sizeof(char)*m_logBufSize);
        m_filesize = filesize;
        m_level = level;

        time_t t = time(NULL);
        struct tm myTm = *localtime(&t);

        const char *p = strrchr(filename, '/');
        char logFullname[256] = {0};
        if(p == NULL)
        {
            snprintf(logFullname, 255, "%d_%02d_%02d_%s",myTm.tm_year+1900, myTm.tm_mon+1, myTm.tm_mday, filename);
        }
        else
        {
            logname = p+1;
            dirpath = std::string(filename, p-filename+1);
            snprintf(logFullname, 255, "%s%d_%02d_%02d_%s",dirpath.data(), myTm.tm_year+1900, myTm.tm_mon+1, myTm.tm_mday, logname.data());
        }

        m_today = myTm.tm_mday;
        m_fp = fopen(logFullname, "a");
        if(m_fp == NULL)
            return false;
        m_nowfilesize = ftell(m_fp);
        return true;
    }

    void Logger::writeLog(LOGLEVEL level, const char *format, ...)
    {
        if(level > m_level) return;
        if(m_isAsync && !m_logQueue) return;
        struct timeval now;
        memset(&now, 0, sizeof(now));
        gettimeofday(&now, NULL);
        time_t t = now.tv_sec;
        struct tm myTm = *localtime(&t);
        char s[16];
        switch(level)
        {
            case DEBUG: strcpy(s, "[debug]:"); break;
            case INFO: strcpy(s, "[info]:"); break;
            case WARN: strcpy(s, "[warn]:"); break;
            case ERROR: strcpy(s, "[error]:"); break;
            default:
                strcpy(s, "[info]:"); break;
        }

        {
            std::lock_guard<std::mutex> lock{m_mutex};
            if(m_today != myTm.tm_mday || m_nowfilesize >= m_filesize)
            {
                fflush(m_fp);
                fclose(m_fp);
                char newLog[256] = {0};
                char tail[16];
                snprintf(tail, 16, "%4d_%02d_%02d_", myTm.tm_year+1900, myTm.tm_mon+1, myTm.tm_mday);
                if(m_today != myTm.tm_mday)
                {
                    snprintf(newLog, 255, "%s%s%s", dirpath.data(), tail, logname.data());
                    m_today = myTm.tm_mday;
                    m_count = 0;
                }
                else
                {
                    TEST:
                    snprintf(newLog, 255, "%s%s%s.%lld", dirpath.data(), tail, logname.data(), m_count);
                    FILE* fp = NULL;
                    if((fp = fopen(newLog, "r")) != NULL)
                    {
                        ++m_count;
                        goto TEST;
                    }
                    else
                        fclose(fp);
                }
                m_fp = fopen(newLog, "a");
                m_nowfilesize = 0;
            }
        }

        va_list valst;
        va_start(valst, format);

        std::string logStr;
        {
            std::lock_guard<std::mutex> lock{m_mutex};
            int n = snprintf(m_buf, 48, "%4d-%02d-%02d %02d:%02d:%02d.%06ld %s",
                             myTm.tm_year + 1900, myTm.tm_mon+1, myTm.tm_mday,
                             myTm.tm_hour, myTm.tm_min, myTm.tm_sec, now.tv_usec, s);
            int m = vsnprintf(m_buf+n, m_logBufSize-1, format, valst);
            m_buf[n+m-1] = '\n';
            logStr = m_buf;
            m_nowfilesize += n+m;
        }

        if(m_isAsync)
        {
            if(!m_logQueue)return;
            if(!m_logQueue->full())
                m_logQueue->push(logStr);
        }
        else
        {
            std::lock_guard<std::mutex> lock{m_mutex};
            fputs(logStr.data(),m_fp);
        }

        va_end(valst);
    }

    void Logger::flush()
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        fflush(m_fp);
    }

}
