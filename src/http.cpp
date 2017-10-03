#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include "imp.h"
#include "http.h"
#include "conf.h"
#include "squeue.h"
#include "logger.h"

extern SS::CONFIG conf;

namespace SS
{
    int HTTPConn::read()
    {
        int len;
        char buf[4096];
        if((len =::read(tcp.fd, buf, sizeof(buf)-1)) > 0)
        {
            buf[len] = 0;
            parserRequest(buf, len);

            poller->modHTTPConn(this, EPOLLOUT);
            return 0;
        }
        else if(len < 0)
            LOG_ERROR("write failed errno: %d", errno);
        poller->delHTTPConn(this);
        --poller->connnum;
        close();
        return -1;
    }

    void HTTPConn::parserRequest(char *buf, int len)
    {
        char *hend = NULL;
        if((hend = strstr(buf, "\r\n\r\n")) == NULL)
        {

            return;
        }
        *hend = 0;

        int i = 0;
        char *method = buf;
        while(buf[i] != ' ') buf[i] = toupper(buf[i]), ++i;
        buf[i] = 0;
        req.method = method;

        ++i;
        char *path = buf + i;
        while(buf[i] != ' ') ++i;
        buf[i] = 0;
        req.path = path;

        ++i;
        for(int j = i; j < len; j ++)
            buf[j] = tolower(buf[j]);
        if(strstr(buf+i, "keep-alive"))
            req.keep_alive = 1;
        else
            req.keep_alive = 0;
    }

#define KEEPHEADER "HTTP/1.1 %d %s\r\n\
Server: HS/0.0.1\r\n\
Connection: Keep-Alive\r\n\
Keep-Alive: timeout=60\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
\r\n"

#define NOKEEPHEADER "HTTP/1.1 %d %s\r\n\
Server: HS/0.0.1\r\n\
Connection: close\r\n\
Content-Type: text/html\r\n\
Content-Length: %d\r\n\
\r\n"

    int HTTPConn::write()
    {
        int contentLength = 0;
        if(req.method != "GET")
            res.status_code = 501, res.reason = "Not implemented";
        else if(req.path.find("../") != std::string::npos)
            res.status_code = 404, res.reason = "Not Found";
        else
        {
            req.path = conf.rootpath + req.path;
            struct stat stbuf;
            if(stat(req.path.data(), &stbuf) < 0)
            {
                res.status_code = 404, res.reason = "Not Found";
            }
            else if(!(S_ISREG(stbuf.st_mode)) || !(S_IRUSR & stbuf.st_mode))
            {
                res.status_code = 403, res.reason = "Forbidden";
            }
            else
            {
                res.status_code = 200, res.reason = "OK";
                contentLength = stbuf.st_size;
            }
        }

        char header[300];
        if(req.keep_alive)
            snprintf(header, sizeof(header), KEEPHEADER, res.status_code, res.reason.data(), contentLength);
        else
            snprintf(header, sizeof(header), NOKEEPHEADER, res.status_code, res.reason.data(), contentLength);

        if(::write(tcp.fd, header, strlen(header)) < 0)
        {
            LOG_ERROR("write failed errno: %d", errno);
            poller->delHTTPConn(this);
            close();
            --poller->connnum;
            return -1;
        }
        else if(contentLength > 0)
        {
            int fd = ::open(req.path.data(), O_RDONLY, 0);
            sendfile(tcp.fd, fd, NULL, contentLength);
            ::close(fd);
        }

        if(!req.keep_alive)
        {
            poller->delHTTPConn(this);
            close();
            --poller->connnum;
            return -1;
        }
        poller->modHTTPConn(this, EPOLLIN);
        return 0;
    }

    void HTTPConn::close()
    {
        tcp.close();
    }

    bool HTTPServer::accept(HTTPConn* conn)
    {
        if(!tcp.accept(&(conn->tcp)))
            return false;
        return true;
    }
}