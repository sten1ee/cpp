#ifndef UTIL_ASSERT_H
#define UTIL_ASSERT_H

#if !defined(DEBUG) && !defined(NDEBUG)
# define DEBUG
#endif

#ifdef DEBUG
# define ASSERT(COND)   ((COND) ? void(0) : __AssertFail(__FILE__, __LINE__))
# define ASSERT_NZ(EXP) (__AssertNonZero(EXP,__FILE__, __LINE__))
# define ASSERT_Z(EXP)  (__AssertZero   (EXP,__FILE__, __LINE__))
# define DEBUGIN(STM)    STM
# define SAFE_CAST(POBJ, CLASS) (dynamic_cast<CLASS*>(POBJ))
# define SURE_CAST(POBJ, CLASS) (ASSERT_NZ(dynamic_cast<CLASS*>(POBJ)))

class __Assert
{
  protected:
    __Assert();

  public:
    const char* file;
    unsigned    line;

    __Assert(const char* file, unsigned line)
      : file(file),
        line(line)
      {}

    void  operator = (const __Assert& a)
      {
        file = a.file;
        line = a.line;
      }
};

// called on assertion failure:
void  __AssertFail(const char* file, unsigned line) throw (__Assert);

template  <class T>
inline
T   __AssertNonZero(T t, const char* file, unsigned line)
{
  if (t)
    ;
  else
    __AssertFail(file, line);
  return t;
}

template  <class T>
inline
T   __AssertZero(T t, const char* file, unsigned line)
{
  if (t)
    __AssertFail(file, line);
  return t;
}

#else
# define ASSERT(COND)   (void(0))
# define ASSERT_NZ(EXP) (EXP)
# define ASSERT_Z(EXP)  (EXP)
# define DEBUGIN(STM)
# define SAFE_CAST(POBJ, CLASS) dynamic_cast<CLASS*>(POBJ)
# define SURE_CAST(POBJ, CLASS) static_cast<CLASS*>(POBJ)
#endif //defined(DEBUG)

#endif //UTIL_ASSERT_H
