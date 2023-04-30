#ifndef PROJECT_BASE_V1_1_ALLOC_H
#define PROJECT_BASE_V1_1_ALLOC_H

#endif //PROJECT_BASE_V1_1_ALLOC_H

#include "../lib/hw.h"
#include "../h/my_console.h"

class MemAlloc
{
public:
    struct MemNode
    {
        MemNode* pPrev;
        MemNode* pNext;
        size_t size;
        char* base;


        // TODO: testirati da li ovo za nullptr default radi
        MemNode(MemNode* pPrev = nullptr, MemNode* pNext = nullptr)
        : pPrev(pPrev), pNext(pNext)
        { }
    };

    static MemAlloc* get();

    MemNode freeHead;

    void* allocMem(size_t size);
    int freeMem(void* p);

private:
    // TODO: obrisati assignment operator, copy construktor, mozda move konstruktor
    MemAlloc();
};