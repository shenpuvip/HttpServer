#ifndef HS_HTTP_H
#define HS_HTTP_H

#include "imp.h"
#include <string>
#include <unordered_map>
#include "net.h"
#include "tcp.h"
#include "poller.h"

namespace SS
{
    struct HTTPRequest
    {
        std::string method, path;
        int keep_alive;
    };

    struct HTTPResponse
    {
        int status_code;
        std::string reason;
    };

    struct HTTPConn
    {
        HTTPRequest req;
        HTTPResponse res;
        TCPConn tcp;
        Poller *poller;

        HTTPConn()
        {

        }

        HTTPConn(const TCPConn& tcpconn):tcp(tcpconn)
        {
            ;
        }

        HTTPConn(int fd, struct sockaddr_in addr):tcp(fd, addr)
        {
            ;
        }

        void close();

        int read();
        int write();
        void parserRequest(char *buf, int len);
    };

    struct HTTPServer
    {
        TCPServer tcp;

        HTTPServer(const TCPServer& tcp):tcp(tcp)
        {
            ;
        }

        HTTPServer(const std::string& host, int port):tcp(host, port)
        {

        }

        void start(int backlog = 10)
        {
            tcp.listen(backlog);
        }

        void close()
        {
            tcp.close();
        }

        bool accept(HTTPConn *conn);
    };
}


#endif //HS_HTTP_H
