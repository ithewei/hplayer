#include "htaskmanager.h"
#include "herr.h"

IMPL_SINGLETON(HTaskManager)

HTaskManager::HTaskManager () {
    limit = TASK_DEQUE_DEFAULT_LIMIT;
    next_serialNO = 0;
}

HTaskManager::~HTaskManager(){

}

int HTaskManager::addTask(HTask* pTask) {
    do{
        std::lock_guard<std::mutex> locker(ready_tasks_mutex);

        if (limit != TASK_DEQUE_NO_LIMIT && ready_tasks.size() >= limit) {
            SAFE_DELETE(pTask);
            return ERR_TASK_DEQUE_FULL;
        }

        pTask->serialNO = next_serialNO++;
        ready_tasks.push_back(pTask);
    }while(0);

    uint32 serialNO = pTask->serialNO;
    hlogd("-------------------------------------------%d", serialNO);
    pTask->wait();

    int errcode = get_id_errcode(serialNO);
    hlogd("===========================================%d=>%d", serialNO, errcode);
    return errcode;
}

int HTaskManager::doTask() {
    HTask* pTask = NULL;
    do {
        std::lock_guard<std::mutex> locker(ready_tasks_mutex);

        if (ready_tasks.size() == 0)
            break;

        pTask = ready_tasks.front();
        ready_tasks.pop_front();
    }while(0);

    if (pTask) {
        HTask::STATUS status = pTask->getStatus();
        if (status == HTask::READY){
            pTask->start();

            // move to wait_tasks
            wait_tasks_mutex.lock();
            wait_tasks[pTask->serialNO] = pTask;
            wait_tasks_mutex.unlock();
        }
    }else{
        // note: idle => traverse wait_tasks, erase ERROR and DONE status tasks. 
        msg_cb(-1, NULL);
    }

    return 0;
}

int HTaskManager::msg_cb(HTask* pTask, void* msg){
    if (pTask && msg){
        pTask->msg_cb(msg);
        HTask::STATUS status = pTask->getStatus();
        if (status == HTask::Error || status == HTask::DONE){
            pTask->destroy();
            return 1;
        }
    }

    return 0;
}

int HTaskManager::msg_cb(int64 serialNO, void* msg) {
    std::lock_guard<std::mutex> locker(wait_tasks_mutex);
    auto iter = wait_tasks.find(serialNO);
    if (iter != wait_tasks.end()){
        HTask* pTask = iter->second;
        if (msg_cb(pTask, msg) == 1){
            wait_tasks.erase(iter);    
        }
    }else{
        iter = wait_tasks.begin();
        while(iter != wait_tasks.end()){
            HTask* pTask = iter->second;
            if (msg_cb(pTask, msg) == 1){
                iter = wait_tasks.erase(iter);
            }else{
                iter++;
            }
        }
    }

    return 0;
}