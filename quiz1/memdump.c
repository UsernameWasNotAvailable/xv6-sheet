#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Forward declaration of the dumping function
void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    // --- NO ARGUMENTS: RUN BUILT-IN EXAMPLES ---
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a); //
    
    printf("Example 2:\n");
    memdump("S", "a string"); //
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s); //

    struct sss {
      char *ptr;
      int num1;
      short num2;
      char byte;
      char bytes[8];
    } example;
    
    example.ptr = "hello";
    example.num1 = 1819438967;
    example.num2 = 100;
    example.byte = 'z';
    strcpy(example.bytes, "");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example); //
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example); //
  } else if(argc == 2){
    // --- ARGUMENT PROVIDED: READ FROM STDIN ---
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n); //
      if(nn <= 0) break;
      n += nn;
    }
    memdump(argv[1], data); //
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void
memdump(char *fmt, char *data)
{
  char *ptr = data; // Pointer to current position in memory

  // Helper to convert hex characters (e.g., 'a') to numeric values (10)
  int hexval(char c){
    if(c >= '0' && c <= '9') return c - '0';
    if(c >= 'a' && c <= 'f') return c - 'a' + 10;
    if(c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1; 
  }

  while(*fmt){
    switch(*fmt++){
      case 'i': // INTEGER (4 bytes)
        printf("%d\n", *(int *)ptr);
        ptr += 4;
        break;
      case 'p': // POINTER/ADDRESS (8 bytes on 64-bit RISC-V)
        printf("%lx\n", *(unsigned long *)ptr);
        ptr += 8;
        break;
      case 'h': // SHORT (2 bytes)
        printf("%d\n", *(short int *)ptr);
        ptr += 2;
        break;
      case 'c': // CHARACTER (1 byte)
        printf("%c\n", *ptr);
        ptr += 1;
        break;
      case 's': // STRING POINTER (8-byte address to a string)
        printf("%s\n", *(char **)ptr);
        ptr += 8;
        break;
      case 'S': // IN-LINE STRING (characters stored at current location)
        printf("%s\n", ptr);
        ptr += strlen(ptr) + 1; 
        break;    
      case 'q': { // HEX-TO-ASCII (Processes 16 hex chars to 8 bytes)
        for(int j = 0; j < 16; j += 2){
          // Fix: Extract adjacent hex pairs (e.g., ptr[0] and ptr[1])
          int hi = hexval(ptr[j]);     // High nibble
          int lo = hexval(ptr[j + 1]); // Low nibble

          if(hi < 0 || lo < 0) continue; 

          // Combine nibbles: (hi << 4) sets bits 4-7, | lo sets bits 0-3
          char v = (hi << 4) | lo; 

          // Print only printable ASCII characters (space 32 to ~ 126)
          if(v >= 32 && v <= 126){ 
            printf("%c", v); 
          }
        }
        printf("\n");
        ptr += 16; // Advance past the 16 hex characters consumed
        break;
      }
    }
  }
}
