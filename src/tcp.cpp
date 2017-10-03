#include <sys/socket.h>
#include <unistd.h>
#include "tcp.h"
#include "logger.h"


namespace SS
{
    void TCPConn::close()
    {
        if(fd >= 0)
        {
            ::close(fd);
            fd = -1;
        }
    }

    int TCPServer::socket()
    {
        m_fd = ::socket(AF_INET, SOCK_STREAM, 0);
        if(m_fd < 0)
        {
            LOG_ERROR("TCP socket failed errno: %d", errno);
            exit(-1);
        }
        return m_fd;
    }

    int TCPServer::bind()
    {
        struct sockaddr_in saddr = m_addr.getAddr();
        int res = ::bind(m_fd, (struct sockaddr *) &saddr, sizeof(saddr));
        if(res < 0)
        {
            LOG_ERROR("TCP bind failed errno: %d", errno);
            exit(-1);
        }
        return res;
    }

    int TCPServer::listen(int backlog)
    {
        int res = ::listen(m_fd, backlog);
        if(res < 0)
        {
            LOG_ERROR("TCP listen failed errno: %d", errno);
            exit(-1);
        }
        return res;
    }

    bool TCPServer::accept(TCPConn *conn)
    {
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientFd = ::accept(m_fd, (struct sockaddr *) &clientAddr, &clientAddrLen);
        if(clientFd < 0)
        {
            LOG_WARN("TCP ACCEPT failed errno: %d", errno);
            return false;
        }
        conn->fd = clientFd;
        conn->addr = clientAddr;
        return true;
    }

    void TCPServer::close()
    {
        if(m_fd >= 0)
        {
            ::close(m_fd);
            m_fd = -1;
        }
    }
}