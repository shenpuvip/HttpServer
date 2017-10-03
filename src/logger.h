#ifndef HS_LOGGER_H
#define HS_LOGGER_H

#include "squeue.h"
#include <mutex>
#include <condition_variable>

namespace SS
{
    class Logger
    {
    public:
        enum LOGLEVEL{OFF, ERROR, WARN, INFO, DEBUG, ALL};

        static Logger* getInstance()
        {
            static Logger instance;
            return &instance;
        }

        static void flushLogThread()
        {
            Logger::getInstance()->asyncWriteLog();
        }

        bool init(const char* filename, Logger::LOGLEVEL level, int filesize = 5000000, int maxQueueSize = 0);

        void writeLog(LOGLEVEL level, const char* format, ...);

        void flush();


    private:
        Logger();
        virtual ~Logger();

        void asyncWriteLog()
        {
            if(!m_logQueue) return;
            std::string logtext;
            int haveNoFlush = 0;
            while(true)
            {
                if(!m_logQueue->pop(logtext, 10000))
                {
                    if(haveNoFlush)
                        fflush(m_fp), haveNoFlush = 0;
                    continue;
                }
                std::lock_guard<std::mutex> lock(m_mutex);
                fputs(logtext.data(), m_fp);
                haveNoFlush = 1;
            }
        }

    private:
        std::mutex m_mutex;
        std::string dirpath;
        std::string logname;
        int64_t m_nowfilesize;
        int64_t m_filesize;
        int m_logBufSize;
        long long m_count;
        LOGLEVEL m_level;
        int m_today;
        FILE* m_fp;
        char* m_buf;
        SQueue<std::string> *m_logQueue;
        bool m_isAsync;
    };

#define LOG

#ifdef LOG
#define LOG_DEBUG(format, ...) Logger::getInstance()->writeLog(Logger::DEBUG, format, __VA_ARGS__)
#define LOG_INFO(format, ...) Logger::getInstance()->writeLog(Logger::INFO, format, __VA_ARGS__)
#define LOG_WARN(format, ...) Logger::getInstance()->writeLog(Logger::WARN, format, __VA_ARGS__)
#define LOG_ERROR(format, ...) Logger::getInstance()->writeLog(Logger::ERROR, format, __VA_ARGS__)
#define LOG_WAIT(s) std::this_thread::sleep_for(std::chrono::seconds(s))
#else
#define LOG_DEBUG(format, ...)
#define LOG_INFO(format, ...)
#define LOG_WARN(format, ...)
#define LOG_ERROR(format, ...)
#define LOG_WAIT(s)
#endif //LOG
}


#endif //HS_LOGGER_H
