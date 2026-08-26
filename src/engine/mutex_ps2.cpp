#if defined(PS2_BUILD)

#include <engine/mutex.h>
#include <kernel.h>
#include <stdio.h>

class PS2Mutex : public Mutex {
private:
    int semaId;
public:
    PS2Mutex() {
        ee_sema_t sema;
        sema.init_count = 1; // 1 = Disponible para el primer lock
        sema.max_count = 1;  // Semáforo binario exclusivo
        sema.option = 0;     
        
        semaId = CreateSema(&sema);
        if (semaId < 0) {
            printf("[ERROR] PS2Mutex: Failed to create semaphore! Code: %d\n", semaId);
        }
    }

    ~PS2Mutex() {
        if (semaId >= 0) {
            DeleteSema(semaId);
        }
    }

    void lock() {
        if (semaId >= 0) {
            WaitSema(semaId);
        }
    }

    void unlock() {
        if (semaId >= 0) {
            SignalSema(semaId);
        }
    }
};

Mutex* createMutex() {
    return new PS2Mutex();
}

#endif // PS2_BUILD
