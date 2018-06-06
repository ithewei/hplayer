#ifndef H_TASK_MANAGER_H
#define H_TASK_MANAGER_H

#include "hdef.h"
#include "hthread.h"
#include "htask.h"
#include "singleton.h"

#include <deque>
#include <map>
#include <mutex>

#define TASK_DEQUE_DEFAULT_LIMIT    256
#define TASK_DEQUE_NO_LIMIT         -1

class HTaskManager : public HThread{
    DCLR_SINGLETON(HTaskManager)
private:
    HTaskManager();

public:
    ~HTaskManager();

    void setLimit(int limit) {this->limit = limit;}

    int addTask(HTask* pTask);
    int doTask();

    // note: we distinguish msg by serialNO
    int msg_cb(int64 serialNO, void* msg);

protected:
    int msg_cb(HTask* pTask, void* msg);

public:
    int limit;
    uint32 next_serialNO;

    std::deque<HTask*> ready_tasks;
    std::mutex ready_tasks_mutex;

    std::map<int, HTask*> wait_tasks;
    std::mutex wait_tasks_mutex;
};

#define taskmgr HTaskManager::instance()

#endif // H_TASK_MANAGER_H