#include <stdlib.h>      // Standard library for malloc, free, and random
#include <unistd.h>      // Standard symbolic constants (like NULL)
#include <stdio.h>       // Standard I/O for printf
#include <assert.h>      // For assert() to verify program assumptions
#include <pthread.h>     // POSIX threads library for multi-threading
#include <sys/time.h>    // For gettimeofday high-resolution timing

#define NBUCKET 5        // The hash table has 5 buckets (slots)
#define NKEYS 100000     // Total number of keys to insert in the test

struct entry {           // Define a node for the linked list in each bucket
  int key;               // The lookup key
  int value;             // The stored value
  struct entry *next;    // Pointer to the next entry in the chain (collision handling)
};
struct entry *table[NBUCKET]; // The actual hash table (array of pointers)
int keys[NKEYS];              // Global array to store random keys for testing
int nthread = 1;              // Global variable for the number of threads

// Global array of mutex locks: one unique lock for every bucket in the table
static pthread_mutex_t locks[NBUCKET];

double now()             // Helper function to get current time in seconds
{
 struct timeval tv;
 gettimeofday(&tv, 0);   // Get system clock with microsecond precision
 return tv.tv_sec + tv.tv_usec / 1000000.0; // Convert to total seconds
}

static void 
insert(int key, int value, struct entry **p, struct entry *n)
{
  struct entry *e = malloc(sizeof(struct entry)); // Allocate memory for a new node
  e->key = key;          // Set the key
  e->value = value;      // Set the value
  e->next = n;           // Point new node's next to the current head (n)
  *p = e;                // Update the bucket pointer to point to this new head
}

static void put(int key, int value)
{
  int i = key % NBUCKET; // Determine which bucket the key belongs to
  
  pthread_mutex_lock(&locks[i]); // LOCK: Start of critical section for bucket 'i'

  struct entry *e = 0;
  for (e = table[i]; e != 0; e = e->next) { // Traverse the list in bucket 'i'
    if (e->key == key)   // If the key already exists...
      break;             // Stop searching
  }
  if(e){                 // If key was found...
    e->value = value;    // Update the existing key's value
  } else {               // If key is not in the table...
    insert(key, value, &table[i], table[i]); // Add it to the front of the bucket
  }

  pthread_mutex_unlock(&locks[i]); // UNLOCK: End of critical section
}

static struct entry* get(int key)
{
  int i = key % NBUCKET; // Determine which bucket the key should be in
  struct entry *e = 0;
  // Searching through the bucket's linked list
  for (e = table[i]; e != 0; e = e->next) {
    if (e->key == key) break; // Found the key
  }
  return e;              // Return the entry (or 0 if not found)
}

static void * put_thread(void *xa)
{
  int n = (int) (long) xa; // Convert thread argument back to integer ID
  int b = NKEYS/nthread;   // Divide keys equally among threads

  for (int i = 0; i < b; i++) {
    put(keys[b*n + i], n); // Each thread puts its specific range of keys
  }
  return NULL;
}

static void * get_thread(void *xa)
{
  int n = (int) (long) xa; // Thread ID
  int missing = 0;         // Counter for keys that weren't found

  for (int i = 0; i < NKEYS; i++) { // Every thread checks for ALL keys
    struct entry *e = get(keys[i]);
    if (e == 0) missing++; // If key is missing, increment counter
  }
  printf("%d: %d keys missing\n", n, missing); // Report total missing keys
  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t *tha;        // Array of thread identifiers
  void *value;           // Return value from thread joins
  double t1, t0;         // Timing variables

  if (argc < 2) {        // Check if user provided thread count
    fprintf(stderr, "Usage: %s nthreads\n", argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]); // Parse thread count from command line
  tha = malloc(sizeof(pthread_t) * nthread); // Allocate thread array
  srandom(0);            // Seed random number generator for reproducibility
  assert(NKEYS % nthread == 0); // Ensure keys can be divided evenly by threads
  for (int i = 0; i < NKEYS; i++) {
    keys[i] = random();  // Fill keys array with random data
  }

  for(int i = 0; i < NBUCKET; i++){
    pthread_mutex_init(&locks[i], NULL); // Initialize one mutex for each bucket
  }

  // PHASE 1: Concurrent Insertions (PUTS)
  t0 = now();            // Start clock
  for(int i = 0; i < nthread; i++) {
    // Create threads to execute put_thread
    assert(pthread_create(&tha[i], NULL, put_thread, (void *) (long) i) == 0);
  }
  for(int i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0); // Wait for all threads to finish
  }
  t1 = now();            // End clock
  printf("%d puts, %.3f seconds, %.0f puts/second\n",
          NKEYS, t1 - t0, NKEYS / (t1 - t0));

  // PHASE 2: Concurrent Lookups (GETS)
  t0 = now();            // Reset clock
  for(int i = 0; i < nthread; i++) {
    // Create threads to execute get_thread
    assert(pthread_create(&tha[i], NULL, get_thread, (void *) (long) i) == 0);
  }
  for(int i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0); // Wait for all threads to finish
  }
  t1 = now();            // End clock
  printf("%d gets, %.3f seconds, %.0f gets/second\n",
          NKEYS*nthread, t1 - t0, (NKEYS*nthread) / (t1 - t0));

  for(int i = 0; i < NBUCKET; i++){
    pthread_mutex_destroy(&locks[i]); // Clean up/destroy the mutexes
  }

  return 0;              // Exit successfully
}
