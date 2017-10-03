#include <fcntl.h>
#include <netdb.h>
#include <string>
#include <cstring>
#include "util.h"
#include "net.h"

namespace SS
{

    int net::setNonBlock(int fd, bool value) {
        int flags = fcntl(fd, F_GETFL, 0);
        if (flags < 0) {
            return errno;
        }
        if (value) {
            return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
        }
        return fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    }

    int net::setReuseAddr(int fd, bool value) {
        int flag = value;
        int len = sizeof flag;
        return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, len);
    }

    int net::setReusePort(int fd, bool value) {
        int flag = value;
        int len = sizeof flag;
        return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, len);
    }


    struct in_addr net::getHostByName(const std::string& host)
    {
        struct in_addr addr;
        char buf[1024];
        struct hostent hent;
        struct hostent* he = NULL;
        int herrno = 0;
        memset(&hent, 0, sizeof hent);
        int r = gethostbyname_r(host.c_str(), &hent, buf, sizeof buf, &he, &herrno);
        if (r == 0 && he && he->h_addrtype==AF_INET) {
            addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
        } else {
            addr.s_addr = INADDR_NONE;
        }
        return addr;
    }


    IP4ADDR::IP4ADDR(const std::string& host, short port) {
        memset(&m_addr, 0, sizeof m_addr);
        m_addr.sin_family = AF_INET;
        m_addr.sin_port = htons(port);
        if (host.size()) {
            m_addr.sin_addr = net::getHostByName(host);
        } else {
            m_addr.sin_addr.s_addr = INADDR_ANY;
        }
        if (m_addr.sin_addr.s_addr == INADDR_NONE){
            m_addr.sin_addr.s_addr = INADDR_ANY;
        }
    }

    std::string IP4ADDR::toString() const {
        uint32_t uip = m_addr.sin_addr.s_addr;
        return util::format("%d.%d.%d.%d:%d",
                            (uip >> 0)&0xff,
                            (uip >> 8)&0xff,
                            (uip >> 16)&0xff,
                            (uip >> 24)&0xff,
                            ntohs(m_addr.sin_port));
    }

    std::string IP4ADDR::ip() const {
        uint32_t uip = m_addr.sin_addr.s_addr;
        return util::format("%d.%d.%d.%d",
                            (uip >> 0)&0xff,
                            (uip >> 8)&0xff,
                            (uip >> 16)&0xff,
                            (uip >> 24)&0xff);
    }

    short IP4ADDR::port() const {
        return ntohs(m_addr.sin_port);
    }

    unsigned int IP4ADDR::ipInt() const {
        return ntohl(m_addr.sin_addr.s_addr);
    }

}