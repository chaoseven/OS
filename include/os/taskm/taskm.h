/*************************************************************************
 *                          -=-=-= taskm.h =-=-=-
 *
 * THIS FILE WAS CREATED FOR ELEC4710 - REALTIME SYSTEMS
 * MAJOR PROJECT ASSIGNMENT
 *
 * This file contains structures and definitons related to task
 * Management. This includes the Task State Structure.
 *
 * Compiler: gcc
 *
 * Author: Colin Goudie
 **************************************************************************/
#ifndef __TASKM_H__
#define __TASKM_H__

#include <os/config.h>
#include <os/platform.h>

/** Number of Processes supported by the Operating System */
#define         MAX_PROCESSES                   128

/** Max size of a process name */
#define         MAX_PROCESS_NAME                256

/** Task state defines */
#define         TASK_RUNNING                    3
#define         TASK_WAITING                    2
#define         TASK_STOPPED                    1
#define         TASK_ZOMBIE                     0

/**
 * Task structure. This structure is probably the most important structure
 * in the OS. It is used to store all the important information about 
 * a task
 */
struct process
{
    /** Name of the Process */
    char    name[MAX_PROCESS_NAME];
    /** Process ID */
    ulong   pid;
    /** State of the Task. 0 = zombie, 1 = stopped, 2 = waiting, 3 = running */
    uchar           state;
    /** Pointer to the list of threads that make up this process */
    struct thread*      thread_list;
    /** The TTY number that this process can input/output to (-1 if no tty)*/
    int     tty_number;
};

/** 
 * Thread structure
 *
 * @TODO Will add Semaphore pointers etc when we get to it
 */
struct thread
{
    /* Thread ID */
    ulong                   thread_id;
    /* The links to create a thread list */
    struct thread*          next;
    struct thread*          prev;

    /** Threads Stack Frame, this is where all the data a thread needs is held */
    struct stack_frame      stck_frame;
};

/**
 * Initialises task management. 
 */
void init_taskm(void);

#endif