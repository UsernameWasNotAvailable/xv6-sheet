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
  char *ptr = data;
  // int i;
  // for (i=0;i<*fmt;i++){
  while(*fmt){
    switch(*fmt++){
      case 'i':
        // Treat ptr as a pointer to an integer (4 bytes) and dereference it
        int valuei = *(int *)ptr;
        printf("%d\n", valuei);
        // Advance ptr by 4 bytes (size of an int)
        ptr += 4;
        break;
      case 'p':
        // Treat ptr as a pointer to a 64-bit unsigned long (address/pointer)
        unsigned long valuep = *(unsigned long *)ptr;
        printf("%lx\n", valuep);
        // Advance ptr by 8 bytes (size of a 64-bit pointer/long)
        ptr += 8;
        break;
      case 'h':
        // Treat ptr as a pointer to a short int (2 bytes) and dereference it
        short int valueh = *(short int *)ptr;
        printf("%d\n", valueh);
        // Advance ptr by 2 bytes (size of a short int)
        ptr += 2;
        break;
      case 'c':
        // Treat ptr as a pointer to a char (1 byte) and dereference it
        char valuec = *ptr;
        printf("%c\n", valuec);
        // Advance ptr by 1 byte (size of a char)
        ptr += 1;
        break;
      case 's':
        // Case 's': The data contains a pointer (address) to a string located elsewhere.
        // We cast the current data pointer to a char** to extract that address.
        char *values = *(char **)ptr;
        // On a 64-bit system (like xv6 on RISC-V), a pointer is 8 bytes.
        printf("%s\n", values);
        ptr += 8;
        break;
      case 'S':
        // Case 'S': The string data is stored "in-line" (directly at the current pointer).
        // We print the string starting exactly where ptr is currently looking.
        printf("%s\n", ptr);
        // Move ptr forward by the length of the string plus 1 for the null terminator (\0).
        ptr += strlen(ptr)+1;
        break;    
      }
  }


// void
// memdump(char *fmt, char *data)
// {
//   char *ptr = data;
//   while(*fmt){
//     switch(*fmt++){
//       case 'i':   // 4 bytes, 32-bit int, decimal
//         printf("%d\n", *(int *)ptr);
//         ptr += 4;
//         break;
//       case 'c':
//         printf("%c\n", *(char *)ptr);
//         ptr += sizeof(char);
//         break;
//       case 's':
//         printf("%s\n", *(char **)ptr);
//         ptr += sizeof(char *);
//         break;
//       case 'S':   // inline null-terminated string
//         printf("%s\n", ptr);
//         ptr += strlen(ptr) + 1;
//         break;
//       // case 'p':   // 8 bytes, 64-bit int, hex
//       //   for (int i = sizeof(char*)-1;i>=0;i--){
//       //     printf("%x", (unsigned char)ptr[i]);
//       //   }
//       //   printf("\n");
//       //   ptr += 8;
//       //   break;
//       case 'p': {   // 8 bytes, 64-bit int, hex
//         int started = 0;
//         for (int i = 7; i >= 0; i--) {
//           unsigned char b = ptr[i];

//           if (b != 0 || started) {
//             printf("%x", b);
//             started = 1;
//           }
//         }
//         // if all 8 bytes were zero
//         if (!started)
//           printf("0");

//         printf("\n");
//         ptr += 8;
//         break;
//       }
//       case 'h':
//         printf("%d\n", *(short *)ptr);
//         ptr += sizeof(short);
//         break;
//       default:
//         printf("Unknown format character %c\n", *(fmt-1));
//     }
//   }
// }