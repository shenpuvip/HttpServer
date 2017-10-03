#ifndef HS_TCP_H
#define HS_TCP_H

#include "imp.h"
#include "net.h"

namespace SS
{
    struct TCPConn
    {
        TCPConn()
        {

        }

        TCPConn(int fd, struct sockaddr_in addr):fd(fd), addr(addr)
        {

        }

        void close();
        int setNonBlock(bool value = false)
        {
            return net::setNonBlock(fd, value);
        }

        int fd;
        IP4ADDR addr;
    };

    struct TCPServer
    {
        TCPServer(const IP4ADDR& addr):m_addr(addr)
        {
            socket();
            bind();
            //setReuseAddr(true);
            setReusePort(true);
        }
        TCPServer(const std::string& host, short port):m_addr(host, port)
        {
            socket();
            bind();
            //setReuseAddr(true);
            setReusePort(true);
        }
        int listen(int backlog = 10);
        bool accept(TCPConn *conn);
        void close();

        int setNonBlock(bool value = false)
        {
            return net::setNonBlock(m_fd, value);
        }

        int setReuseAddr(bool value = true)
        {
            return net::setReusePort(m_fd, value);
        }

        int setReusePort(bool value = true)
        {
            return net::setReusePort(m_fd, value);
        }

    private:
        int m_fd;
        IP4ADDR m_addr;
        int socket();
        int bind();
    };
}




#endif //HS_TCP_H
