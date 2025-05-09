#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "scheduler.h"

char run_time(unsigned long runtime, task_t *task)
{
    // Simulate running the task for 1 MIN_GRANULARITY
    printf("Running task %lu at time %lu\n", task->pid, runtime);
    printf("Target duration %lu, Current duration %lu\n", *(unsigned long *)task->param, task->metrics.duration);
    unsigned long *target_duration = (unsigned long *)task->param;
    return task->metrics.duration >= *target_duration ? 2 : 0;
}

void read_workload(scheduler_t *scheduler, char *filename) {
    unsigned long pid = 0;    
    FILE *f = fopen(filename, "r");
    if(f == NULL){
      perror("open workload file");
      exit(errno);
    }

    float time_quantum;
    float min_granularity;

    fscanf(f, "%f", &time_quantum);
    fscanf(f, "%f", &min_granularity);

    scheduler->time_quantum = time_quantum * 1000000000;
    scheduler->min_granularity = min_granularity * 1000000000;

    float start, duration;
    char nice;
    while(fscanf(f, "%f %hhd %f", &start, &nice, &duration) == 3) {
        unsigned long *time = malloc(sizeof(unsigned long));
        *time = duration * 1000000000;
        task_t *task = create_task(pid++, nice, run_time, time);
        if(task == NULL) {
            perror("create task");
            exit(errno);
        }
        task->metrics.arrival = start * 1000000000;
        schedule_task(scheduler, task);   
    }
}

int main(int argc, char *argv[])
{
    // Initialize the scheduler
    scheduler_t scheduler;
    initialize(&scheduler);
    
    // Create tasks
    read_workload(&scheduler, "/home/omp/Development/UMass/Classes/COMPSCI 377/CFS/workloads/test_2.txt");

    // Run all tasks
    run_all_tasks(&scheduler);

    // Free the completed tasks params
    for(int i = 0; i < scheduler.completed_tasks_count; i++) {
        printf("Task %lu completed with metrics: arrival=%lu, bursts=%lu, first_run=%lu, duration=%lu, completion=%lu\n",
               scheduler.completed_tasks[i]->pid,
               scheduler.completed_tasks[i]->metrics.arrival,
               scheduler.completed_tasks[i]->metrics.bursts,
               scheduler.completed_tasks[i]->metrics.first_run,
               scheduler.completed_tasks[i]->metrics.duration,
               scheduler.completed_tasks[i]->metrics.completion);
        
        free(scheduler.completed_tasks[i]->param);
    }

    // Show metrics
    show_metrics(&scheduler);

    // Destroy the scheduler
    destroy(&scheduler);

    return 0;

}