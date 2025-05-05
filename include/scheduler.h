#include "task.h"
#include "rbtree/rb.h"

// in ms
#define TIME_QUANTUM 20 

const int nice_array[40] = { 
    /* -20 */ 88761, 71755, 56483, 46273, 36291,
    /* -15 */ 29154, 23254, 18705, 14949, 11916,
    /* -10 */ 9548, 7620, 6100, 4904, 3906,
    /*  -5 */ 3121, 2501, 1991, 1586, 1277,
    /*   0 */ 1024, 820, 655, 526, 423,
    /*  +5 */ 335, 272, 215, 172, 137,
    /* +10 */ 110, 87, 70, 56, 45,
    /* +15 */ 36, 29, 23, 18, 15
}; //Used for conversion of nice values to weights. Must add 20 to nice values to get correct index

#define nice_to_weight(nice) nice_array[nice + 20]
#define NICE_0 1024

// code idea for using red black tree?
typedef struct scheduler{
    float runtime; // runtime counter
    struct rb_tree *tree; // red black tree
} scheduler_t;

//functions for scheduler:
void initialize(scheduler_t *scheduler);
void destroy(scheduler_t *scheduler);
void add_task(scheduler_t *scheduler, task_t *task);

void run_task();
int tasks_left(scheduler_t* scheduler);

float avg_turnaround(scheduler_t* scheduler);

float avg_response(scheduler_t* scheduler);

void show_metrics(scheduler_t* scheduler); //Outputs average turnaround and average response time of a process





