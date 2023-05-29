#include"../h/my_tests.h"

void externalInterruptTest()
{
    putString("=== Testing externalInterruptTest");
    putNewline();

    uint64 oldTimer = gTimer;
    while(true)
    {
        if(gTimer - oldTimer == 10)
            break;

        assert(gTimer - oldTimer <= 11);
    }

    __asm__ volatile("mv x10, x10");
    putString("=== PASS in externalInterruptTest");
    putNewline();
}

uint64 semVal = 0;

void testSemaphores()
{
    assert(semVal == 0);

    putString("=== Testing semaphores");
    putNewline();

    sem_t sem1, sem2, sem3, semFinal;
    int stat = sem_open(&sem1, 1);
    assert(stat == 0);
    stat = sem_open(&sem2, 0);
    assert(stat == 0);
    stat = sem_open(&sem3, 1);
    assert(stat == 0);
    stat = sem_open(&semFinal, 0);
    assert(stat == 0);

    assert(sem1 == INIT_SEM && sem2 == INIT_SEM+1);
    assert((uint64)&sem1 - (uint64)&sem2 == 8);

    void doSemaphoresA(void*);
    thread_t a;
    thread_create(&a, doSemaphoresA, &sem1);

    semVal = 69;

    for(int i=0; i<1000; i++)
    {
        stat = sem_wait(sem1);
        assert(stat == 0);

        assert(semVal == 69);
        semVal = 420;

        stat = sem_signal(sem2);
        assert(stat == 0);
    }

    stat = sem_close(sem1);
    assert(stat == 0);
    stat = sem_close(sem2);
    assert(stat == 0);


//    stat = sem_wait(sem3); // the purpose of this wait and the following signal is to test the "-1" return value of sem_wait that is ended by a sem_closed. (sem3 is helper for this, we actualy test for semFinal)
//    assert(stat == 0);

    stat = sem_wait(semFinal);
    assert(stat == -1);

    thread_join(a);

    putString("=== PASS in semaphore test");
    putNewline();
}

void doSemaphoresA(void*p)
{
    sem_t sem1 = *((sem_t*)p);
    sem_t sem2 = *((sem_t*)p-1);  // "-" instead of + because stack grows downwards
    sem_t sem3 = *((sem_t*)p-2);
    sem_t semFinal = *((sem_t*)p-3);

    assert(sem1 == INIT_SEM && sem2 == INIT_SEM+1 && sem3 == INIT_SEM+2 && semFinal == INIT_SEM+3);

    for(int i=0; i<1000; i++)
    {
        int stat = sem_wait(sem2);
        assert(stat == 0);

        assert(semVal == 420);
        semVal = 69;

        stat = sem_signal(sem1);
        assert(stat == 0);
    }

    while(ISemaphore::pAllSemaphores[semFinal]->value == 0) // this is to ensure the other thread is waiting on this one before we close this one
    {
        assert(ISemaphore::pAllSemaphores[semFinal]->value == -1);
        thread_dispatch();
    }

    int stat = sem_close(semFinal);
    assert(stat == 0);

    stat = sem_close(sem3);
    assert(stat == 0);
}

void testTimeSleep()
{
    putString("=== Testing \"testTimeSleep\"");
    putNewline();

    thread_t a, b;

    uint64 argA = 4;
    uint64 argB = 1;

    void doSleepA(void*);

    thread_create(&a, doSleepA, &argA);
    thread_create(&b, doSleepA, &argB);

    thread_join(a);
    thread_join(b);

    putString("=== PASS in \"testTimeSleep\"");
    putNewline();
}

void doSleepA(void* p)
{
    uint64 *n = (uint64*)p;

    uint64 i = *n;
    while(i <= 8)
    {
        int a = fib(i);

        uint64 oldTimer = gTimer;
        int b = time_sleep(a);

        assert(b == 0);
        assert((oldTimer - gTimer) <= (((uint64)a) + 2));
        assert((oldTimer - gTimer) >= ((uint64)a));

        i++;
    }
}

int sliceFirstCounter = 0;
int sliceSecondCounter = 0;

void testTimeSlice()
{
    assert(sliceFirstCounter == 0);
    assert(sliceSecondCounter == 0);

    putString("=== Testing \"testTimeSlice\"");
    putNewline();

    void doP(void*);
    void doQ(void*);

    uint64 n = 150000000;

    void doSliceFirst(void*);
    void doSliceSecond(void*);

    thread_t sliceFirst;
    thread_create(&sliceFirst, &doSliceFirst, &n);

    thread_t sliceSecond;
    thread_create(&sliceSecond, &doSliceSecond, &n);

    thread_join(sliceFirst);
    thread_join(sliceSecond);

    int dif = (sliceFirstCounter - sliceSecondCounter);
    if(dif < 0) // dif = abs(dif);
        dif = -dif;

    assert(dif < sliceFirstCounter * 8/10); // WARNING: not 100% check, depends on "n" and many other stuff

    putString("=== PASS in testing \"testTimeSlice\"");
    putNewline();
}


void doSliceFirst(void* n)
{
    uint64 oldTimer = gTimer;
    for(uint64 i=0; i<(*((uint64*)n)); i++)
    {
        if(oldTimer != gTimer)
        {
            __asm__ volatile("mv x10, x10");

            sliceFirstCounter++;
            oldTimer = gTimer;
        }
    }
    assert(sliceFirstCounter > 0); // "n" in test too small probably
}

void doSliceSecond(void* n)
{
    uint64 newN = *(uint64*)n;

    uint64 oldTimer = gTimer;
    for(uint64 i=0; i<newN; i++)
    {
        __asm__ volatile("mv x10, x10");
        assert((*((uint64*)n)) == newN);

        if(oldTimer != gTimer)
        {
            __asm__ volatile("mv x10, x10");

            sliceSecondCounter++;
            assert((((int)sliceFirstCounter) - ((int)sliceSecondCounter)) <= 1);
            oldTimer = gTimer;
        }

        __asm__ volatile("mv x10, x10");
    }

    assert(sliceSecondCounter > 0); // "n" in test too small probably
}

void testSyncCall()
{
//    assert(MemAlloc::get()->getUserlandUsage() == IThread::initialUserMemoryUsage);

    putString("=== Testing \"testSyncCall\"");
    putNewline();

    void doA(void*);
    void doB(void*);

    int argA = 69;
    int argB = 420;
    thread_t a;
    thread_t b;
    thread_create(&a, doA, &argA);
    thread_create(&b, doB, &argB);
    thread_dispatch();

    thread_join(b);
    thread_join(a);

    putString("=== PASS in testing \"testSyncCall\"");
    putNewline();
}

void doA(void* p)
{
    assert(*((int*)p) == 69);

    for(int i=0; i<=5; i++)
    {
        assert(fib(i) == test_call(i));
        thread_dispatch();
    }

    __asm__ volatile("mv x10, x10");
    thread_exit();
    assert(false);
}

void doB(void* p)
{
    assert(*((int*)p) == 420);

    for(int i=0; i<=10; i++)
    {
        assert(fib(i) == test_call(i));
        thread_dispatch();
    }

    thread_exit();
    assert(false);
}


uint64 fib(uint64 n)
{
    if(n == 1)
        return 1;
    else if (n == 0)
        return 0;
    else return fib(n-1) + fib(n-2);
}

void testSystemCalls()
{
    putString("=== Testing \"testSystemCalls\"");
    putNewline();

    int a = 0;
    int b = 0;

    for(int i=1; i<10; i++)
    {
        a++;
        b++;

        uint64 called = test_call(i);
        uint64 actual = fib(i);

        if(called != actual)
        {
            putString("test_call() error. i=");
            putInt(i);
            putString(" fib(i)=");
            putU64(actual);
            putString(" test_call(i)=");
            putU64(called);
            putNewline();
            assert(false);
        }

        assert(a == b);
    }

//    assert(MemAlloc::get()->getUserlandUsage() == IThread::initialUserMemoryUsage);
    putString("=== PASS in testing system calls");
    putNewline();
}

void testMemoryAllocator()
{
    putString("=== Testing \"testMemoryAllocator\"");
    putNewline();
    int n = 1000;
    int **a;
    a = (int**)mem_alloc(n * sizeof(void*));
    for(int i=0; i<n; i++)
    {
        a[i] = (int*)mem_alloc(sizeof(int));
        *a[i] = i;
    }

    int result = 0;
    for(int i=0; i<n; i++)
    {
        result += *a[i];
    }

//    uint64 vau = n * sizeof(void*);
//    if(n % MEM_BLOCK_SIZE != 0)
//        vau +=  MEM_BLOCK_SIZE - ((sizeof(int)* n) % MEM_BLOCK_SIZE);
//    vau += MEM_BLOCK_SIZE * n;
//    assert(MemAlloc::get()->getUserlandUsage() == vau);

//    putInt(result);
//    putNewline();

    assert(result == (n-1) * n / 2);

//    uint64 oldTaken = MemAlloc::get()->getUserlandUsage();
//    uint64 newTaken;

    int t;
    for(int i=0; i<n; i++)
    {
        t = mem_free(a[i]);
        assert(t == 0);

//        newTaken = MemAlloc::get()->getUserlandUsage();
//        assert(oldTaken - newTaken == MEM_BLOCK_SIZE);
    }

//    assert(MemAlloc::get()->getUserlandUsage() == MEM_BLOCK_SIZE);

    t = mem_free(a);
    assert(t == 0);

//    assert(MemAlloc::get()->getUserlandUsage() == IThread::initialUserMemoryUsage);
    putString("=== PASS in testing memory allocator");
    putNewline();
}


