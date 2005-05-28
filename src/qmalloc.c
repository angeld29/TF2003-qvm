/*
  Semi-braindamaged Malloc for Quake 3 Virtual Machine.
  Copyright 2002  PhaethonH <phaethon@linux.ucla.edu>

  Permission granted to copy, modify, distribute, and use, provided this
  copyright notice remains intact.  Source is provided on an as-is basis,
  with no warranties.
  $Id: qmalloc.c,v 1.3 2005-05-28 18:33:52 AngelD Exp $
*/

typedef int size_t;

#include "qmalloc.h"


static unsigned char arena[MALLOC_ARENA];

static int *_arena_metadata = 0;
static unsigned char *_arena_data = 0;
static int _arena_pages = 0;
static int _arena_freestart = 1;

enum {
  MALLOC_MAGIC = 0x6B8E6EB9,
  PAGESIZE = 256,
};

/*
Every PAGESIZE-bytes chunk of heap is attached to one integer metadata.
Metadata holds a key, the value of the starting address of the memory block.
The result is _arena_pages number of chunks, with a corresponding _arena_pages
number of metadata integers.
*/


static void
malloc_init ()
{
  if (((int*)arena)[0] == MALLOC_MAGIC)
      return;  /* Already initialized. */

  {
    int divider;
    divider = PAGESIZE + sizeof(*_arena_metadata);
    _arena_pages = MALLOC_ARENA / divider;
    _arena_metadata = (int*)arena;
    _arena_data = (unsigned char*)(_arena_metadata + _arena_pages);
    ((int*)arena)[0] = MALLOC_MAGIC;
  }
 G_dprintf("malloc_init: %d pages of %d bytes.  Metadata @ %08X, heap @ %08X\n", _arena_pages, PAGESIZE, _arena_metadata, _arena_data);
}


static int
findspot (int pages)
{
  int i, j;

  malloc_init();
//printf("findspot: for %d pages\n", pages);
//  for (i = 1; i < (_arena_pages - pages); i++)
  if (_arena_freestart >= (_arena_pages - pages))
      _arena_freestart = 1;
//printf("findspot: for %d pages starting @ %d\n", pages, _arena_freestart);
  for (i = _arena_freestart; i < (_arena_pages - pages); i++)
    {
      if (_arena_metadata[i] == 0)
        {
          for (j = 0; j < pages; j++)
            {
              if (_arena_metadata[i+j] != 0)
                  break;
            }
          if (j < pages)  /* loop ended because of insufficient free pages. */
            {
              i += j;    /* Skip the blocks we know we can't use. */
              continue;  /* with  for i = ... */
            }
          for (j = 0; j < pages; j++)
            {
              _arena_metadata[i+j] = (int)(_arena_data + (i * PAGESIZE));
            }
//printf("findspot: returning page %d (x %d) @ %08X\n", i, pages, (_arena_data + (i * PAGESIZE)));
          _arena_freestart = i;
          return i;
        }
    }
  G_dprintf("FATAL: findspot: Heap exhausted\n");
  return 0;
}


static int
usedblocks (void *ptr)
{
  int i;
  int retval;

  malloc_init();
  retval = 0;
  for (i = 0; i < _arena_pages; i++)
    {
      if (_arena_metadata[i] == (int)ptr)
          retval++;
    }
  return retval;
}


void *
malloc(size_t size)
{
  int pages;
  int n;
  void *retval;

  pages = 1 + ((size - 1) / PAGESIZE);
  n = findspot(pages);
  retval = (void*)(_arena_data + (n * PAGESIZE));
//printf("malloc: for %d bytes -> %08X\n", size, retval);
  return retval;
}


void *
calloc(size_t nmemb, size_t size)
{
  char *retval,*p;
  int i;

  retval = (char*)(malloc(nmemb * size));
/*  for (i = 0; i < (nmemb * size); i++)
    {
      retval[i] = 0;
    }*/
  memset( retval, 0, (nmemb * size));

  return (void*)retval;
}


void
free(void *ptr)
{
  int n;

  malloc_init();
//  n = 0;
  n = ((unsigned char*)ptr - _arena_data) / PAGESIZE;
  if ((n < 0) || (n > _arena_pages))
    {
      /* Outside heap.  Bad. */
      return;
    }
  _arena_freestart = n;  /* Next allocation tries here, to see if it fits. */
  while (_arena_metadata[n] == (int)ptr)
    {
      _arena_metadata[n] = 0;
      n++;
    }
  return;
}


void *
realloc(void *ptr, size_t size)
{
  void *m;
  int n, movesize;
  int pages;

  if (size == 0)
    {
      free(ptr);
      return 0;
    }
  m = malloc(size);
  movesize = usedblocks(ptr) * PAGESIZE;
//printf("realloc: from %08X to %08X, old size %d, new size %d\n", ptr, m, movesize, size);
  if (size < movesize)
      movesize = size;
//printf("realloc: moving %d bytes\n", movesize);
//  memmove(m, ptr, movesize);
  memcpy(m, ptr, movesize);
  free(ptr);
  return m;
}


#define _ALLOCA_RINGSIZE 256

void *
alloca(size_t size)
{
  static void* alloca_strips[_ALLOCA_RINGSIZE] = { 0, };
  static int snum = 0;
  void *p;

  if (size < 0)
    {
      /* Hack: reset alloca allocations */
//printf("alloca: reset hack\n");
      for (snum = 0; snum < _ALLOCA_RINGSIZE; snum++)
        {
          free(alloca_strips[snum]);
          alloca_strips[snum] = 0;
        }
      snum = 0;
      return 0;
    }

  if (alloca_strips[snum])
    {
      free(alloca_strips[snum]);
      alloca_strips[snum] = 0;
    }
  p = malloc(size);
  if (p == 0)
    {
      G_dprintf("FATAL: malloc out of memoy\n");
    }
  alloca_strips[snum] = p;
  snum = (snum + 1) % _ALLOCA_RINGSIZE;
  return p;
}


void
alloca_reset()
{
  alloca(-1);
}



#if 0


/* Debugging routines. */

void
dump_mheader (void *x)
{
  printf("metadata %08X\n", x);
}

void
dump_block (void *x)
{
  int i;
//  for (i = 0; i < PAGESIZE; i++)
//    {
//      printf("%02X ", i[(char*)x]);
//    }
  printf("\n");
//  dump_mheader((void*)((char*)x - sizeof(mheader_t)));
}


int
test_malloc ()
{
  char *a, *b, *c, *d, *e, *f;

  printf("arena @ %d\n", arena);
//  printf("arenaned @ %d\n", arenaend);
  printf("pages = %d\n", _arena_pages);

  a = (char*)malloc(700);
  dump_mheader(arena);
  printf("a @ %08X\n", a);
  dump_block(a);

  b = (char*)malloc(70000);
  printf("b @ %08X\n", b);
  dump_block(b);

printf("\n");

  free(a);
  dump_mheader(arena);
  dump_block(b);

  c = (char*)malloc(100);
  printf("c @ %08X\n", c);
  dump_block(c);

printf("\n");

  dump_mheader(arena);
  c = (char*)realloc(c, 3);
  printf("c @ %08X\n", c);
  dump_block(c);

  c = (char*)realloc(c, 3000);
  printf("c @ %08X\n", c);
  dump_block(c);


  return 1;
}


/* testing with GCC */
int
main()
{
  test_malloc();
  return 0;
}

#endif
