#if defined(ANDROID_BUILD)

#include <engine/mutex.h>
#include <mutex>

class AndroidMutex : public Mutex {
private:
    std::mutex mtx;
public:
    AndroidMutex() {}
    virtual ~AndroidMutex() {}

    void lock() override {
        mtx.lock();
    }

    void unlock() override {
        mtx.unlock();
    }
};

Mutex* createMutex() {
    return new AndroidMutex();
}

#endif // ANDROID_BUILD
