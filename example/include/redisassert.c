
#include "redisassert.h"
#include <stdio.h>
#include <stdarg.h>



void _serverPanic(const char *file, int line, const char *msg, ...) {
    va_list ap;
    va_start(ap,msg);
    char fmtmsg[256];
    vsnprintf(fmtmsg,sizeof(fmtmsg),msg,ap);
    va_end(ap);

    // bugReportStart();
    fprintf(stderr, "------------------------------------------------");
    fprintf(stderr, "!!! Software Failure. Press left mouse button to continue");
    fprintf(stderr,"Guru Meditation: %s #%s:%d",fmtmsg,file,line);
    fprintf(stderr,"------------------------------------------------");
    *((char*)-1) = 'x';
}

//redis-cli
void _serverAssert(const char *estr, const char *file, int line) {
    fprintf(stderr, "=== ASSERTION FAILED ===");
    fprintf(stderr, "==> %s:%d '%s' is not true",file,line,estr);
    *((char*)-1) = 'x';
}