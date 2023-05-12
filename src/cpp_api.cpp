#include"../h/cpp_api.h"

void* operator new(size_t size)
{
    return mem_alloc(size);
}

void operator delete(void* p)
{
    mem_free(p);
}
