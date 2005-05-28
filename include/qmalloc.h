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
