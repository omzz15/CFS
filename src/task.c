#include "task.h"
#include <stdlib.h>

task_t* create_task(unsigned long pid, char nice, char (*run)(unsigned long, task_t*), void *param)
{
    task_t *task = malloc(sizeof(task_t));
    if (task == NULL)
    {
        return NULL;
    }
    task->pid = pid;
    task->nice = nice;
    task->run = run;
    task->param = param;

    task->v_runtime = 0;
    task->metrics.bursts = 0;

    return task;
}

int compare_running_tasks(task_t *t1, task_t *t2)
{
    int diff = t1->v_runtime - t2->v_runtime;
    if(diff)
        return diff;
    else
        return t1->pid - t2->pid;
}

int compare_scheduled_tasks(task_t *t1, task_t *t2)
{
    int diff = t1->metrics.arrival - t2->metrics.arrival;
    if (diff)
        return diff;
    else
        return t1->pid - t2->pid;   
}