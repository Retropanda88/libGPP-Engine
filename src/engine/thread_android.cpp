#if defined(ANDROID_BUILD)

#include <engine/thread.h>
#include <thread>
#include <atomic>

class AndroidThread : public Thread {
private:
    std::thread* worker;
    std::atomic<bool> running;

    static void threadWrapper(ThreadFunc func, void* arg, std::atomic<bool>* flag) {
        *flag = true;
        if (func) {
            func(arg);
        }
        *flag = false;
    }

public:
    AndroidThread() : worker(nullptr), running(false) {}

    virtual ~AndroidThread() {
        if (worker) {
            if (worker->joinable()) {
                worker->join();
            }
            delete worker;
        }
    }

    bool start(ThreadFunc func, void* arg) override {
        if (running) return false;

        if (worker) {
            if (worker->joinable()) worker->join();
            delete worker;
            worker = nullptr;
        }

        try {
            worker = new std::thread(threadWrapper, func, arg, &running);
            return true;
        } catch (...) {
            running = false;
            return false;
        }
    }

    void join() override {
        if (worker && worker->joinable()) {
            worker->join();
        }
    }

    bool isRunning() override {
        return running;
    }
};

Thread* createThread() {
    return new AndroidThread();
}

#endif // ANDROID_BUILD
