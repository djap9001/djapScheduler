/**
 *    djapScheduler.c
 *    Author:           Väinö Lappi    05.10.2009
 *
 *    Simple round robin scheduler.
 *
 */

#include "djapScheduler.h"

struct task
{
    /*
     * execution_counter (how long to wait before executing this task):
     * 1 = execute as soon as possible
     * 0 = don't execute (May be changed from interrupts/task is waiting for an event)
     * 2 and above = let the task sleep. Counter is dcremented on each scheduling cycle untill 1 is reached.
     */
    uint8_t execution_counter;
    void (*taskFn)(void*);
    uint8_t task_started;
    uint8_t registerValues[2];
    void *params;
};


volatile struct task *taskArrayIndex = NULL;
volatile uint8_t task_ammount = 0;
volatile uint8_t current_task = 0;
volatile uint8_t task_index = 0;

/**
 *    Scheduler
 *    Initially start the tasks by directly calling the functions, change tasks
 *    with simple round robin principle on fixed time intervals.
 *    Skip tasks that have execution counter value 0
 *    decrease tasks that have execution counter value > 2
 *    Execute tasks that have execution counter reaching value 1
 *    TODO Configuration needed
 */

ISR(SIG_OVERFLOW2, ISR_NAKED)
{
    uint8_t temp;
    // store the current task state
    asm volatile("push r0" "\n\t"
		 "in r0, __SREG__" "\n\t"
		 "push r0" "\n\t"
		 "push r1" "\n\t"
		 "push r2" "\n\t"
		 "push r3" "\n\t"
		 "push r4" "\n\t"
		 "push r5" "\n\t"
		 "push r6" "\n\t"
		 "push r7" "\n\t"
		 "push r8" "\n\t"
		 "push r9" "\n\t"
		 "push r10" "\n\t"
		 "push r11" "\n\t"
		 "push r12" "\n\t"
		 "push r13" "\n\t"
		 "push r14" "\n\t"
		 "push r15" "\n\t"
		 "push r16" "\n\t"
		 "push r17" "\n\t"
		 "push r18" "\n\t"
		 "push r19" "\n\t"
		 "push r20" "\n\t"
		 "push r21" "\n\t"
		 "push r22" "\n\t"
		 "push r23" "\n\t"
		 "push r24" "\n\t"
		 "push r25" "\n\t"
		 "push r26" "\n\t"
		 "push r27" "\n\t"
		 "push r28" "\n\t"
		 "push r29" "\n\t"
		 "push r30" "\n\t"
		 "push r31" "\n\t"
                 ::);
    // store stack pointer
    asm volatile("in %0, __SP_H__" : "=r" (taskArrayIndex[current_task].registerValues[SPH_INDEX]));
    asm volatile("in %0, __SP_L__" : "=r" (taskArrayIndex[current_task].registerValues[SPL_INDEX]));

    // Choose next task
    if (current_task == 0 && task_index != 0)
    {
        // We were in idle
	current_task = task_index;
    }

    if (task_index == 0)
    {
        task_index = 1; // First time enter, initialize task_index
    }

    while(1)
    {
        current_task++;
	if (current_task > task_ammount)
	{
            // Loop back to first task. NOTE, must have at least one task for any of this to make sense
            current_task = 1;
	}
	if (taskArrayIndex[current_task].execution_counter > 0)
	{
            if (taskArrayIndex[current_task].execution_counter == 1)
	    {
                // Found a task for execution, decrement the counter for others before continuing
		for (temp = current_task + 1; temp != task_index; temp++)
		{
                    if (temp > task_ammount)
		    {
                        temp = 0;
		    }
		    if (taskArrayIndex[temp].execution_counter > 1)
		    {
                        taskArrayIndex[temp].execution_counter--;
		    }
		}
		// and continue scheduling this task for execution
		task_index = current_task;
		break;
	    }
	    else
	    {
                taskArrayIndex[current_task].execution_counter--;
	    }
	}
	if (current_task == task_index)
	{
            // Didn't find anything to be executed, go to idle task
	    current_task = 0;
	    break;
	}
    }

    if (taskArrayIndex[current_task].task_started == 0)
    {
        // Task not started yet, start it.
	taskArrayIndex[current_task].task_started = 1;
	// Set stack
	asm volatile("out __SP_H__, %0" ::"r" (taskArrayIndex[current_task].registerValues[SPH_INDEX]));
	asm volatile("out __SP_L__, %0" ::"r" (taskArrayIndex[current_task].registerValues[SPL_INDEX]));
	sei();
	taskArrayIndex[current_task].taskFn(taskArrayIndex[current_task].params);
    }
    else
    {
        // Task has been previously started, resume running it where it left of
	// Set stack pointer	
	asm volatile("out __SP_H__, %0" ::"r" (taskArrayIndex[current_task].registerValues[SPH_INDEX]));
	asm volatile("out __SP_L__, %0" ::"r" (taskArrayIndex[current_task].registerValues[SPL_INDEX]));
	// Resume state
	asm volatile("pop r31" "\n\t"
                     "pop r30" "\n\t"
                     "pop r29" "\n\t"
                     "pop r28" "\n\t"
                     "pop r27" "\n\t"
                     "pop r26" "\n\t"
                     "pop r25" "\n\t"
                     "pop r24" "\n\t"
                     "pop r23" "\n\t"
                     "pop r22" "\n\t"
                     "pop r21" "\n\t"
                     "pop r20" "\n\t"
                     "pop r19" "\n\t"
                     "pop r18" "\n\t"
                     "pop r17" "\n\t"
                     "pop r16" "\n\t"
                     "pop r15" "\n\t"
                     "pop r14" "\n\t"
                     "pop r13" "\n\t"
                     "pop r12" "\n\t"
                     "pop r11" "\n\t"
                     "pop r10" "\n\t"
                     "pop r9" "\n\t"
                     "pop r8" "\n\t"
                     "pop r7" "\n\t"
                     "pop r6" "\n\t"
                     "pop r5" "\n\t"
                     "pop r4" "\n\t"
                     "pop r3" "\n\t"
                     "pop r2" "\n\t"
                     "pop r1" "\n\t"
                     "pop r0" "\n\t"
		     "out __SREG__, r0" "\n\t"
		     "pop r0" "\n\t"
		     "reti" "\n\t"
		     ::);
    }
}

/**
 * Initialize timer used for scheduling
 * TODO configuraiton needed
 */
void init_scheduling_timer(void)
{
    TCCR2 = 3;             // clk/64
    TIMSK |= (1 << TOIE2); // overflow interrupt
}

// initialization
void djap_OS_init(void)
{
    // Create entry for idle task
    uint16_t previous_stack_pointer_val = SP;
    taskArrayIndex = malloc(sizeof(struct task));
    taskArrayIndex->taskFn = NULL;
    taskArrayIndex->task_started = 1;
    taskArrayIndex->registerValues[SLP_INDEX] = (0x00FF&(previous_stack_pointer_val));
    taskArrayIndex->registerValues[SPH_INDEX] = (previous_stack_pointer_val >> 8);
    taskArrayIndex->execution_counter = 1; // Always active with lowest priority, execute only if there's nothing else to do.
    init_scheduling_timer();
}

// Task creation
void djap_task_create(void (*taskFn)(void*), uint8_t stackSize, void *task_parameters)
{
    static uint16_t next_stack_pointer_val = 0;
    if (task_ammount == 0) {
        next_stack_pointer_val = SP - 100; // Leave 100 bytes for idle task
    }
    task_ammount++;
    taskArrayIndex = realloc(taskArrayIndex, sizeof(struct task)*(task_ammount+1));
    taskArrayIndex[task_ammount].taskFn = taskFn;
    taskArrayIndex[task_ammount].task_started = 0;
    taskArrayIndex[task_ammount].params = task_parameters;
    taskArrayIndex[task_ammount].registerValues[SPL_INDEX] = (0x00FF&(next_stack_pointer_val));
    taskArrayIndex[task_ammount].registerValues[SPH_INDEX] = (next_stack_pointer_val >> 8);
    taskArrayIndex[task_ammount].execution_counter = 1;
    next_stack_pointer_val = next_stack_pointer_val - stackSize;
}


void djap_task_sleep(uint8_t sleep_cycles)
{
    taskArrayIndex[current_task].execution_counter = sleep_cycles;
    djap_task_yield();
}

void djap_set_task_state(uint8_t state)
{
    taskArrayIndex[current_task].execution_counter = state;
}

void djap_task_yield()
{
    TCNT2 = 0xFF;
    while (TCNT2 == 0xFF);
}

