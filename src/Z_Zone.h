#ifndef Z_ZONE_H__
#define Z_ZONE_H__

void  Z_Init(void);
// void* SDLCALL Z_Malloc(size_t size);
// void* SDLCALL Z_Calloc(size_t nmemb, size_t size);
// void* SDLCALL Z_Realloc(void* ptr, size_t size);
// void  SDLCALL Z_Free(void* ptr);

void*  Z_Malloc(size_t size);
void*  Z_Calloc(size_t nmemb, size_t size);
void*  Z_Realloc(void* ptr, size_t size);
void   Z_Free(void* ptr);
int   Z_FreeMemory(void);

#endif
