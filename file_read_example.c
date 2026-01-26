#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int fd;                               // File ID
    char c;                               // Character buffer
    int j;                                // Loop counter

    for (j = 1; j < argc; j++)            // For each file provided
    {
        // OPEN: Mode 0 is Read-Only
        if((fd = open(argv[j], 0)) < 0) {
            printf("Error opening %s\n", argv[j]);
            continue;                     // Skip failed file
        }

        // READ: 1 byte at a time until End of File (0)
        while (read(fd, &c, 1) > 0)
        {
            printf("%c", c);              // Output byte to screen
            //code hereeeeeeeeeee


            
        }

        close(fd);                        // Clean up
    }
    
    exit(0);                              // End program
}
