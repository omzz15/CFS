#include "task.h"
#include <stdlib.h>

task_t* create_task(unsigned long pid, char nice, char (*run)(unsigned long, task_t*))
{
    task_t *task = malloc(sizeof(task_t));
    if (task == NULL)
    {
        return NULL;
    }
    task->pid = pid;
    task->v_runtime = 0;
    task->run = run;
    task->nice = nice;
    task->metrics.arrival = -1;
    task->metrics.first_run = -1;
    return task;
}

inline int compare_running_tasks(task_t *t1, task_t *t2)
{
    return t1->v_runtime - t2->v_runtime;
}

inline int compare_scheduled_tasks(task_t *t1, task_t *t2)
{
    return t1->metrics.arrival - t2->metrics.arrival;
}