//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Win32 memory allocator for small objects (1 - 256 bytes)
// Send comments and suggestions to stenly@sirma.bg
// Last modified 07.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef UTIL_ALLOCAT_H
#define UTIL_ALLOCAT_H

#include <stddef.h>

typedef struct allocator* allocator_t;

#ifdef ENABLE_CUSTOM_ALLOCATOR
allocator_t create_allocator(size_t nom_size[], size_t nsz);
void       destroy_allocator(allocator_t alc);
#endif

void*   allocate(allocator_t alc, size_t sz);
void  deallocate(allocator_t alc, void*  vp);

#ifndef DISABLE_DEFAULT_ALLOCATOR
extern allocator_t default_allocator;

inline void*   allocate(size_t sz) { return allocate(default_allocator, sz); }
inline void  deallocate(void*  vp) {      deallocate(default_allocator, vp); }
#endif

#endif//UTIL_ALLOCAT_H
