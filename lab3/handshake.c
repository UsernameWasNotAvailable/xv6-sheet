#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main (int argc, char *argv[])
{
    int p1[2];    // Array to hold file descriptors for the first pipe (Parent -> Child)
    int p2[2];    // Array to hold file descriptors for the second pipe (Child -> Parent)

    char buf[1];  // Temporary buffer to hold the character being passed
    char val = argv[1][0]; // Take the first character from the command line argument


    // Create two pipes. pipe() returns 0 on success, -1 on failure.
    // p[0] is the read end, p[1] is the write end.
    if(pipe(p1) < 0 || pipe(p2) < 0){
        printf("pipe failed");
        exit(1);
    }

    // Create a new process
    if (fork() == 0){
        // --- CHILD PROCESS ---
        
        // Block until there is data to read from pipe 1 (sent by parent)
        read(p1[0], buf, 1);
        printf("%d: received %c from parent\n", getpid(), buf[0]);
        
        // Write the received character into pipe 2 (to send back to parent)
        write(p2[1], buf, 1);

        exit(0); // Terminate child process
    } else {
        // --- PARENT PROCESS ---
        
        // Send the initial character into pipe 1
        write(p1[1], &val, 1);
        
        // Block until the child sends the character back through pipe 2
        read(p2[0], buf, 1);

        printf("%d: received %c from child\n", getpid(), buf[0]);

        exit(0); // Terminate parent process
    }
}
