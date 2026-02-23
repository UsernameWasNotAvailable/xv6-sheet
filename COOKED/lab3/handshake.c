#include "kernel/types.h"   // Basic data types used by xv6 (int, uint, etc.)
#include "kernel/stat.h"    // File statistics structure (not strictly needed here, but common include)
#include "user/user.h"      // User-level system calls like fork(), pipe(), read(), write(), exit()

int main (int argc, char *argv[]){
    // argc = number of command-line arguments
    // argv = array of argument strings
    // argv[0] is the program name
    // argv[1] is the first user-provided argument

    // Check if user provided exactly ONE argument
    // (Program name + 1 input = total argc must be 2)
    if (argc != 2){
        // File descriptor 2 = standard error (stderr)
        fprintf(2, "Usage: handshake <byte>\n");
        exit(1);  // Exit with error status
    }

    // Take the first character of the input string
    // Example: if user types "A", argv[1][0] = 'A'
    char byte_to_send = argv[1][0];

    // p1: pipe for parent -> child
    // p2: pipe for child -> parent
    int p1[2], p2[2];

    // Create first pipe
    // p1[0] = read end
    // p1[1] = write end
    pipe(p1);

    // Create second pipe
    // p2[0] = read end
    // p2[1] = write end
    pipe(p2);

    // Create a new process
    int pid = fork();

    // If fork fails
    if (pid < 0){
        fprintf(2, "fork failed\n");
        exit(1);
    }

    // CHILD PROCESS (fork returns 0 in child)
    if (pid == 0){
        char buf;  // buffer to store received byte

        // Read 1 byte from parent through pipe p1
        // p1[0] is read end
        read(p1[0], &buf, 1);

        // Print message showing child received byte
        printf("%d: received %c from parent\n", getpid(), buf);

        // Send the same byte back to parent through pipe p2
        // p2[1] is write end
        write(p2[1], &buf, 1);

        // Exit child process
        exit(0);
    }
    else{
        // PARENT PROCESS (fork returns child's PID)

        char buf;  // buffer to store byte received back

        // Send byte to child through p1
        // p1[1] is write end
        write(p1[1], &byte_to_send, 1);

        // Read 1 byte from child through p2
        // p2[0] is read end
        read(p2[0], &buf, 1);

        // Print message showing parent received byte back
        printf("%d: received %c from child\n", getpid(), buf);

        // Wait for child process to finish (prevent zombie)
        wait(0);

        // Exit parent process
        exit(0);
    }
}