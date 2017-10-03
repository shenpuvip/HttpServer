#ifndef HS_CONF_H
#define HS_CONF_H

#include <cstdint>
#include <string>

//#define MEMPRE

namespace SS
{
    struct CONFIG
    {
        uint16_t port;
        std::string rootpath;
        int threadnum;
        int backlog;
        int waitMs;
        int MAXEVENTS;
        int memory;
        std::string logpath;
        int loglevel;
        int logsize;

        int readConf(const char *filename);
        void init()
        {
            port = 61480;
            rootpath = "./";
            threadnum = 3;
            backlog = 2000;
            waitMs = -1;
            MAXEVENTS = 2000;
            memory = 0;
            logpath = "./hs.log";
            loglevel = 1;
            logsize = 50*1024*1024;
        }
        void show()
        {
            printf("PORT: %d\n", port);
            printf("WAITMS: %d\n", waitMs);
            printf("BACKLOG: %d\n", backlog);
            printf("THREADNUM: %d\n", threadnum);
            printf("ROOTPATH: %s\n", rootpath.data());
            printf("LOGLEVEL: %d\n", loglevel);
            printf("LOGSIZE: %dM\n", logsize/1024/1024);
            printf("LOGPATH: %s\n", logpath.data());
            printf("MAXEVENTS: %d\n", MAXEVENTS);
//            printf("MEMORY: ");
//            if(memory == 0)
//                printf("new\n");
//            else if(memory == -1)
//                printf("queue\n");
//            else
//                printf("pre\n");
        }
    };
}


#endif //HS_CONF_H
