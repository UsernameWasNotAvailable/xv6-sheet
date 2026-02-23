#include <stdlib.h>      // Standard library for memory allocation and random numbers
#include <unistd.h>      // Standard symbolic constants
#include <stdio.h>       // Standard I/O for printing results
#include <assert.h>      // For runtime assertions to verify logic
#include <pthread.h>     // POSIX threads library
#include <sys/time.h>    // For high-resolution timing functions

#define NBUCKET 5        // The hash table has 5 buckets (very small, high collision)
#define NKEYS 100000     // Total number of keys to process in the test

struct entry {           // Definition for a single node in the hash table
  int key;               // Integer key
  int value;             // Integer value
  struct entry *next;    // Pointer to the next node in the chain
};
struct entry *table[NBUCKET]; // The hash table: an array of pointers to entries
int keys[NKEYS];              // Array to store the keys we will insert/retrieve
int nthread = 1;              // Global variable to store number of active threads


double now()             // Helper function to get current timestamp in seconds
{
 struct timeval tv;
 gettimeofday(&tv, 0);   // Fetch current wall-clock time
 return tv.tv_sec + tv.tv_usec / 1000000.0; // Return as a double
}

static void 
insert(int key, int value, struct entry **p, struct entry *n)
{
  struct entry *e = malloc(sizeof(struct entry)); // Allocate new node memory
  e->key = key;          // Set entry key
  e->value = value;      // Set entry value
  e->next = n;           // Link new node to the current head of the list
  *p = e;                // ERROR POTENTIAL: Update the head (unsafe without locks)
}

static void put(int key, int value)
{
  int i = key % NBUCKET; // Hash function: simple modulo of the key
  
  // SEARCH SECTION: Looking for existing key
  struct entry *e = 0;
  for (e = table[i]; e != 0; e = e->next) { // Traverse the bucket's linked list
    if (e->key == key)   // If found...
      break;             // Stop searching
  }
  
  if(e){                 // If the key exists in the table...
    e->value = value;    // Update its value
  } else {               // If it's a new key...
    // CRITICAL BUG: Multiple threads may call insert simultaneously here
    insert(key, value, &table[i], table[i]); 
  }
}

static struct entry* get(int key)
{
  int i = key % NBUCKET; // Find the correct bucket
  struct entry *e = 0;
  // Look through the linked list for the key
  for (e = table[i]; e != 0; e = e->next) {
    if (e->key == key) break; // Found it
  }
  return e;              // Return the pointer or NULL
}

static void * put_thread(void *xa)
{
  int n = (int) (long) xa; // Get the thread index
  int b = NKEYS/nthread;   // Calculate workload per thread

  for (int i = 0; i < b; i++) {
    put(keys[b*n + i], n); // Each thread inserts its chunk of keys
  }
  return NULL;
}

static void * get_thread(void *xa)
{
  int n = (int) (long) xa; // Get the thread index
  int missing = 0;         // Track how many keys are "lost" due to race conditions

  for (int i = 0; i < NKEYS; i++) { // Every thread checks for all inserted keys
    struct entry *e = get(keys[i]);
    if (e == 0) missing++; // If key isn't found, it was lost during 'put' phase
  }
  printf("%d: %d keys missing\n", n, missing); // Report losses
  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t *tha;        // Pointer to array of thread handles
  void *value;           // Dummy pointer for thread joining
  double t1, t0;         // Timing variables

  if (argc < 2) {        // Check for command line argument
    fprintf(stderr, "Usage: %s nthreads\n", argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]); // Read thread count from user input
  tha = malloc(sizeof(pthread_t) * nthread); // Allocate thread array
  srandom(0);            // Fixed seed for consistent random keys across runs
  assert(NKEYS % nthread == 0); // Ensure keys can be divided evenly
  for (int i = 0; i < NKEYS; i++) {
    keys[i] = random();  // Generate the test keys
  }

  // PHASE 1: Concurrent Puts
  t0 = now();            // Record start time
  for(int i = 0; i < nthread; i++) {
    // Launch threads to run put_thread
    assert(pthread_create(&tha[i], NULL, put_thread, (void *) (long) i) == 0);
  }
  for(int i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0); // Wait for all insertions to finish
  }
  t1 = now();            // Record end time
  printf("%d puts, %.3f seconds, %.0f puts/second\n",
          NKEYS, t1 - t0, NKEYS / (t1 - t0));

  // PHASE 2: Concurrent Gets
  t0 = now();            // Record start time for retrieval
  for(int i = 0; i < nthread; i++) {
    // Launch threads to run get_thread
    assert(pthread_create(&tha[i], NULL, get_thread, (void *) (long) i) == 0);
  }
  for(int i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0); // Wait for all gets to finish
  }
  t1 = now();            // Record end time
  printf("%d gets, %.3f seconds, %.0f gets/second\n",
          NKEYS*nthread, t1 - t0, (NKEYS*nthread) / (t1 - t0));

  return 0;              // Standard exit
}
