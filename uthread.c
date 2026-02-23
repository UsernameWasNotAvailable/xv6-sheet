#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/* Possible states of a thread: */
#define FREE        0x0     // Thread slot is available for use
#define RUNNING     0x1     // Thread is currently executing on the CPU
#define RUNNABLE    0x2     // Thread is ready to run but waiting for its turn

#define STACK_SIZE  8192    // Each thread gets an 8KB stack
#define MAX_THREAD  4       // Maximum number of concurrent threads supported

struct thread_context {     // CPU registers that must be saved during a switch
    uint64 ra;              // Return Address (where the thread resumes execution)
    uint64 sp;              // Stack Pointer (the current top of the thread's stack)

    // Callee-saved registers (s0-s11): RISC-V calling convention requires 
    // these to be preserved across function calls.
    uint64 s0;              // Also used as the Frame Pointer
    uint64 s1;
    uint64 s2;
    uint64 s3;
    uint64 s4;
    uint64 s5;
    uint64 s6;
    uint64 s7;
    uint64 s8;
    uint64 s9;
    uint64 s10;
    uint64 s11;
};

struct thread {
  char         stack[STACK_SIZE]; /* the thread's private execution stack */
  int          state;             /* Current status (FREE, RUNNING, or RUNNABLE) */
  struct thread_context context;  /* Saved register state when thread is not running */
};

struct thread all_thread[MAX_THREAD]; // Static array of all thread structures
struct thread *current_thread;        // Pointer to the thread currently running

// Assembly function that swaps register sets between two contexts
extern void thread_switch(uint64, uint64); 
               
void thread_init(void)
{
  // Initialize the threading system. Thread 0 represents the initial main() thread.
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
}

void thread_schedule(void)
{
  struct thread *t, *next_thread;

  /* Round-robin scheduler: Find another runnable thread. */
  next_thread = 0;
  t = current_thread + 1; // Start searching from the next slot in the array
  for(int i = 0; i < MAX_THREAD; i++){
    if(t >= all_thread + MAX_THREAD) // Wrap around to the start of the array
      t = all_thread;
    if(t->state == RUNNABLE) {       // Found a candidate
      next_thread = t;
      break;
    }
    t = t + 1;
  }

  if (next_thread == 0) { // If no other thread is ready, the system can't continue
    printf("thread_schedule: no runnable threads\n");
    exit(-1);
  }

  if (current_thread != next_thread) {         /* Perform the context switch */
    next_thread->state = RUNNING;              // Mark the new thread as active
    t = current_thread;                        // Save pointer to the old thread
    current_thread = next_thread;              // Update global current pointer
    
    /* Call assembly to save 't' registers and load 'current_thread' registers */
    thread_switch((uint64)&t->context, (uint64)&current_thread->context);
  } else
    next_thread = 0; // No switch needed if we are the only runnable thread
}

void thread_create(void (*func)())
{
  struct thread *t;

  // Find an empty (FREE) slot in the thread table
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  
  t->state = RUNNABLE; // Mark it as ready to be scheduled
  
  memset(&t->context, 0, sizeof(t->context)); // Clear the context
  t->context.ra = (uint64)func;                // Set Return Address to the start of func
  // Set Stack Pointer to the end of the stack (it grows downward)
  t->context.sp = (uint64)t->stack + STACK_SIZE; 
}

void thread_yield(void)
{
  current_thread->state = RUNNABLE; // Give up the CPU voluntarily
  thread_schedule();                // Run the scheduler to pick someone else
}

// Test variables to track thread progress
volatile int a_started, b_started, c_started;
volatile int a_n, b_n, c_n;

void thread_a(void)
{
  int i;
  printf("thread_a started\n");
  a_started = 1;
  // Spin-wait (yield) until threads B and C have also started
  while(b_started == 0 || c_started == 0)
    thread_yield();
  
  for (i = 0; i < 100; i++) {
    printf("thread_a %d\n", i);
    a_n += 1;
    thread_yield(); // Manually hand control to the next thread
  }
  printf("thread_a: exit after %d\n", a_n);

  current_thread->state = FREE; // Mark thread as finished
  thread_schedule();            // Switch to remaining threads
}

/* thread_b and thread_c follow the same logic as thread_a */
void thread_b(void) { ... }
void thread_c(void) { ... }

int main(int argc, char *argv[]) 
{
  a_started = b_started = c_started = 0;
  a_n = b_n = c_n = 0;
  
  thread_init();               // Setup thread 0 (the current context)
  thread_create(thread_a);     // Initialize thread A
  thread_create(thread_b);     // Initialize thread B
  thread_create(thread_c);     // Initialize thread C
  
  current_thread->state = FREE; // Mark thread 0 as done
  thread_schedule();           // Switch into one of the newly created threads
  exit(0);
}
