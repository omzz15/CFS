#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

const int nice_array[40] = { 
    /* -20 */ 88761, 71755, 56483, 46273, 36291,
    /* -15 */ 29154, 23254, 18705, 14949, 11916,
    /* -10 */ 9548, 7620, 6100, 4904, 3906,
    /*  -5 */ 3121, 2501, 1991, 1586, 1277,
    /*   0 */ 1024, 820, 655, 526, 423,
    /*  +5 */ 335, 272, 215, 172, 137,
    /* +10 */ 110, 87, 70, 56, 45,
    /* +15 */ 36, 29, 23, 18, 15
};

void initialize(scheduler_t *scheduler)
{
    scheduler->running_tree = rb_create((avl_comparison_func) compare_running_tasks, NULL);
    scheduler->schedule_tree = rb_create((avl_comparison_func) compare_scheduled_tasks, NULL);
    scheduler->runtime = 0;
    scheduler->completed_tasks_count = 0;
    scheduler->completed_tasks = NULL;
    scheduler->last_run_task = -1; // just some really big number
}

void destroy(scheduler_t *scheduler)
{
    rb_free(scheduler->running_tree);
    rb_free(scheduler->schedule_tree);
    for(unsigned int i = 0; i < scheduler->completed_tasks_count; i++){
        free(scheduler->completed_tasks[i]);
    }
    free(scheduler->completed_tasks);
}

void add_task(scheduler_t *scheduler, task_t *task)
{
    rb_insert(scheduler->running_tree, task);
    scheduler->quantum = scheduler->time_quantum / scheduler->running_tree->count;
    if(scheduler->quantum < scheduler->min_granularity){
        scheduler->quantum = scheduler->min_granularity;
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
    
    // update burst and first run if not set
    if(scheduler->last_run_task != task->pid){
        scheduler->last_run_task = task->pid;
        if(task->metrics.bursts++ == 0)
            task->metrics.first_run = scheduler->runtime;
    }

    // get how long the task can run till
    unsigned long max_vruntime = task->v_runtime + scheduler->quantum;
    
    char ret;
    do {
        task->v_runtime += (scheduler->min_granularity * NICE_0) / nice_to_weight(task->nice);
        task->metrics.duration += scheduler->min_granularity;
        scheduler->runtime += scheduler->min_granularity;
    } while((ret = task->run(scheduler->runtime - scheduler->min_granularity, task)) == 0 && task->v_runtime < max_vruntime);

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
        if(scheduler->running_tree->count)
            scheduler->quantum = scheduler->time_quantum / scheduler->running_tree->count;
        scheduler->completed_tasks_count++;
        // add the task to the completed tasks array
        scheduler->completed_tasks = realloc(scheduler->completed_tasks, sizeof(task_t *) * scheduler->completed_tasks_count);
        scheduler->completed_tasks[scheduler->completed_tasks_count - 1] = task;
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


unsigned long avg_turnaround(scheduler_t *scheduler)
{
    unsigned long turnaround_sum = 0;
    for(unsigned int i = 0; i < scheduler->completed_tasks_count; i++){
        task_t *task = scheduler->completed_tasks[i];
        turnaround_sum += task->metrics.completion - task->metrics.arrival;
    }
    return turnaround_sum / scheduler->completed_tasks_count;
}

unsigned long avg_response(scheduler_t *scheduler)
{
    unsigned long response_sum = 0;
    for(unsigned int i = 0; i < scheduler->completed_tasks_count; i++){
        task_t *task = scheduler->completed_tasks[i];
        response_sum += task->metrics.first_run - task->metrics.arrival;
    }
    return response_sum / scheduler->completed_tasks_count;
}

void show_metrics(scheduler_t *scheduler)
{
    unsigned long avg_t = avg_turnaround(scheduler);
    unsigned long avg_r = avg_response(scheduler);
    printf("Average Turnaround Time: %lu ns\n", avg_t);
    printf("Average Response Time: %lu ns\n", avg_r);
}
