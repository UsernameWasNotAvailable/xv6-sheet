#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

// Define the 3 possible states for our "Machine"
#define ST_READY    0   // Waiting to start a new word
#define ST_DIGITS   1   // Currently reading a valid number (0-9 only)
#define ST_INVALID  2   // Current word has letters; ignore until next space

void sixfive(int fd) {
    char c;
    int state = ST_READY;            // Start in the READY state
    long long current_val = 0;       // Holds the actual math value of the number
    char *seps = " -\r\t\n./,";      // Characters that end a word (separators)

    // Read the file 1 character at a time
    while (read(fd, &c, 1) > 0) {
        
        if (strchr(seps, c)) {       // --- CASE 1: HIT A SEPARATOR ---
            if (state == ST_DIGITS) { // If we just finished a clean number
                // Check if divisible by 5 OR 6
                if (current_val % 5 == 0 || current_val % 6 == 0) {
                    printf("%lld\n", current_val);
                }
            }
            state = ST_READY;        // Reset state for the next word
            current_val = 0;         // Reset math value to zero
        } 
        else if (c >= '0' && c <= '9') { // --- CASE 2: HIT A DIGIT ---
            if (state == ST_READY || state == ST_DIGITS) {
                state = ST_DIGITS;   // We are officially reading a number
                // Math trick: move decimal over and add the new digit
                current_val = current_val * 10 + (c - '0');
            }
            // Note: if state is already INVALID, we stay INVALID
        } 
        else {                       // --- CASE 3: HIT A LETTER/SYMBOL ---
            state = ST_INVALID;      // "Poison" the current word
            current_val = 0;
        }
    }

    // FINAL CHECK: If file ends with a number (no separator at end)
    if (state == ST_DIGITS) {
        if (current_val % 5 == 0 || current_val % 6 == 0) {
            printf("%lld\n", current_val);
        }
    }
}

int main(int argc, char *argv[]) {
    int fd, i;

    if (argc < 2) {                  // User didn't provide a filename
        fprintf(2, "usage: sixfive files...\n");
        exit(1);
    }

    for (i = 1; i < argc; i++) {     // Loop through every file provided
        if ((fd = open(argv[i], O_RDONLY)) < 0) {
            fprintf(2, "sixfive: cannot open %s\n", argv[i]);
            exit(1);
        }
        sixfive(fd);                 // Run the parser on this file
        close(fd);                   // Close file to save resources
    }
    exit(0);
}
