#include "htask.h"
#include "hcos.h"
#include "hlog.h"

HTask::HTask(task_param_s* param, HTask* parent) : HObj(parent) {
    status = READY;
    errcode = 0;
    memcpy(&tps, param, sizeof(task_param_s));
}

HTask::STATUS HTask::getStatus() {
    // judge timeout
    if (status == EXECUTING) {
        if (gettick() - start_ts > tps.timeout) {
            status = Error;
            errcode = ERR_TASK_TIMEOUT;
            return status;
        }
    }

    // judge sub_task
    int sub_status = -1;
    auto iter = children.begin();
    while (iter != children.end()) {
        HTask* pTask = (HTask*)(*iter);
        sub_status = pTask->getStatus();
        if ( sub_status == Error ){
            status = Error;
            errcode = pTask->errcode;
            return status;
        }else if (sub_status == DONE) {
            sub_status = DONE;
        }
    }

    if (status == EXECUTING && sub_status == DONE)
        status = SUBDONE;

    return status;
}

void HTask::start() {
    start_ts = gettick();
    status = EXECUTING;

    auto iter = children.begin();
    while (iter != children.end()) {
        HTask* pTask = (HTask*)(*iter);
        pTask->start();
    }
    
    exec();
}

void HTask::wait() {
    if (tps.bSyn) {
        mutex.lock();

        // note: wait until call destroy
        mutex.lock();
        mutex.unlock();
        delete this;
    }
}

void HTask::destroy() {
    if (tps.bSyn) {
        // note: just unlock, delete this by wait()
        mutex.unlock();
    }else{
        delete this;
    }
}