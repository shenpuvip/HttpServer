#include <thread>
#include "poller.h"
#include "http.h"
#include "conf.h"
#include "squeue.h"
#include "logger.h"

extern SS::CONFIG conf;

#ifdef MEMQUE
extern SS::SQueue<SS::HTTPConn*> HTTPConnQue;
#endif

namespace SS
{

    Poller::Poller()
    {
        m_fd = epoll_create1(0);
        if(m_fd < 0)
        {
            LOG_ERROR("epoll_create failed: errno %d", errno);
            LOG_WAIT(5);
            exit(-1);
        }
        activeEvents = new epoll_event[conf.MAXEVENTS];
        connnum.store(0);
    }
    Poller::~Poller()
    {
        if(activeEvents)
            delete [] activeEvents;
    }

    void Poller::loop()
    {
        for(;;)
        {
            loop_once(conf.waitMs);
        }
    }

    int Poller::loop_once(int waitMs)
    {
        int nfds = epoll_wait(m_fd, activeEvents, sizeof(epoll_event)*conf.MAXEVENTS, waitMs);
        for(int i = 0; i < nfds; i ++)
        {
            HTTPConn *conn = (HTTPConn*)activeEvents[i].data.ptr;
            if(!conn)continue;
            int events = activeEvents[i].events;
            if(events & EPOLLIN)
            {
                if(conn->read() < 0)
                {
                #ifdef MEMQUE
                    HTTPConnQue.push(conn);
                #elif !defined(MEMPRE)
                    delete(conn);
                #endif
                    LOG_INFO("CLIENT CLOSE: %s", conn->tcp.addr.toString().data());
                }
            }
            else if(events & EPOLLOUT)
            {
                if(conn->write() < 0)
                {
                #ifdef MEMQUE
                    HTTPConnQue.push(conn);
                #elif !defined(MEMPRE)
                    delete(conn);
                #endif
                    LOG_INFO("CLIENT CLOSE: %s", conn->tcp.addr.toString().data());
                }
            }
            else
            {
                LOG_WARN("unknow event: %d", events);
            }
        }

        return 0;
    }

    int Poller::addHTTPConn(HTTPConn *conn, int event)
    {
        return addEvent(conn->tcp.fd, conn, event);
    }

    int Poller::modHTTPConn(HTTPConn *conn, int event)
    {
        return modEvent(conn->tcp.fd, conn, event);
    }

    int Poller::delHTTPConn(HTTPConn *conn)
    {
        return delEvent(conn->tcp.fd, conn, EPOLLIN);
    }
}
