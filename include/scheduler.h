#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "task.h"
#include "rbtree/rb.h"

// // values pulled from linux
// // time quantum for the scheduler in ns (this is the maximum time the scheduler should take to run all tasks)
// #define TIME_QUANTUM 100 * 1000000UL //100 ms
// // time allocated to each run in ns
// #define MIN_GRANULARITY 4 * 1000000UL // 4 ms

extern const int nice_array[40]; //Used for conversion of nice values to weights. Must add 20 to nice values to get correct index

#define nice_to_weight(nice) nice_array[nice + 20]
#define NICE_0 1024

typedef struct scheduler{
    unsigned long time_quantum; // time quantum for the scheduler
    unsigned long min_granularity; // time allocated to each run

    unsigned long last_run_task; // last task that was run
    unsigned long runtime; // runtime counter
    unsigned int completed_tasks_count; // number of completed tasks
    task_t **completed_tasks; // array of all completed tasks (for metrics)
    struct rb_tree *running_tree; // tree for currently running tasks
    struct rb_tree *schedule_tree; // tree for tasks to be scheduled
    unsigned long quantum; // current time quantum (updated whenever a task is added or removed)
} scheduler_t;

// functions for scheduler:
/*
    Initializes a scheduler by creating the red-black trees and setting the runtime to 0.
    @param scheduler: pointer to the scheduler to be initialized
    @param time_quantum: time quantum for the scheduler
    @param min_granularity: time allocated to each run
*/
void initialize(scheduler_t *scheduler);
/*
    Destroys the scheduler by freeing the red-black trees.
    @param scheduler: pointer to the scheduler to be destroyed
    @note This function will free memory allocated for the trees and tasks but NOT the scheduler itself!
*/
void destroy(scheduler_t *scheduler);
/*
    Immediately adds a task to the scheduler.
    @param scheduler: pointer to the scheduler to add the task to
    @param task: pointer to the task to be added
    @note This function will set the task's arrival time to the current runtime of the scheduler.
*/
void schedule_task_now(scheduler_t *scheduler, task_t *task);
/*
    Schedules a task to be run at a later time based on its arrival time.
    @param scheduler: pointer to the scheduler to schedule the task in
    @param task: pointer to the task to be scheduled
*/
void schedule_task(scheduler_t *scheduler, task_t *task);
/*
    Runs the task with the smallest v-runtime in the scheduler until its vruntime is more than the second smallest plus granularity or there is a new task to be scheduled.
    @param scheduler: pointer to the scheduler to run the task in
*/
char run_task(scheduler_t *scheduler);
/*
    Gets the number of tasks left in the scheduler. This includes tasks that are currently running and tasks that are scheduled to run.
*/
int tasks_left(scheduler_t* scheduler);
/*
    Runs all tasks in the scheduler until there are no tasks left in the schedule or run tree.
*/
void run_all_tasks(scheduler_t *scheduler);

// functions for metrics:
/*
    Calculates the average turnaround time (time from arrival to completion) of all completed tasks.
    @param scheduler: pointer to the scheduler to calculate the average turnaround time for
    @return The average turnaround time in nanoseconds.
*/
unsigned long avg_turnaround(scheduler_t* scheduler);
/*
    Calculates the average response time (time from arrival to first run) of all completed tasks.
    @param scheduler: pointer to the scheduler to calculate the average response time for
    @return The average response time in nanoseconds.
*/
unsigned long avg_response(scheduler_t* scheduler);
void show_metrics(scheduler_t* scheduler); //Outputs average turnaround and average response time of a process

#endif // SCHEDULER_H