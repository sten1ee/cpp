#ifndef UTIL_DIRENT_H
#define UTIL_DIRENT_H

#include <dirent.h>
#include <stdio.h>

#include "util/shared_rep.h"
#include "util/debug.h"

class file_ent;
class dir_ent;
class dir_iterator;

const char DIR_SEPARATOR = '\\';
 
class file_ent
{
  public:

  protected:
    const char* _path;

  public:
   ~file_ent()
      { delete[] _path; }

    file_ent(const file_ent& f);
    file_ent(const char* p);
    file_ent(const dir_ent& d, const char* n);

    const  char* path() const; // eg. `C:\documents\letters\letter.txt'
    const  char* name() const; // eg. `letter.txt'
    const  char* ext () const; // eg. `.txt'

    static bool  path_exists(const char* path);
    static bool  path_is_dir(const char* path);
           bool  exists() const;
           bool  is_dir() const;
};

class dir_ent : public file_ent
{
  public:
    dir_ent(const dir_ent& d);
    dir_ent(const file_ent& f);
    dir_ent(const char* p);
    dir_ent(const dir_ent& d, const char* n);

    dir_iterator  iterator() const;

//    static dir_ent  resolve(const char* path);
};

struct dir_iterator_data
{
  dir_iterator_data(const dir_ent& d)
      : de(d)
    {;}

  dir_iterator_data()
      : de(*(dir_ent*)0)
    {
      UTIL_ASSERT(0);
    }

  const dir_ent&  de;
  DIR*            DIR;
  dirent*         dirent;
  int             errno;
};

class dir_iterator : protected shared_rep<dir_iterator_data>
{
  protected:
    typedef shared_rep<dir_iterator_data> super;

    void  advance()
      {
        rep().dirent = ::readdir(rep().DIR);
      }

    friend class dir_ent;

    dir_iterator(const dir_ent* d)
        : super(*d)
      {
        if (0 == (rep().DIR = ::opendir(d->path()))) {
          rep().errno  = ::errno;
          rep().dirent = 0;
        }
        else {
          rep().errno  = 0;
          advance();
        }
      }

  public:

   ~dir_iterator()
      {
        ::closedir(rep().DIR);
      }

    operator bool () const
      {
        return rep().dirent != 0;
      }

    file_ent  operator * () const
      {
        return file_ent(rep().de, rep().dirent->d_name);
      }

    dir_iterator& operator ++ ()
      {
        advance();
        return *this;
      }
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// class file_ent inlines
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline
file_ent::file_ent(const file_ent& f)
  {
    char* fp = new char[strlen(f.path()) + 1];
    strcpy(fp, f.path());
    _path = fp;
  }

inline
file_ent::file_ent(const char* p)
  {
    char* fp = new char[strlen(p) + 1];
    strcpy(fp, p);
    _path = fp;
  }

inline
file_ent::file_ent(const dir_ent& d, const char* n)
  {
    int dp_len = strlen(d.path());
    int append_ds = (dp_len == 0 || d.path()[dp_len - 1] != DIR_SEPARATOR);
    char* fp = new char[dp_len + append_ds + strlen(n) + 1];
    strcpy(fp, d.path());
    if (append_ds) {
      fp[dp_len]   = DIR_SEPARATOR;
      fp[++dp_len] = 0;
    }
    strcpy(fp + dp_len, n);
    _path = fp;
  }

inline
const char*   file_ent::path() const  // eg. `C:\documents\letters\letter.txt'
  {
    return _path;
  }

inline
const char*   file_ent::name() const  // eg. `letter.txt'
  {
    const char* ds = strrchr(path(), DIR_SEPARATOR);
    if (ds != 0)
      return ds + 1;
    else
      return path();
  }

inline
const char*   file_ent::ext () const  // eg. `.txt'
  {
    return strrchr(path(), '.');
  }

inline
bool  file_ent::path_exists(const char* path)
  {
    FILE* f = fopen(path, "r");
    if (f) {
      fclose(f);
      return true;
    }
    else {
      return false;
    }
  }

inline
bool  file_ent::path_is_dir(const char* path)
  {
    DIR* d = ::opendir(path);
    if (d) {
      ::closedir(d);
      return true;
    }
    else {
      return false;
    }
  }

inline
bool  file_ent::exists() const
  {
    return path_exists(path());
  }

inline
bool  file_ent::is_dir() const
  {
    return path_is_dir(path());
  }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// class dir_ent inlines
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
inline
dir_ent::dir_ent(const dir_ent& d)
    : file_ent(d)
  {
    UTIL_ASSERT(is_dir());
  }

inline
dir_ent::dir_ent(const file_ent& f)
    : file_ent(f)
  {
    UTIL_ASSERT(is_dir());
  }

inline
dir_ent::dir_ent(const char* p)
    : file_ent(p)
  {
    UTIL_ASSERT(is_dir());
  }

inline
dir_ent::dir_ent(const dir_ent& d, const char* n)
    : file_ent(d, n)
  {
    UTIL_ASSERT(is_dir());
  }

inline
dir_iterator  dir_ent::iterator() const
  {
    return dir_iterator(this);
  }

#endif //UTIL_DIRENT_H
