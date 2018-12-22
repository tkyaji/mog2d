#ifndef mogmalloc_h
#define mogmalloc_h

#include <stdlib.h>

#define mogmalloc(sz)           malloc(sz)
#define mogrealloc(p,newsz)     realloc(p,newsz)
#define mogfree(p)              free(p)
#define mogcalloc(sz1,sz2)      calloc(sz1,sz2)
#define mogmalloc_initialize()  ;
#define mogmalloc_finalize()    ;


#endif /* mogmalloc_h */
