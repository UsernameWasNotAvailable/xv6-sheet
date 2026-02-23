// Question 1: “sixfive” (4 marks)
//  Your solution should be in the file “user/sixfive.c”.
//  For this task, use the system calls “open” and “read”, “C strings”, and
// processing text files in C.
//  For each input file, “sixfive” must print all the numbers in the file that are a
// sequence of the characters 5 and 6 with no more than 3 digits.
//  Numbers are a sequence of decimal digits separated by characters in the string
// " -\r\t\n./,".
//  For the six in "xv6", “sixfive” shouldn't print 6 but, for example, "/6," it should.
//  Hints:
// o Read the input file a character at the time
// o Test if a character match any of the separators using “strchr” (see
// “user/ulib.c”).
// o Start and end of file are implicit separators.
// o Add the program to “UPROGS” in “Makefile”.
// o Changes to the file system persist across runs of “qemu”; to get a clean
// file system run “make clean” and then “make qemu”.
//  The following example illustrates “sixfive's” behavior:
// $ sixfive sixfive.txt
// 555
// 56
// 666
// 565
// $




#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// Function to check if a string consists only of '5' and '6' and length <= 3
void check_and_print(char *buf, int len) {
    if (len == 0 || len > 3) return;

    for (int i = 0; i < len; i++) {
        if (buf[i] != '5' && buf[i] != '6') {
            return; // Contains other characters (like 'x' in 'xv6' or other digits)
        }
    }
    
    // It's a valid sequence, print it
    buf[len] = '\0';
    printf("%s\n", buf);
}

void sixfive(int fd) {
    char c;
    char word[512]; // Buffer to hold the current sequence
    int i = 0;
    char *seps = " -\r\t\n./,";

    while (read(fd, &c, 1) > 0) {
        // If the character is a separator
        if (strchr(seps, c)) {
            check_and_print(word, i);
            i = 0; // Reset buffer index
        } else {
            // Append to current word if it fits
            if (i < 511) {
                word[i++] = c;
            }
        }
    }
    // Handle the last word if file doesn't end with a separator
    check_and_print(word, i);
}

int main(int argc, char *argv[]) {
    int fd;

    if (argc <= 1) {
        fprintf(2, "usage: sixfive file...\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if ((fd = open(argv[i], 0)) < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }
        sixfive(fd);
        close(fd);
    }
    exit(0);
}