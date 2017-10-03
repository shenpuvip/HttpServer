#ifndef HS_POLLER_H
#define HS_POLLER_H

#include <sys/epoll.h>
#include <atomic>
#include "http.h"
#include "imp.h"
namespace SS
{
    struct Poller
    {
        Poller();
        ~Poller();
        void loop();
        int loop_once(int waitMs);
        int addEvent(int fd, void *ptr, int event)
        {
            struct epoll_event ev;
            ev.events = event;
            ev.data.ptr = ptr;
            int res = epoll_ctl(m_fd, EPOLL_CTL_ADD, fd, &ev);
            return res;
        }
        int modEvent(int fd, void *ptr, int event)
        {
            struct epoll_event ev;
            ev.events = event;
            ev.data.ptr = ptr;
            int res = epoll_ctl(m_fd, EPOLL_CTL_MOD, fd, &ev);
            return res;
        }
        int delEvent(int fd, void *ptr, int event)
        {
            struct epoll_event ev;
            ev.events = event;
            ev.data.ptr = ptr;
            int res = epoll_ctl(m_fd, EPOLL_CTL_DEL, fd, &ev);
            return res;
        }

        int addHTTPConn(HTTPConn *conn, int event);
        int modHTTPConn(HTTPConn *conn, int event);
        int delHTTPConn(HTTPConn *conn);

        std::atomic<int> connnum;

    private:
        int m_fd;
        struct epoll_event *activeEvents;
    };
}

#endif //HS_POLLER_H
