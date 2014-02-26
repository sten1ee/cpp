#ifndef UTIL_ASSERT_H
#define UTIL_ASSERT_H

#define ASSERT(COND) \
          ((COND) ? void(0) : (void)__AssertionFailure(__FILE__, __LINE__))

extern  void* __debug_ptr;
extern  void  __AssertionFailure(const char* file, unsigned line);

class FailedAssert
{
  protected:
    FailedAssert();
  public:
    const char* file;
    unsigned    line;

    FailedAssert(const char* file, unsigned line)
      : file(file),
        line(line)
      {}
    void  operator = (const FailedAssert& fa)
      {
        file = fa.file;
        line = fa.line;
      }
};

#endif //UTIL_ASSERT_H
