#include "kernel/types.h"                 // Includes basic types like uint, short, etc.
#include "kernel/stat.h"                  // Includes file statistics (standard inclusion)
#include "user/user.h"                    // Includes system calls like pause(), printf(), exit()

int 
main(int argc, char *argv[])              // argc: count of inputs; argv: the inputs themselves
{
    int i;                                // Variable to keep track of our loop position
    
    for(i = 1; i < argc; i++)             // Loop through each input (starts at 1 because 0 is program name)
    {
        int timer = atoi(argv[i]);        // ASCII to Integer: Converts the text input to a real number
        
        printf("sleeping for %d\n", timer); // Print a message so the user knows what's happening
        
        pause(timer);                     // System call: Tells the kernel to put this process to sleep
    }

    exit(0);                              // Tells the OS this process is finished successfully
}
