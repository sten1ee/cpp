//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Win32 memory allocator for small objects (1 - 256 bytes)
// Send comments and suggestions to stenly@sirma.bg
// Last modified 19.12.2000
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include <windows.h>

#ifndef UTIL_ASSERT
# define ASSERT(C)
# define DEBUGIN(STM)
#else
# include "util/assert.h"
#endif

#include "allocat.h"
#ifdef ALLOCATOR_DIAG
# include "diag.h"
using namespace diag;
#endif

struct cell_t
{
  cell_t* next_cell;
};

struct blk_t
{
  blk_t*  next_blk;
  blk_t*  prev_blk;
  cell_t* free_cell;
  size_t  nallocated;
};


#ifdef ALLOCATOR_THREADSAFE
# if defined(ALLOCATOR_SYNC_CS)
// Use CRITICAL_SECTION to synchronize allocator:
typedef CRITICAL_SECTION lock_t;

inline
void  CREATE_LOCK(lock_t& lock)
  {
    InitializeCriticalSection(&lock);
  }

inline
void  DESTROY_LOCK(lock_t& lock)
  {
    DeleteCriticalSection(&lock);
  }

inline
void  ACQUIRE_LOCK(lock_t& lock, void*)
  {
    EnterCriticalSection(&lock);
  }

inline
void  RELEASE_LOCK(lock_t& lock, void*)
  {
    LeaveCriticalSection(&lock);
  }

#  define CHECK_LOCK(LOCK,KEY)   ((void)0)
#  define DECLARE_KEY(KEYNAME)   short  KEYNAME

# elif defined(ALLOCATOR_SYNC_MX)
#  include "sync.h"
   typedef mutex lock_t;
#  define CREATE_LOCK(LOCK)      ((void)0)
#  define DESTROY_LOCK(LOCK)     ((void)0)
#  define ACQUIRE_LOCK(LOCK,KEY) ASSERT(KEY); LOCK.acquire(KEY)
#  define RELEASE_LOCK(LOCK,KEY) ASSERT(KEY); LOCK.release(KEY)
#  define CHECK_LOCK(LOCK,KEY)   ASSERT(KEY); ASSERT(LOCK.matches(KEY))
#  define DECLARE_KEY(KEYNAME)   short  KEYNAME

# else
#  if !defined(ALLOCATOR_SYNC_LOCK)
#   define ALLOCATOR_SYNC_LOCK
#  endif
#  include "lock.h"
#  define CREATE_LOCK(LOCK)      create_lock(LOCK)
#  define DESTROY_LOCK(LOCK)     destroy_lock(LOCK)
#  define ACQUIRE_LOCK(LOCK,KEY) acquire_lock(LOCK)
#  define RELEASE_LOCK(LOCK,KEY) release_lock(LOCK)
#  define CHECK_LOCK(LOCK,KEY)   ((void)0)
#  define DECLARE_KEY(KEYNAME)   ((void)0)

# endif
#else
#  define CREATE_LOCK(LOCK)      ((void)0)
#  define DESTROY_LOCK(LOCK)     ((void)0)
#  define ACQUIRE_LOCK(LOCK,KEY) ((void)0)
#  define RELEASE_LOCK(LOCK,KEY) ((void)0)
#  define CHECK_LOCK(LOCK,KEY)   ((void)0)
#  define DECLARE_KEY(KEYNAME)   ((void)0)
#endif

struct grp_t
{
  blk_t*   frst_blk;  // head of the live queue - blocks having free cells
  blk_t*   last_blk;  // last block in the live queue
  size_t   nom_size;  // group's cells nominal size
#ifdef ALLOCATOR_THREADSAFE
  lock_t   lock;
#endif
};

enum
{
  N_DSPS = 64,
  N_GRPS = 20,
};

struct allocator
{
  blk_t*   full_blk;
  blk_t*   free_blk;
  blk_t*   wild_blk;
#if defined(ALLOCATOR_ALIGN_GRPS)
  blk_t*   _pad_1;
#endif
#if defined(ALLOCATOR_THREADSAFE)
  lock_t   full_blk_lock;
  lock_t   free_blk_lock;
# if defined(ALLOCATOR_ALIGN_GRPS) && defined(ALLOCATOR_SYNC_LOCK)
  lock_t   _pad_2;
  lock_t   _pad_3;
# endif
#endif
  grp_t*   dsps[N_DSPS];
  grp_t    grps[N_GRPS];

  char*    p_block(blk_t* blk) const;

  blk_t*   blk_of(void* vp_cell) const;
  grp_t*   grp_of(const blk_t* blk);

  blk_t*   allocate_block(grp_t* grp);
  bool     deallocate_block(blk_t* blk);

  void* operator  new(size_t);
  void  operator  delete(void*);

  allocator(size_t nom_size[], size_t nsz);
 ~allocator();
};

#define HEAP_BASE     ((void*)this)

#ifdef  ALLOCATOR_PAGES_PER_BLOCK
# define _PPB ALLOCATOR_PAGES_PER_BLOCK
#else
# define _PPB 2
#endif

#ifdef  ALLOCATOR_N_BDSC_BLOCKS
# define _NBB ALLOCATOR_N_BDSC_BLOCKS
#else
# define _NBB 128
#endif

enum
{
  PAGE_SIZE        = 4096,                   // VM page size
  BDSC_SIZE        = sizeof(blk_t),          // block descriptor size
  BLOCK_SIZE       = _PPB * PAGE_SIZE,       // block size (tunable)
  N_BDSC_BLOCKS    = _NBB,                   // # of descriptor blocks (tunable)
  N_BDSCS_IN_BLOCK = BLOCK_SIZE / BDSC_SIZE, // # block descriptors in descriptor block
  N_BLOCKS_TOTAL   = N_BDSC_BLOCKS * N_BDSCS_IN_BLOCK,
  N_DATA_BLOCKS    = N_BLOCKS_TOTAL - N_BDSC_BLOCKS,

  SZ_DIV = 4,
  SZ_MAX = N_DSPS * SZ_DIV
};

inline
blk_t*  FRST_BLK(const grp_t* grp)
  {
    return (blk_t*)((char*)&grp->frst_blk - offsetof(blk_t, next_blk));
  }

inline
blk_t*  FULL_BLK(const allocator* alc)
  {
    return (blk_t*)((char*)&alc->full_blk - offsetof(blk_t, next_blk));
  }

inline
bool  has_free_cell(const blk_t* blk)
  {
    return 0 != blk->free_cell;
  }

inline
char*  allocator::p_block(blk_t* blk) const
  {
    return (char*)HEAP_BASE + (blk - (blk_t*)HEAP_BASE) * BLOCK_SIZE;
  }

inline
blk_t* allocator::blk_of(void* vp_cell) const
  {
    ASSERT((char*)HEAP_BASE < (char*)vp_cell);
    unsigned d = (char*)vp_cell - (char*)HEAP_BASE;
    unsigned bid = d / BLOCK_SIZE;
    ASSERT(bid < N_BLOCKS_TOTAL);
    ASSERT(N_BDSC_BLOCKS <= bid);
    return (blk_t*)HEAP_BASE + bid;
  }

inline
grp_t* allocator::grp_of(const blk_t* blk)
  {
    return &grps[blk->nallocated >> 24];
  }

inline
void   commit_block(void* block)
  {
    DEBUGIN(void* res =)
    VirtualAlloc(block, BLOCK_SIZE, MEM_COMMIT, PAGE_READWRITE);
    ASSERT(res == block);
  }

inline
void   decommit_block(void* block)
  {
    DEBUGIN(bool res =)
    VirtualFree(block, BLOCK_SIZE, MEM_DECOMMIT);
    ASSERT(res);
  }

#if defined(UTIL_ASSERT) && !defined(NDEBUG)
class Audit
{
		void*	const   ptr;
    const size_t  sz;
	public:
  	Audit(void* _ptr, size_t _sz)
    	: ptr(_ptr), sz(_sz)
      {
			  DWORD old_prot;
  			VirtualProtect(ptr, sz, PAGE_READWRITE, &old_prot);
      }

   ~Audit()
   		{
			  DWORD old_prot;
  			VirtualProtect(ptr, sz, PAGE_NOACCESS, &old_prot);
      }
};

static bool  asserts()
{
  ASSERT(BLOCK_SIZE >= PAGE_SIZE);
  ASSERT((BLOCK_SIZE & (BLOCK_SIZE - 1)) == 0);
  ASSERT(BLOCK_SIZE %  PAGE_SIZE == 0);
  ASSERT(sizeof(allocator) <= N_BDSC_BLOCKS * BDSC_SIZE);
  ASSERT(sizeof(allocator) <= PAGE_SIZE);
  ASSERT(BLOCK_SIZE <= 0x10000); // 64K
  // or else the following check in allocate_block will go wrong
  // due to block dissalignment (see MEM_RESERVE)
  // if ((int)free_blk % BLOCK_SIZE == 0 && free_blk == wild_blk) {
#if defined(ALLOCATOR_ALIGN_GRPS) && defined(ALLOCATOR_SYNC_LOCK)
  ASSERT(offsetof(allocator, grps[0]) % sizeof(grp_t) == 0);
#endif

  return true;
}
bool _asserts = asserts();

# define AUDIT(ALC) Audit audit(ALC, BLOCK_SIZE * N_BDSC_BLOCKS)
# define CHECK_INTEGRITY(ALC) \
    ASSERT(grp->frst_blk != 0 || grp->last_blk == 0); \
    ASSERT(grp->frst_blk == 0 || grp->frst_blk != ALC->full_blk);\
    ASSERT(grp->frst_blk == 0 || grp->frst_blk != ALC->free_blk);\
    ASSERT(ALC->full_blk == 0 || ALC->full_blk != ALC->free_blk)
#else
# define CHECK_INTEGRITY(ALC)
#endif


void* allocate(allocator* alc, size_t sz)
{
  DEBUGIN(AUDIT(alc));

  DECLARE_KEY(grp_key);
  DECLARE_KEY(full_blk_key);

#ifdef ALLOCATOR_DIAG
  sz += sizeof(size_t);
#endif

  if (0 == sz || SZ_MAX < sz)
    return 0;

  grp_t*  grp = alc->dsps[(sz - 1)/SZ_DIV];

  ACQUIRE_LOCK(grp->lock, &grp_key);

  blk_t* blk = grp->frst_blk;
  if (0 == blk) {
    if (0 == (blk = alc->allocate_block(grp))) {// the allocator is exhausted
      RELEASE_LOCK(grp->lock, &grp_key);
      return 0;
    }
    ASSERT(blk == grp->frst_blk && has_free_cell(blk));
  }
  else {
    if (!has_free_cell(blk)) { // the block is exhautsed
      blk_t* next_blk = blk->next_blk;
      ACQUIRE_LOCK(alc->full_blk_lock, &full_blk_key); // <<<<<<<< Lock full_blk
      if (0 != (blk->next_blk = alc->full_blk)) {
        alc->full_blk->prev_blk = blk;
      }
      (alc->full_blk = blk)->prev_blk = FULL_BLK(alc);
      RELEASE_LOCK(alc->full_blk_lock, &full_blk_key); // >>>>>> Unlock full_blk
      if (0 != (blk = next_blk)) {
        (grp->frst_blk = blk)->prev_blk = FRST_BLK(grp);
      }
      else {
        grp->frst_blk = 0;
        grp->last_blk = 0;
        if (0 == (blk = alc->allocate_block(grp))) {
          RELEASE_LOCK(grp->lock, &grp_key);
          return 0;
        }
      }
    }
  }
  ASSERT(blk == grp->frst_blk && has_free_cell(blk));
  CHECK_INTEGRITY(alc);

  cell_t* cell = blk->free_cell;

  CHECK_LOCK(grp->lock, &grp_key);

  if ((int)cell->next_cell < 0) {
    ASSERT((int)cell->next_cell == -1);
    // indicates a first time allocation for this cell
    if (0 == ((int)cell & (BLOCK_SIZE - 1))) {
      ASSERT((char*)cell == alc->p_block(blk));
      blk->free_cell = 0;
    }
    else {
      (blk->free_cell = (cell_t*)((char*)cell - grp->nom_size))->next_cell = (cell_t*)-1;
    }
  }
  else {
    blk->free_cell = cell->next_cell;
  }

  blk->nallocated++;

#ifdef ALLOCATOR_DIAG
  sz -= sizeof(size_t);
  *(size_t*)cell = sz;
  cell = (cell_t*)((char*)cell + sizeof(size_t));
  ++n_allocated[sz];
  ++c_allocate;
#endif

  RELEASE_LOCK(grp->lock, &grp_key);
  
  return cell;
}


void  deallocate(allocator* alc, void* vp_cell)
{
  DEBUGIN(AUDIT(alc));

  DECLARE_KEY(grp_key);
  DECLARE_KEY(full_blk_key);

  if (vp_cell == 0)
    return;

#ifdef ALLOCATOR_DIAG
  vp_cell = (char*)vp_cell - sizeof(size_t);
  size_t sz = *(size_t*)vp_cell;
#endif

  blk_t* blk = alc->blk_of(vp_cell);
  grp_t* grp = alc->grp_of(blk);

  ASSERT(((char*)vp_cell - alc->p_block(blk)) % grp->nom_size == 0);

  ACQUIRE_LOCK(grp->lock, &grp_key);

  if (!has_free_cell(blk) && blk != grp->frst_blk) {
    // deallocating from dead block
    // detach from dead:
    ACQUIRE_LOCK(alc->full_blk_lock, &full_blk_key); // <<<<<<<<<< Lock full_blk
    if (0 != (blk->prev_blk->next_blk = blk->next_blk)) {
      blk->next_blk->prev_blk = blk->prev_blk;
    }
    RELEASE_LOCK(alc->full_blk_lock, &full_blk_key); // >>>>>>>> Unlock full_blk
    // and insert back to live:
    if (grp->frst_blk == 0) {
      ASSERT(grp->last_blk == 0);
      (grp->frst_blk = blk)->prev_blk = FRST_BLK(grp);
    }
    else {
      (grp->last_blk->next_blk = blk)->prev_blk = grp->last_blk;
    }
    blk->next_blk = 0;
    grp->last_blk = blk;
  }
  CHECK_INTEGRITY(alc);

  ((cell_t*)vp_cell)->next_cell = blk->free_cell;
  blk->free_cell = (cell_t*)vp_cell;

  if ((--blk->nallocated << 8) == 0) { // fully free block
    alc->deallocate_block(blk);
  }
  
#ifdef ALLOCATOR_DIAG
  --n_allocated[sz];
  --c_allocate;
#endif

  RELEASE_LOCK(grp->lock, &grp_key);
}


blk_t*  allocator::allocate_block(grp_t* grp)
{
  DECLARE_KEY(free_blk_key);

  ACQUIRE_LOCK(free_blk_lock, &free_blk_key);

  if ((int)free_blk < 0) { // the reserved space is exhausted - give up !
    ASSERT((int)free_blk == -1);
    RELEASE_LOCK(free_blk_lock, &free_blk_key);
    return 0; // allocator exhausted !
  }
  char* block = p_block(free_blk);
  commit_block(block);
  if ((int)free_blk % BLOCK_SIZE == 0 && free_blk == wild_blk) {
    commit_block(free_blk);
  }

  DEBUGIN(memset(block, 0xFE, BLOCK_SIZE));
  const int NOMS = grp->nom_size;
  (free_blk->free_cell = (cell_t*)(block + ((BLOCK_SIZE - BLOCK_SIZE % NOMS) - NOMS)))->next_cell = (cell_t*)-1;
  ASSERT(((char*)free_blk->free_cell - block) % NOMS == 0);

  blk_t* next_free;
  if (free_blk->next_blk != 0) {
    next_free = free_blk->next_blk;
  }
  else {
    ASSERT(free_blk == wild_blk);
    if ((blk_t*)HEAP_BASE + (N_BLOCKS_TOTAL - 1) <= free_blk)
      next_free = (blk_t*)-1; // allocator exhausted !
    else
      next_free = ++wild_blk;
  }

  blk_t*& frst_blk = grp->frst_blk;
  blk_t*& last_blk = grp->last_blk;
  if (last_blk == 0) {
    (last_blk = free_blk)->next_blk = 0;
  }
  else {
    (free_blk->next_blk = frst_blk)->prev_blk = free_blk;
  }
  (frst_blk = free_blk)->prev_blk = FRST_BLK(grp);
  
  frst_blk->nallocated = (size_t)(grp - &grps[0]) << 24;

  free_blk = next_free;

#ifdef ALLOCATOR_DIAG
  ++c_allocate_block;
#endif

  CHECK_INTEGRITY(this);
  
  RELEASE_LOCK(free_blk_lock, &free_blk_key);

  return frst_blk;
}


bool  allocator::deallocate_block(blk_t* blk)
{
  DECLARE_KEY(free_blk_key);

  ASSERT((blk->nallocated << 8) == 0);

  // this block is fully free !
  grp_t* grp = grp_of(blk);
#ifdef ALLOCATOR_KEEP_FRST_BLKS
  if (blk == grp->frst_blk) {
    return false;
  }
#endif
  if (0 != (blk->prev_blk->next_blk = blk->next_blk)) {
    blk->next_blk->prev_blk = blk->prev_blk;
  }
  else {
    ASSERT(blk == grp->last_blk);
#ifndef ALLOCATOR_KEEP_FRST_BLKS
    if (grp->frst_blk == 0)
      grp->last_blk = 0;
    else
#endif
      grp->last_blk = blk->prev_blk;
  }

  // decommit before acquiring free_blk_lock to improve performance:
  decommit_block(p_block(blk));

  ACQUIRE_LOCK(free_blk_lock, &free_blk_key); // <<<<<<<<<<<<<<<<< Lock free_blk

  blk->next_blk = free_blk;
  free_blk = blk;

#ifdef ALLOCATOR_DIAG
  --c_allocate_block;
#endif

  CHECK_INTEGRITY(this);

  RELEASE_LOCK(free_blk_lock, &free_blk_key); // >>>>>>>>>>>>>>> Unlock free_blk

  return true;
}


void* allocator::operator  new(size_t)
{
  void* vp = VirtualAlloc(0, N_BLOCKS_TOTAL * BLOCK_SIZE, MEM_RESERVE, PAGE_NOACCESS);
#ifdef ALLOCATOR_DIAG
  if (vp != 0)
    printf("[allocator] reserved %dMB at base address %p\n",
      (N_BLOCKS_TOTAL * BLOCK_SIZE) / (1024 * 1024),
      vp);
  else
    printf("![allocator] failed to reserve %dMB !\n",
      (N_BLOCKS_TOTAL * BLOCK_SIZE) / (1024 * 1024));
#endif
  if (vp == 0)
    return 0;
  ASSERT((int)vp % BLOCK_SIZE == 0);
  commit_block(vp);
  return vp;
}


void  allocator::operator  delete(void* vp)
{
  VirtualFree(vp, 0, MEM_RELEASE);
}


allocator::allocator(size_t nom_size[], size_t nsz)
{
  DEBUGIN(AUDIT(this));

  full_blk = 0;
  free_blk = wild_blk = (blk_t*)HEAP_BASE + N_BDSC_BLOCKS;
  size_t k = 0;
  for (size_t i = 0; i < nsz; ++i) {
    size_t sz = grps[i].nom_size = nom_size[i];
    ASSERT(sz % SZ_DIV == 0);
    ASSERT((k + 1) * SZ_DIV <= sz);
    while ((k + 1) * SZ_DIV <= sz) {
      ASSERT(k < N_DSPS);
      dsps[k++] = &grps[i];
    }
  }
  ASSERT(k == N_DSPS);
#ifdef ALLOCATOR_THREADSAFE
  CREATE_LOCK(full_blk_lock);
  CREATE_LOCK(free_blk_lock);
  for (size_t j = 0; j < N_GRPS; ++j) {
    CREATE_LOCK(grps[j].lock);
  }
#endif
}


allocator::~allocator()
{
#ifdef ALLOCATOR_THREADSAFE
  DESTROY_LOCK(full_blk_lock);
  DESTROY_LOCK(free_blk_lock);
  for (size_t i = 0; i < N_GRPS; ++i) {
    DESTROY_LOCK(grps[i].lock);
  }
#endif
}


allocator*  create_allocator(size_t nom_size[], size_t nsz)
{
  return new allocator(nom_size, nsz);
}


void  destroy_allocator(allocator* alc)
{
  delete alc;
}

#ifdef ALLOCATOR_DIAG
void  diag::print_allocator_params(FILE* f)
{
  fprintf(f, "[allocator]\n"
# ifdef ALLOCATOR_THREADSAFE
    "\tthreadsafe, sync = "
#  if   defined(ALLOCATOR_SYNC_CS)
    "CS"    // sync using critical sections
#  elif defined(ALLOCATOR_SYNC_MX)
    "mutex" // sync using mutexes (internal implementation)
#  else
    "lock"  // sync using locks (internal implementation)
#  endif
    ";\n"
# endif
    "\tpages_per_block = %d, n_bdsc_blocks = %d, keep_frst_blks = %s, \n"
    "\talign_grps = %s;\n",
    BLOCK_SIZE/PAGE_SIZE,
    N_BDSC_BLOCKS,
# ifdef ALLOCATOR_KEEP_FRST_BLKS
  "yes",
# else
  "no",
# endif
# ifdef ALLOCATOR_ALIGN_GRPS
  "yes"
# else
  "no"
# endif
    );
}
#endif

#ifndef DISABLE_DEFAULT_ALLOCATOR
static size_t  nom_size[N_GRPS] = { 4,  8, 12, 16,
                                   20, 24, 28, 32,
                                   40, 48, 56, 64,
                                   80, 96,112,128,
                                  160,192,224,256};

allocator_t default_allocator = new allocator(nom_size, sizeof(nom_size) / sizeof(*nom_size));
#endif
