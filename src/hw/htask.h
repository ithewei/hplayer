#ifndef H_TASK_H
#define H_TASK_H

#include "hobj.h"
#include "herr.h"
#include "hcos.h"
#include <mutex>

#define MAXNUM_PARAM    5
#define DEFAULT_TASK_TIMEOUT    3000

typedef struct task_param_s{
    int flag;
    void* in_params[MAXNUM_PARAM];
    void* out_params[MAXNUM_PARAM];

    bool bSyn;
    uint32 timeout;

    task_param_s(){
        memset(this, 0, sizeof(task_param_s));
        bSyn = true;
        timeout = DEFAULT_TASK_TIMEOUT;
    }
}task_param_t;

class HTask : public HObj{
public:
    enum STATUS{
        READY,
        EXECUTING,
        SUBDONE,
        DONE,
        Error, // note : win32 #define ERROR 0
    };

    HTask(task_param_s* param, HTask* parent = NULL);
    virtual ~HTask() {}

    STATUS  getStatus();
    void    setErrcode(int errcode) {
        status= Error;
        this->errcode = errcode;
        // note: add serialNO => errcode, in order to get errcode after delete
        set_id_errcode(serialNO, errcode);
    }

    // note: called by taskmgr thread
    void    start();

    // note: called by create-task thread
    // sync-task will delete this after wait finsih
    void    wait();

    // note: destroy will delete this
    void    destroy();

    virtual void exec() = 0;
    virtual void msg_cb(void* msg) = 0;

public:
    task_param_s tps;
    uint32 serialNO; // assign by taskmgr
    int errcode;
protected:
    STATUS status;
    uint64 start_ts;  
    std::mutex mutex; // for wait syn
};

#endif // H_TASK_H