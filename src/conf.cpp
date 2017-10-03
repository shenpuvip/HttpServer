#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cctype>
#include "conf.h"

namespace SS
{
    int CONFIG::readConf(const char *filename)
    {
        init();
        FILE *fp = fopen(filename, "r");
        if(!fp)
            return -1;
        char buf[256];
        while(fgets(buf, 200, fp))
        {
            if(feof(fp)) break;
            if(strchr(buf, '#'))continue;
            char *delim = strchr(buf, '=');
            if(!delim)continue;
            *delim = 0;
            ++delim;
            if(strcmp("PORT", buf) == 0)
            {
                this->port = atoi(delim);
            }
            else if(strcmp("ROOT", buf) == 0)
            {
                for(int i = 0; delim[i] && i < 80; i ++)
                    if(isspace(delim[i]))
                    {
                        delim[i] = 0;
                        break;
                    }
                this->rootpath = delim;
            }
            else if(strcmp("THREADNUM", buf) == 0)
            {
                this->threadnum = atoi(delim);
            }
            else if(strcmp("BACKLOG", buf) == 0)
            {
                this->backlog = atoi(delim);
            }
            else if(strcmp("WAITMS", buf) == 0)
            {
                this->waitMs = atoi(delim);
            }
            else if(strcmp("MAXEVENTS", buf) == 0)
            {
                this->MAXEVENTS = atoi(delim);
            }
            else if(strcmp("LOGPATH", buf) == 0)
            {
                for(int i = 0; delim[i] && i < 80; i ++)
                    if(isspace(delim[i]))
                    {
                        delim[i] = 0;
                        break;
                    }
                this->logpath = delim;
            }
            else if(strcmp("LOGLEVEL", buf) == 0)
            {
                this->loglevel = atoi(delim);
            }
            else if(strcmp("LOGSIZE", buf) == 0)
            {
                this->logsize = atoi(delim);
                this->logsize *= 1024*1024;
            }
        }
        return 0;
    }
}