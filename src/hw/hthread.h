#ifndef H_THREAD_H
#define H_THREAD_H

#include <thread>
#include <mutex>
#include <atomic>
#include "hcos.h"
#include "hlog.h"

class HThread{
public:
    HThread() {
        status = STOP;
    }

    virtual ~HThread() {

    }

    virtual int start() {
        if (status == STOP) {
            status = RUNNING;
            thread = std::thread(&HThread::thread_proc, this);
        }
        return 0;
    }

    virtual int stop() {
        if (status != STOP) {
            status = STOP;
            thread.join(); // wait thread exit
        }
        return 0;
    }

    virtual void pause() {
        if (status == RUNNING) {
            status = PAUSE;
        }
    }

    virtual void resume() {
        if (status == PAUSE) {
            status = RUNNING;
        }
    }

    void thread_proc() {
        doPrepare();
        run();
        doFinish();
    }

    virtual void run() {
        while (status != STOP) {
            if (status == PAUSE) {
                msleep(1);
                continue;
            }

            doTask();

            msleep(1);
        }
    }

    virtual void doPrepare() {}
    virtual int doTask() {
        static uint64 cnt = 0;
        cnt++;
        hlogi("%d", cnt);
        return 0;
    }
    virtual void doFinish() {}

    std::thread thread;
    enum STATUS {
        RUNNING,
        PAUSE,
        STOP
    };
    std::atomic<STATUS> status;
};

#endif // H_THREAD_H
