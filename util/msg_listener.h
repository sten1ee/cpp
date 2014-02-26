#ifndef util_msg_listener_h
#define util_msg_listener_h

#include <stdarg.h>
#include <stdio.h>

class msg_listener
{
  public:
    virtual ~msg_listener() {;}

    virtual void  vmessagef(const char* fmt, va_list argptr)=0;

    void  messagef(char* fmt, ...)
      {
        va_list  argptr;
        va_start(argptr, fmt);
        vmessagef(fmt, argptr);
        va_end(argptr);
      }

    void  message(char* msg)
      {
        messagef("%s", msg);
      }
};    

class fprintf_msg_listener : public msg_listener
{
    FILE* file;

  public:
    fprintf_msg_listener(FILE* file)
      : file(file)
      {;}

    virtual void  vmessagef(const char* fmt, va_list argptr)
      {
        vfprintf(file, fmt, argptr);
        fprintf(file, "\n");
      }
};

#endif //util_message_listener_h
