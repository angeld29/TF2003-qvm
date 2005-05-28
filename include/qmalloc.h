/*
  Semi-braindamaged Malloc for Quake 3 Virtual Machine.
  Copyright 2002  PhaethonH <phaethon@linux.ucla.edu>

  Permission granted to copy, modify, distribute, and use, provided this
  copyright notice remains intact.  Source is provided on an as-is basis,
  with no warranties.
  $Id: qmalloc.h,v 1.2 2005-05-28 16:39:48 AngelD Exp $
*/

#ifndef _QMALLOC_H_
#define _QMALLOC_H_

#ifndef NULL
#define NULL (0)
#endif

enum {
  MALLOC_ARENA = 1024 * 8192,  /* 8MB of heap memory. */
};

void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *alloca(size_t size);
void alloca_reset();

#endif
