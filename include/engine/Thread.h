#ifndef GPP_THREAD_H
#define GPP_THREAD_H

typedef void (*ThreadFunc)(void* arg);

class Thread {
public:
    virtual ~Thread() {}
    
    virtual bool start(ThreadFunc func, void* arg) = 0;
    virtual void join() = 0;
    virtual bool isRunning() = 0;
};

Thread* createThread();

#endif