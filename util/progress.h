#ifndef UTIL_PROGRESS_BAR_H
#define UTIL_PROGRESS_BAR_H

class ProgressBar
{
  public:
    virtual      ~ProgressBar()                      {}
    virtual void  start(const char* sJobDescription) {}
    virtual void  show(const char* sJobStatus)       {}
    virtual void  finish()                           {}
};

#endif//UTIL_PROGRESS_BAR_H
