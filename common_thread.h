#include <thread>

#ifndef THREAD_H_
#define THREAD_H_

class Thread {
    private:
        std::thread thread;
 
    public:
        Thread() {}

        void start();

        void join();

        virtual void run() = 0;
        
        virtual ~Thread() {}

    private:
        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        Thread(Thread&& other);

        Thread& operator=(Thread&& other);
};
#endif
