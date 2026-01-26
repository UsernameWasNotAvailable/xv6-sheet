#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void memdump(char *fmt, char *data);

int
main(int argc, char *argv[])
{
  if(argc == 1){
    printf("Example 1:\n");
    int a[2] = { 61810, 2025 };
    memdump("ii", (char*) a);
    
    printf("Example 2:\n");
    memdump("S", "a string");
    
    printf("Example 3:\n");
    char *s = "another";
    memdump("s", (char *) &s);

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
    strcpy(example.bytes, "xyzzy");
    
    printf("Example 4:\n");
    memdump("pihcS", (char*) &example);
    
    printf("Example 5:\n");
    memdump("sccccc", (char*) &example);
  } else if(argc == 2){
    // format in argv[1], up to 512 bytes of data from standard input.
    char data[512];
    int n = 0;
    memset(data, '\0', sizeof(data));
    while(n < sizeof(data)){
      int nn = read(0, data + n, sizeof(data) - n);
      if(nn <= 0)
        break;
      n += nn;
    }
    memdump(argv[1], data);
  } else {
    printf("Usage: memdump [format]\n");
    exit(1);
  }
  exit(0);
}

void memdump(char *fmt, char *data) {
  char *ptr = data;                               // Initialize cursor at the start of data

  for(int i = 0; fmt[i] != '\0'; i++) {           // Loop through each format instruction
    char c = fmt[i];                              // Get current rule (e.g., 'i', 'h', 's')

    if(c == 'i') {                                // --- CASE: INTEGER ---
      int value = *(int*)ptr;                     // Cast to int* (4 bytes) and grab value
      printf("%d\n", value);                      // Print as signed decimal
      ptr += sizeof(int);                         // Move cursor forward 4 bytes

    } else if(c == 'h') {                         // --- CASE: SHORT ---
      short value = *(short*)ptr;                 // Cast to short* (2 bytes) and grab value
      printf("%d\n", value);                      // Print as decimal
      ptr += sizeof(short);                       // Move cursor forward 2 bytes

    } else if (c =='c') {                         // --- CASE: CHARACTER ---
      char value = *ptr;                          // Grab exactly 1 byte from current spot
      printf("%c\n", value);                      // Print as ASCII character
      ptr += 1;                                   // Move cursor forward 1 byte

    } else if (c=='s') {                          // --- CASE: STRING POINTER ---
      char *value = *(char**)ptr;                 // Grab 8-byte ADDRESS stored at ptr
      printf("%s\n", value);                      // Print string found at THAT address
      ptr += sizeof(char*);                       // Move cursor forward 8 bytes

    } else if (c=='S') {                          // --- CASE: INLINE STRING ---
      printf("%s\n", ptr);                        // Print text sitting directly at ptr
      ptr += strlen(ptr) + 1;                     // Move past string length + null char

    } else if (c=='p') {                          // --- CASE: HEX/POINTER ---
      unsigned long value = *(unsigned long*)ptr; // Grab 8 bytes as a large number
      printf("%lx\n", value);                     // Print as Hexadecimal (long hex)
      ptr += sizeof(char*);                       // Move cursor forward 8 bytes
    }
  }
}
