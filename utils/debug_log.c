#include "debug_log.h"

extern size_t vasprintf(char * buf, const char *fmt, args_list args);

void serialLog(char *message, ...){
#ifdef DEBUG_SERIAL
    args_list args;
    args_start(args, message);
    char buf[512];
    vasprintf(buf, message, args);
    if(message[strlen(message)-1]!='\n')
        appendStr(buf, "\n");
    serial_send_data(COM1_PORT, buf);
    args_end(args);
#endif
}
