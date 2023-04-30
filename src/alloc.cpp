#include "../h/alloc.h"
#include "../lib/console.h"
#include "../h/my_console.h"

MemAlloc::MemAlloc()
{
    freeHead.size = 1024 * 1024;
}

// TODO: wtf? ne mogu staviti pNext na neke vrednosti tipa 100, 1000, 10^n...
// : freeHead(0, (MemNode*)((sizeof(MemAlloc)+1)*100))
//  :freeHead(0, (MemNode*)100)
//  putInt((long long)freeHead.pPrev);
//  putInt((long long)freeHead.pNext);
//  freeHead.pPrev = (MemNode*)0;
//  freeHead.pNext = (MemNode*)100000000;

MemAlloc* MemAlloc::get()
{
    static MemAlloc instance;
    return &instance;
}

void* MemAlloc::allocMem(size_t size)
{
    // Algorithm: puts it in the first place large enough found

    size_t initialSize = size;

    putString("=== called: allocMem(");
    putU64(size);
    putString(")");
    putNewline();

    if(size % MEM_BLOCK_SIZE != 0) // align size
    {
        size += MEM_BLOCK_SIZE - (size % MEM_BLOCK_SIZE);
    }

    assert(size % MEM_BLOCK_SIZE == 0);

    MemNode* pCur = &freeHead;
    while(pCur != nullptr)
    {
        if(pCur->size >= size)
        {
            assert((uint64)pCur->base % MEM_BLOCK_SIZE == 0); // check for alignment errors
//            void* ret = pCur->base;
            pCur->size -= size;
            pCur->base += size;
//            return ret;
            return (void*)(initialSize*2);
        }

        pCur = pCur->pNext;
    }

    assert(false); // there is no slot large enough

    return nullptr;
}

int MemAlloc::freeMem(void* p)
{
    assert(false);
    return 0;
}
