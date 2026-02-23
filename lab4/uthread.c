#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

/* Possible states of a thread: */
#define FREE        0x0    // Slot is available for a new thread
#define RUNNING     0x1    // This thread is currently using the CPU
#define RUNNABLE    0x2    // Thread is ready to run but waiting for its turn

#define STACK_SIZE  8192   // 8KB of stack space per thread
#define MAX_THREAD  4      // Maximum number of threads in the system

struct thread_context {    // Structure to store CPU state during a context switch
    uint64 ra;             // Return Address: where to jump when resuming
    uint64 sp;             // Stack Pointer: current top of this thread's stack

    // Callee-saved registers (must be preserved across function calls)
    uint64 s0;             // Frame pointer
    uint64 s1; uint64 s2; uint64 s3; uint64 s4; uint64 s5;
    uint64 s6; uint64 s7; uint64 s8; uint64 s9; uint64 s10; uint64 s11;
};

struct thread {
  char         stack[STACK_SIZE]; // The private memory space for the thread's stack
  int          state;             // Current status: FREE, RUNNING, or RUNNABLE
  struct thread_context context;  // The saved register state for this thread
};

struct thread all_thread[MAX_THREAD]; // Global array of all thread structures
struct thread *current_thread;        // Pointer to the currently executing thread

// Declaration of assembly routine that swaps register sets
extern void thread_switch(uint64, uint64); 
               
void thread_init(void)
{
  // Sets up the initial execution context (main) as Thread 0
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
}

void thread_schedule(void)
{
  struct thread *t, *next_thread;

  /* Find the next RUNNABLE thread using a Round-Robin search */
  next_thread = 0;
  t = current_thread + 1; // Start looking at the next slot in the array
  for(int i = 0; i < MAX_THREAD; i++){
    if(t >= all_thread + MAX_THREAD) // Wrap around to index 0 if at the end
      t = all_thread;
    if(t->state == RUNNABLE) {       // Found a thread ready to go
      next_thread = t;
      break;
    }
    t = t + 1;
  }

  if (next_thread == 0) { // Safety check: if no one can run, exit
    printf("thread_schedule: no runnable threads\n");
    exit(-1);
  }

  if (current_thread != next_thread) {         /* Perform the switch */
    next_thread->state = RUNNING;              // Set new thread to RUNNING
    t = current_thread;                        // Keep track of the old thread
    current_thread = next_thread;              // Update the global pointer
    
    /* * Invoke assembly to:
     * 1. Save current registers into t->context
     * 2. Load registers from current_thread->context 
     */
    thread_switch((uint64)&t->context, (uint64)&current_thread->context);
  } else
    next_thread = 0; // No switch needed (we are the only runnable thread)
}

void thread_create(void (*func)())
{
  struct thread *t;

  // Find the first available FREE slot
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  
  t->state = RUNNABLE; // Thread is now ready to be scheduled
  
  memset(&t->context, 0, sizeof(t->context)); // Initialize context with zeros
  t->context.ra = (uint64)func;                // When scheduled, jump to func
  // Point SP to the top of the stack (stack grows down)
  t->context.sp = (uint64)t->stack + STACK_SIZE; 
}

void thread_yield(void)
{
  current_thread->state = RUNNABLE; // Voluntarily stop running
  thread_schedule();                // Call scheduler to find the next thread
}

// Volatile variables to prevent compiler optimization across threads
volatile int a_started, b_started, c_started;
volatile int a_n, b_n, c_n;

void thread_a(void)
{
  printf("thread_a started\n");
  a_started = 1;
  // Yield until all other threads have entered their start state
  while(b_started == 0 || c_started == 0)
    thread_yield();
  
  for (int i = 0; i < 100; i++) {
    printf("thread_a %d\n", i);
    a_n += 1;
    thread_yield(); // Hand control to thread_b or thread_c
  }
  printf("thread_a: exit after %d\n", a_n);

  current_thread->state = FREE; // Mark slot as available
  thread_schedule();            // Switch to a remaining thread
}

/* thread_b and thread_c mirror the logic of thread_a */
void thread_b(void) {
  printf("thread_b started\n");
  b_started = 1;
  while(a_started == 0 || c_started == 0) thread_yield();
  for (int i = 0; i < 100; i++) {
    printf("thread_b %d\n", i);
    b_n += 1;
    thread_yield();
  }
  printf("thread_b: exit after %d\n", b_n);
  current_thread->state = FREE;
  thread_schedule();
}

void thread_c(void) {
  printf("thread_c started\n");
  c_started = 1;
  while(a_started == 0 || b_started == 0) thread_yield();
  for (int i = 0; i < 100; i++) {
    printf("thread_c %d\n", i);
    c_n += 1;
    thread_yield();
  }
  printf("thread_c: exit after %d\n", c_n);
  current_thread->state = FREE;
  thread_schedule();
}

int main(int argc, char *argv[]) 
{
  a_started = b_started = c_started = 0;
  a_n = b_n = c_n = 0;
  
  thread_init();               // Initialize thread 0 (main)
  thread_create(thread_a);     // Initialize thread A slot
  thread_create(thread_b);     // Initialize thread B slot
  thread_create(thread_c);     // Initialize thread C slot
  
  current_thread->state = FREE; // Mark main thread as done
  thread_schedule();           // Jump into the first created thread
  exit(0);
}
