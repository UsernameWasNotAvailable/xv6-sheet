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

void
memdump(char *fmt, char *data)
{
    // Initialize a cursor (ptr) to track our current position in the raw memory block
    char *ptr = data;	

    // Loop through the format string until we hit the null terminator '\0'
    for(int i = 0; fmt[i] != '\0'; i++)
    {
        char c = fmt[i]; // Get the current "instruction" (e.g., 'i', 's', 'c')

        if(c == 'i') // 'i' for Integer (4 bytes)
        {
            // (int*)ptr: Tells the CPU to treat the address at ptr as an integer
            // *(int*)ptr: The outer star "grabs" the 4-byte value from that address
            int value = *(int*)ptr;
            printf("%d\n", value);
            ptr += sizeof(int); // Move the cursor 4 bytes forward

        } else if(c == 'h') // 'h' for Short Integer (2 bytes)
        {
            // Cast to short* and dereference to grab 2 bytes
            short value = *(short*)ptr;
            printf("%d\n", value);
            ptr += sizeof(short); // Move the cursor 2 bytes forward

        } else if (c =='c') // 'c' for Character (1 byte)
        {
            // Since ptr is already a char*, we just dereference it directly
            char value = *ptr;
            printf("%c\n", value);
            ptr += 1; // Move the cursor 1 byte forward

        } else if (c=='s') // 's' for Pointer to a String (8 bytes on 64-bit)
        {
            // *(char**)ptr: Treats the memory at ptr as an 8-byte ADDRESS (pointer)
            // It grabs that address and stores it in 'value'
            char *value = *(char**)ptr;
            printf("%s\n", value); // printf follows that address to find the text
            ptr += sizeof(char*); // Move cursor by the size of a pointer (8 bytes)

        } else if (c=='S') // 'S' for Inline String
        {
            // Unlike 's', the text is sitting DIRECTLY at the current ptr
            printf("%s\n", ptr);
            // Move cursor past the string AND its null terminator (+1)
            ptr += strlen(ptr) + 1;

        } else if (c=='p') // 'p' for Hexadecimal Pointer/Address (8 bytes)
        {
            // Cast to unsigned long to ensure we grab the full 8-byte address
            unsigned long value  = *(unsigned long*)ptr;
            printf("%lx\n", value); // Print as hexadecimal
            ptr += sizeof(char*); // Move the cursor 8 bytes forward
        }
    }
}
