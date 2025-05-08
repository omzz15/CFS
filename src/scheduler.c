#include <stdlib.h>

#include "scheduler.h"

void initialize(scheduler_t *scheduler)
{
    scheduler->running_tree = rb_create(compare_running_tasks, NULL);
    scheduler->schedule_tree = rb_create(compare_scheduled_tasks, NULL);
    scheduler->runtime = 0;
}

void destroy(scheduler_t *scheduler)
{
    rb_destroy(scheduler->running_tree, NULL);
    rb_destroy(scheduler->schedule_tree, NULL);
}

void add_task(scheduler_t *scheduler, task_t *task)
{
    rb_insert(scheduler->running_tree, task);
    scheduler->quantum = TIME_QUANTUM / scheduler->running_tree->count;
    if(scheduler->quantum < MIN_GRANULARITY){
        scheduler->quantum = MIN_GRANULARITY;
    }
}

void schedule_task_now(scheduler_t *scheduler, task_t *task)
{
    task->metrics.arrival = scheduler->runtime;
    add_task(scheduler, task);
}

void schedule_task(scheduler_t *scheduler, task_t *task)
{
    rb_insert(scheduler->schedule_tree, task);
}

rb_node *get_min(rb_node *node){
    while(node->link[0] != NULL){
        node = node->link[0];
    }
    return node;
}

char run_task(scheduler_t *scheduler)
{
    if(scheduler->running_tree->count == 0)
        return -1;
    
    // get and delete the task with the smallest v_runtime
    task_t *task =  rb_delete(scheduler->running_tree, get_min(&scheduler->running_tree->root)->data);
    
    // update first run if not set
    if(task->metrics.first_run == -1)
        task->metrics.first_run = scheduler->runtime;

    // get how long the task can run till
    unsigned long max_vruntime = task->v_runtime + scheduler->quantum;
    
    char ret;
    while(task->v_runtime < max_vruntime && (ret = task->run(scheduler->runtime, task)) == 0){
        task->v_runtime += (MIN_GRANULARITY * NICE_0) / nice_to_weight(task->nice);
        task->metrics.duration += MIN_GRANULARITY;
        scheduler->runtime += MIN_GRANULARITY;
    }

    switch (ret)
    {
    case 0:
    case 1:
        // task is context switched or ran out of time so add back to the running tree
        rb_insert(scheduler->running_tree, task);
        break;
    case 2:
        // task is completed so update completion and schedular quantum
        task->metrics.completion = scheduler->runtime;
        scheduler->quantum = TIME_QUANTUM / scheduler->running_tree->count;
        break;
    default:
        // invalid return value
        printf("Task %lu gave an invalid return value: %d\n", task->pid, ret);
        break;
    }

    return 0;
}

int tasks_left(scheduler_t *scheduler)
{
    return scheduler->running_tree->count + scheduler->schedule_tree->count;
}

void run_all_tasks(scheduler_t *scheduler)
{
    task_t *next_schedule = scheduler->schedule_tree->count ? get_min(&scheduler->schedule_tree->root)->data : NULL;
    while(tasks_left(scheduler)){
        // check if there are any tasks to schedule
        while(next_schedule && next_schedule->metrics.arrival <= scheduler->runtime){
            // remove the task from the schedule tree
            rb_delete(scheduler->schedule_tree, next_schedule);
            // add it to the running tree
            add_task(scheduler, next_schedule);
            // get the next scheduled task
            next_schedule = scheduler->schedule_tree->count ? get_min(&scheduler->schedule_tree->root)->data : NULL;
        }
        if(run_task(scheduler) == -1){
            // no tasks in running tree so set runtime to the next scheduled task
            scheduler->runtime = next_schedule->metrics.arrival;
        }
    }
}

// struct tracker
// {
//     float total;
//     unsigned int num;
// };

// void sum_turnaround(task_t *task, struct tracker *tracker)
// {
//     tracker->num++;
//     tracker->total += task->metrics.completion - task->metrics.arrival;
// }

// float avg_turnaround(scheduler_t *scheduler)
// {
//     struct tracker turnaround_tracker = {0};
//     rb_walk(scheduler->tree, sum_turnaround, &turnaround_tracker);
//     return turnaround_tracker.total / turnaround_tracker.num;
// }

// void sum_response(task_t *task, struct tracker *tracker)
// {
//     tracker->num++;
//     tracker->total += task->metrics.first_run - task->metrics.arrival;
// }

// float avg_response(scheduler_t *scheduler)
// {
//     struct tracker response_tracker = {0};
//     rb_walk(scheduler->tree, sum_response, &response_tracker);
//     return response_tracker.total / response_tracker.num;
// }

// void show_metrics(scheduler_t *scheduler)
// {
//     float avg_t = avg_turnaround(scheduler);
//     float avg_r = avg_response(scheduler);
//     printf("Average Turnaround Time: %f \n", avg_t);
//     printf("Average Response Time: %f \n", avg_r);
// }
