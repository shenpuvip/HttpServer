#include <thread>
#include <vector>
#include <algorithm>
#include "http.h"
#include "tcp.h"
#include "poller.h"
#include "net.h"
#include "util.h"
#include "conf.h"
#include "squeue.h"
#include "logger.h"

using namespace SS;

#ifdef MEMQUE
SQueue<HTTPConn*> HTTPConnQue(30000);
#elif defined(MEMPRE)
HTTPConn pre[80000];
#endif

struct CONFIG conf;

void initLog();

int main(int argc, char **argv)
{
    const char *conffile = "./config.conf";
    if(argc > 1)
        conffile = argv[1];
    if(conf.readConf(conffile) < 0)
    {
        printf("bad config file\n");
        exit(0);
    }

    initLog();
    conf.show();

    HTTPServer server("0.0.0.0", conf.port);
    LOG_INFO("server running on %d", conf.port);
    Poller pollers[conf.threadnum];
    std::vector<std::thread> threads;
    for(int i = 0; i < conf.threadnum; i ++)
    {
        threads.emplace_back(std::thread(&Poller::loop, std::ref(pollers[i])));
    }

    server.start(conf.backlog);

    HTTPConn tmp;
    for(;;)
    {
        for(int i = 0; i < conf.threadnum; )
        {
            HTTPConn *conn = NULL;

            #ifdef MEMPRE
                   conn = &tmp;
            #elif  MEMQUE
                if(!HTTPConnQue.pop(conn, 10))
                    conn = new HTTPConn;
            #else
                conn = new HTTPConn;
            #endif

            if(!server.accept(conn))
            {
                #ifdef MEMQUE
                    HTTPConnQue.push(conn);
                #elif !defined(MEMPRE)
                    delete(conn);
                #endif

                LOG_WARN("accept failed errno: %d", errno);
                continue;
            }

            #ifdef MEMPRE
                int fd = conn->tcp.fd;
                pre[fd].tcp.fd = fd;
                pre[fd].tcp.addr = conn->tcp.addr;
                conn = &pre[fd];
            #endif

            if(!conn) continue;
            LOG_INFO("NEW ACCEPT: %s", conn->tcp.addr.toString().data());
            conn->tcp.setNonBlock(true);
            conn->poller = &pollers[i];
            //++poller[i].connnum;
            pollers[i].addHTTPConn(conn, EPOLLIN);
            ++i;
        }
    }

    return 0;
}

void initLog()
{
    if(conf.loglevel < 0 || conf.loglevel > 5) conf.loglevel = 1;
    Logger::LOGLEVEL level = (Logger::LOGLEVEL)(conf.loglevel);
    if(Logger::getInstance()->init(conf.logpath.data(), level, 50*1024*1024, 10000))
    {
        std::thread logThread([]() { Logger::flushLogThread(); });
        logThread.detach();
    }
    else
    {
        printf("log start failed\n");
        exit(0);
    }
}