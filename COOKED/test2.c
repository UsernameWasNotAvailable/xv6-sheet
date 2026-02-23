// // Question 2: “memdump” (4 marks)
// //  Refer to the task “memdump” from the lab assignment “xv6labs-w1”.
// //  Your solution should be in the file “user/memdump.c”.
// //  The task is to extend and add a new format option “q” that would take a 16-bytes
// // of Hex string argument and to print the corresponding ASCII characters; ignore
// // non-printable ASCII characters.
// //  Keep all other previous formats (i, p, h, c, s, S).
// // 3
// // SIT Restricted
// // Code snippets:
// // // Convert a single hex character ('0'-'9', 'a'-'f', 'A'-'F') to its
// // numeric value
// // int hexval(char c)
// // {
// //  if(c >= '0' && c <= '9') return c - '0';
// //  if(c >= 'a' && c <= 'f') return c - 'a' + 10;
// //  if(c >= 'A' && c <= 'F') return c - 'A' + 10;
// //  return -1; // invalid character
// // }
// // // add the following to the switch/if statement in memdump()
// // case 'q': {
// //  // Loop over 16 hex characters (8 bytes)
// //  for(int j = 0; j < 16; j += 2){
// //  int hi = hexval(p[j]); // high nibble of the byte
// //  int lo = ; // fill the missing code

// //  if(hi < 0 || lo < 0)
// //  continue; // skip invalid hex chars

// //  // combine high and low nibbles into a byte
// //  char v = ; // fill the missing code

// //  // printable only
// //  if(v >= && v <= ) // fill the missing values. Refer to the ASCII Table.
// //  printf("%c", v); // print printable ASCII
// //  }
// //  printf("\n");
// //  p += 16; // move pointer past the 16 hex chars
// //  break;
// // }
// // Sample output:
// // $
// // $ echo 5465737420313233 | memdump q
// // Test 123 


case 'q': {
    // Loop over 16 hex characters (representing 8 bytes)
    for(int j = 0; j < 16; j += 2){
        int hi = hexval(p[j]);     // high nibble of the byte
        int lo = hexval(p[j + 1]); // fill: get the low nibble from the next char

        if(hi < 0 || lo < 0)
            continue; // skip invalid hex chars

        // combine high and low nibbles into a single byte
        // Formula: (high * 16) + low
        char v = (hi << 4) | lo; 

        // Printable ASCII range: Space (32) to Tilde (126)
        if(v >= 32 && v <= 126) 
            printf("%c", v); // print printable ASCII
    }
    printf("\n");
    p += 16; // move pointer past the 16 hex chars processed
    break;
}


// or use ptr 
// if you define p as ptrrrrr in your code 


// making a new syscall ???
#include "kernel/types.h"
#include "user/user.h"

uint32 swap32(uint32 x) {
    return ((x & 0x000000FF) << 24) |
           ((x & 0x0000FF00) << 8)  |
           ((x & 0x00FF0000) >> 8)  |
           ((x & 0xFF000000) >> 24);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: swap32 <hex_value>\n");
        exit(1);
    }

    uint32 input = (uint32)atoi_hex(argv[1]); // Assuming a hex-to-int helper
    uint32 output = swap32(input);

    printf("Input: 0x%x\n", input);
    printf("Swapped: 0x%x\n", output);
    exit(0);
}




// adding sytem call ????
uint64
sys_endianswap(void)
{
  int x;
  // Retrieve the integer argument from user-space
  argint(0, &x);
  
  uint32 val = (uint32)x;
  // Perform the swap
  return ((val & 0x000000FF) << 24) |
         ((val & 0x0000FF00) << 8)  |
         ((val & 0x00FF0000) >> 8)  |
         ((val & 0xFF000000) >> 24);
}


// etsting the code ???

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        exit(1);
    }

    // Basic check for hex prefix and valid characters
    if (argv[1][0] != '0' || (argv[1][1] != 'x' && argv[1][1] != 'X')) {
        printf("%s\nInvalid argument\n", argv[1]);
        exit(1);
    }

    // Convert hex string to uint (simplified)
    uint x = (uint)strtol(argv[1], 0, 16); 
    
    // Call the kernel
    uint swapped = endianswap(x);

    printf("0x%x\n0x%x\n", x, swapped);
    exit(0);
}


// idk how to works 