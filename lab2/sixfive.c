#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    char c;
    char buffer[32];               // Storage for the digits we find
    int i = 0;                     // Counter: how many digits are in the buffer
    int is_valid = 1;              // Flag: 1 if word is a pure number, 0 if it has letters
    int fd;
    char *sep = " -\r\t\n./,";     // List of characters that end a "word"

    for (int j = 1; j < argc; j++) {           // Loop through filenames from user input
        if ((fd = open(argv[j], 0)) < 0) {     // Try to open the file
            printf("Error opening file\n");
            continue;                          // Skip to next file if this one fails
        }

        while (read(fd, &c, 1) > 0) {          // Read the file 1 character at a time
            
            if (strchr(sep, c)) {              // --- IF CHARACTER IS A SEPARATOR ---
                if (i > 0 && is_valid) {       // If we have digits and no letters found
                    buffer[i] = '\0';          // Terminate string so atoi() can read it
                    int num = atoi(buffer);    // Convert text "10" into number 10

                    if (num % 5 == 0 || num % 6 == 0) { // Check divisibility
                        printf("%d\n", num);   // Print it if it matches
                    }
                }
                i = 0;                         // Reset: clear the buffer index
                is_valid = 1;                  // Reset: assume next word is a number

            } else if (c >= '0' && c <= '9') { // --- IF CHARACTER IS A DIGIT ---
                if (i < 31) {                  // If there is still space in the buffer
                    buffer[i++] = c;           // Save digit and move index forward
                }

            } else {                           // --- IF CHARACTER IS A LETTER/SYMBOL ---
                is_valid = 0;                  // Mark word as "dirty" (not a pure number)
            }
        }
        close(fd);                             // Close the file before moving to next
    }
    exit(0);                                   // Properly end the program
}
