#include "../h/alloc.h"
#include "../h/my_console.h"
#include "../lib/hw.h"

uint64 alignForward(uint64 n, uint64 alignConst)
{
    assert(alignConst == MEM_BLOCK_SIZE);
    if(n % MEM_BLOCK_SIZE != 0) // align size
    {
        n += MEM_BLOCK_SIZE - (n % MEM_BLOCK_SIZE);
    }

    assert(n % alignConst == 0);

    return n;
}

uint64 alignBackward(uint64 n, uint64 alignConst)
{
    assert(alignConst == MEM_BLOCK_SIZE);
    uint64 ret = n - n % MEM_BLOCK_SIZE;;
    assert(ret % MEM_BLOCK_SIZE == 0);
    return ret;
}

void MemAlloc::printUserlandUsage()
{
    uint64 a = getUserlandUsage();
    kPutString("Allocated ");
    kPutU64(a);
    kPutString(" bytes are allocated for userland");
    kPutNewline();
}

MemAlloc::MemAlloc()
: MAX_NODE_SIZE(sizeof(FreeNode) > sizeof(TakenNode) ? sizeof(FreeNode) : sizeof(TakenNode))
{
    pFreeHead = (FreeNode*)alignForward((uint64)HEAP_START_ADDR, MEM_BLOCK_SIZE);
    pFreeHead->base = (char*)alignForward((uint64)pFreeHead + MAX_NODE_SIZE, MEM_BLOCK_SIZE);
    pFreeHead->size = alignBackward((uint64)HEAP_END_ADDR - (uint64)pFreeHead->base, MEM_BLOCK_SIZE);

    pTakenHead = nullptr;

    assert(((uint64)pFreeHead->size) / 1024 / 1024 == 127);
}


MemAlloc* MemAlloc::get()
{
    static MemAlloc instance;
    return &instance;
}

void* MemAlloc::allocMem(size_t size)
{
//    kPutInt(size);
//    kPutNewline();

    // Algorithm: puts it in the first place large enough found

    size = alignForward(size, MEM_BLOCK_SIZE);

    size_t usableSize = size;
    size_t actualSize = alignForward(usableSize + MAX_NODE_SIZE, MEM_BLOCK_SIZE);
    assert(usableSize % MEM_BLOCK_SIZE == 0 && actualSize % MEM_BLOCK_SIZE == 0);
    size = 0; // WARNING: use actualSize and initialSize instead

    FreeNode* pCur = pFreeHead;
    FreeNode* pPrev = nullptr;

//    assert(pFreeHead->size > 1000000);

//    uint64 tempOriginalSize = pFreeHead->size;

    while(pCur != nullptr)
    {
        assert(((uint64) pCur->base) % MEM_BLOCK_SIZE == 0 &&
               pCur->size % MEM_BLOCK_SIZE == 0); // check for alignment errors
        assert(((uint64)pCur) % MEM_BLOCK_SIZE == 0);

        FreeNode *newFree;
        newFree = (FreeNode *) alignForward((uint64) (pCur->base + usableSize), MEM_BLOCK_SIZE);
        char* newFreeBase = (char*)alignForward((uint64)newFree + MAX_NODE_SIZE, MEM_BLOCK_SIZE);
        if (pCur->base + pCur->size > newFreeBase)
        {
            // success: memory block found
            newFree->base = newFreeBase;

            newFree->size = pCur->base + pCur->size - newFree->base;
            assert(newFree->size % MEM_BLOCK_SIZE == 0 && (uint64)newFree->base % MEM_BLOCK_SIZE == 0 &&
                   ((uint64) newFree) % MEM_BLOCK_SIZE == 0);

//            assert();
//            if(!(tempOriginalSize - newFree->size <= actualSize + alignForward(MAX_NODE_SIZE, MEM_BLOCK_SIZE)))
//            {
//                __asm__ volatile("mv x10, x10");
//                uint64 sizeDiff = tempOriginalSize - newFree->size;
//                assert(sizeDiff);
////                kPutU64(tempOriginalSize);
////                IConsole::get()->flush();
//            }

            newFree->pNext = pCur->pNext; // replace pCur with the new one

            if(pPrev != nullptr) // ???
                pPrev->pNext = newFree;
            else
            {
                assert(pCur == pFreeHead);
                pFreeHead = newFree;
            }

            TakenNode* newTaken = (TakenNode*)pCur;
            newTaken->pNext = pTakenHead;
            newTaken->base = pCur->base;
            newTaken->size = usableSize;
            pTakenHead = newTaken;

            void *ret = newTaken->base;
            return ret;
        }

        pPrev = pCur;
        pCur = pCur->pNext;
    }

    assert(false); // there is no slot large enough
    return nullptr;
}

int MemAlloc::freeMem(void* p)
{
    TakenNode* pCur = pTakenHead;
    TakenNode* pPrev = nullptr;

    while(pCur != nullptr)
    {
        assert((char*)alignBackward((uint64)pCur->base - MAX_NODE_SIZE, MEM_BLOCK_SIZE) == (char*)pCur); // nisam siguran da je tacno
        if(pCur->base == p)
        {
            // sta ako ne postoji pPrev? sta ako ne postoji pNext->pNext?
            if(pPrev) // ???
                pPrev->pNext = pCur->pNext; // "delete" current taken block from the linked list
            else
            {
                assert(pCur == pTakenHead);
                pTakenHead = pCur->pNext;
            }

            FreeNode* newFree = (FreeNode*)alignBackward(((uint64)p) - MAX_NODE_SIZE, MEM_BLOCK_SIZE);
            newFree->size = pCur->size;
            newFree->base = pCur->base;
            newFree->pNext = pFreeHead;
            pFreeHead = newFree;

            return 0; // zero signifies sucesss
        }

        pPrev = pCur;
        pCur = pCur->pNext;
    }

    kPutString("=== Error: mem_free was maby given an incorrect adress?");
    kPutNewline();
    assert(false); // temp
    return -1; // negative values signify failure
}

uint64 MemAlloc::getUserlandUsage()
{
    uint64 ret = 0;
    TakenNode *pCur = pTakenHead;
    while(pCur != nullptr)
    {
        ret += pCur->size;
        pCur = pCur->pNext;
    }

    return ret;
}