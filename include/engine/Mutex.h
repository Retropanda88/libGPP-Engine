#ifndef GPP_MUTEX_H
#define GPP_MUTEX_H

class Mutex {
public:
    virtual ~Mutex() {}
    virtual void lock() = 0;
    virtual void unlock() = 0;
};

Mutex* createMutex();

#endif