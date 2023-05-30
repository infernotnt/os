#pragma once

#include "../lib/hw.h"
#include "../h/my_console.h"
#include "../h/my_console.h"

void max(uint64 a, uint64 b);

class MemAlloc
{
public:
    struct FreeNode;
    struct TakenNode;

    static MemAlloc* get();

    FreeNode* pFreeHead;
    TakenNode* pTakenHead;

    void* allocMem(size_t size);
    int freeMem(void* p);
    uint64 getUserlandUsage();
    void printUserlandUsage();

    const size_t MAX_NODE_SIZE;
private:
    // TODO: obrisati assignment operator, copy construktor, mozda move konstruktor
    MemAlloc();
};

struct MemAlloc::FreeNode
{
    FreeNode* pNext;
    size_t size;
    char* base;

    // TODO: testirati da li ovo za nullptr default radi
    FreeNode(FreeNode* pNext = nullptr)
    : pNext(pNext)
        { }
};



struct MemAlloc::TakenNode
{
    TakenNode* pNext;
    size_t size;
    char* base;

    // TODO: testirati da li ovo za nullptr default radi
    TakenNode(TakenNode* pNext = nullptr)
    : pNext(pNext)
        { }
};
