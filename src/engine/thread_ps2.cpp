
/**
 * @file thread_ps2.cpp
 * @brief PS2 Thread implementation for libGPP-Engine.
 * 
 * ============================================================================
 * CRITICAL DEVELOPER NOTES & CONCURRENCY GUIDELINES FOR PLAYSTATION 2 (EE):
 * ============================================================================
 * 
 * 1. VOLATILE VISIBILITY:
 *    Any shared state variable between threads (such as running flags or buffer 
 *    cursors) MUST be declared as `volatile`. The PS2 toolchain optimizer (-O2) 
 *    caches variables in CPU registers; without `volatile`, the main thread will 
 *    never see updates made by secondary threads.
 * 
 * 2. CPU YIELDING (RotateThreadReadyQueue):
 *    The PS2 kernel scheduler requires secondary threads to yield control 
 *    voluntarily. Infinite loops inside worker threads MUST include a call to 
 *    `RotateThreadReadyQueue(32)` to prevent starving the main thread and system 
 *    subsystems of CPU cycles.
 * 
 * 3. AVOID BUSY-WAITING / SPIN-LOCKS:
 *    Do not run high-contention loops without pauses. Forcing multiple threads 
 *    to aggressively fight for a Mutex without yielding will lock up the 
 *    Emotion Engine scheduler and freeze the console.
 * 
 * 4. THREAD-SAFETY LIMITATIONS:
 *    Standard I/O (`printf`) and graphic pipelines (`gsKit`) are NOT thread-safe. 
 *    Secondary threads should only process data and interact with Mutex-protected 
 *    buffers. Let the main loop handle all rendering and console output.
 * 
 * 5. STACK & GLOBAL POINTER (_gp):
 *    Each thread allocates its own stack buffer (recommended 32 KB minimum) 
 *    aligned via `memalign()`. The global pointer register (`_gp`) must be 
 *    explicitly assigned (`tdata.gp_reg = &_gp`) to prevent segmentation faults 
 *    when switching execution contexts.
 * ============================================================================
 */
 
#if defined(PS2_BUILD)

#include <engine/thread.h>
#include <kernel.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdio.h>

#define THREAD_STACK_SIZE (32 * 1024)

extern char _gp;

class PS2Thread : public Thread {
private:
    int threadId;
    ThreadFunc userFunc;
    void* userArg;
    u8* stackBuffer;
    volatile bool running;

    static void entryPoint(void* arg) {
        PS2Thread* self = (PS2Thread*)arg;
        
        if (self != NULL && self->userFunc != NULL) {
            self->userFunc(self->userArg);
        }
        
        if (self != NULL) {
            self->running = false;
        }
        
        ExitThread();
    }

public:
    PS2Thread() {
        threadId = -1;
        userFunc = NULL;
        userArg = NULL;
        running = false;
        stackBuffer = (u8*)memalign(64, THREAD_STACK_SIZE);
    }

    ~PS2Thread() {
        if (threadId >= 0) {
            TerminateThread(threadId);
            DeleteThread(threadId);
        }
        if (stackBuffer) {
            free(stackBuffer);
        }
    }

    bool start(ThreadFunc func, void* arg) {
        if (running) return false;

        userFunc = func;
        userArg = arg;

        ee_thread_t tdata;
        tdata.func = (void*)PS2Thread::entryPoint;
        tdata.stack = stackBuffer;
        tdata.stack_size = THREAD_STACK_SIZE;
        tdata.gp_reg = &_gp;
        tdata.initial_priority = 32;
        tdata.option = 0;

        threadId = CreateThread(&tdata);
        if (threadId < 0) {
            printf("[ERROR] CreateThread failed with ID: %d\n", threadId);
            return false;
        }

        running = true;
        
        int ret = StartThread(threadId, this);
        if (ret < 0) {
            printf("[ERROR] StartThread failed with code: %d\n", ret);
            running = false;
            return false;
        }

        // Ceder turno al planificador para que el kernel estabilice el hilo
        RotateThreadReadyQueue(32);

        return true;
    }

    void join() {
        while (running) {
            RotateThreadReadyQueue(32);
        }
    }

    bool isRunning() {
        return running;
    }
};

Thread* createThread() {
    return new PS2Thread();
}

#endif // PS2_BUILD