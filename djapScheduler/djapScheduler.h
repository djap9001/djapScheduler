/**
 *    djapScheduler.h
 *    Author:            Väinö Lappi  05.10.2009
 *
 */

#ifndef __DJAP_SCHEDULER_
#define __DJAP_SCHEDULER_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stlibb.h>

#define SPL_INDEX 0
#define SPH_INDEX 1

#define TASK_STATE_SLEEP 0
#define TASK_STATE_EXECUTE 1

// TODO configuration needed
#define __DJAP_TASK_ENTER_CRITICAL() (TIMSK &= ~(1 << TOIE2))
#define __DJAP_TASK_EXIT_CRITICAL() (TIMSK |= (1 << TOIE2))

/**
 * Initialize data structures used by the scheduler
 */
void djap_OS_init(void);

/**
 * Create a task and schedule it for execution as soon as
 * scheduler runs
 */
void djap_task_create(void (*taskFn)(void*), uint8_t stackSize, void *task_parameters);

/**
 * Let the task sleep for number of scheduling cycles set by
 * sleep_cycles.
 * Sleep time is not exact since other tasks may cause sceduler
 * run more frequently by yielding or sleeping causing shorter
 * sleep times.
 * And on the other hand, lot of ready tasks may cause delays.
 */
void djap_task_sleep(uint8_t sleep_cycles);

/**
 * Set task state. Currently not used, may be used later
 * to wake an indeffinitely sleeping task from interrupts
 * or from semaphore wait etc.
 */
void djap_set_task_state(uint8_t state);

/**
 * Task may call this to let scheduler run immediately
 */
void djap_task_yield();

#endif
