#ifndef HS_NET_H
#define HS_NET_H

#include <string>
#include <cstdint>
#include <netinet/in.h>

namespace SS
{

    struct net
    {
        static int setNonBlock(int fd, bool value = true);
        static int setReuseAddr(int fd, bool value = true);
        static int setReusePort(int fd, bool value = true);

        static uint16_t hton(uint16_t v) { return htons(v); }
        static uint32_t hton(uint32_t v) { return htonl(v); }
        static uint16_t ntoh(uint16_t v) { return ntohs(v); }
        static uint32_t ntoh(uint32_t v) { return ntohl(v); }

        static struct in_addr getHostByName(const std::string& host);
    };

    struct IP4ADDR
    {
        IP4ADDR(){}
        IP4ADDR(const std::string& host, short port);
        IP4ADDR(short port): IP4ADDR("", port) {}
        IP4ADDR(const struct sockaddr_in& addr):m_addr(addr){}

        std::string toString() const;
        std::string ip() const;
        short port() const;
        unsigned int ipInt() const;
        struct sockaddr_in& getAddr() {return m_addr;}
        struct sockaddr_in m_addr;
    };

}


#endif //HS_NET_H
