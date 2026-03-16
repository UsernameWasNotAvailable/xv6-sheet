#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400
// ---------------------------------------------------------
// -> EXAM ADDITION HERE <-
// ---------------------------------------------------------
#define O_NOFOLLOW 0x800

//The Exam Twist: Why 0x800? Because xv6 uses hexadecimal bit flags.

//0x200 in binary is 0010 0000 0000

//0x400 in binary is 0100 0000 0000

//The next independent bit is 1000 0000 0000, which is 0x800 in hexadecimal.
//If the exam has existing flags that already use 0x800, you must jump to the next bit, which would be 0x1000.
