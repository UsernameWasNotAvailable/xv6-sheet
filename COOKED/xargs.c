#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"


int main (int argc, char *argv[]){
    char *new_argv[MAXARG];
    int n_limit = -1;    // -1 signifies "collect everything" until pipe ends --> do the normal echo if no -n provided
    int start_index = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0){
        n_limit = atoi(argv[2]); // conver the 1 or 2 that came in in the 2 index to an int
        start_index = 3; // index wher hte actual command start 
    }

    int i;
    for (i=start_index; i<argc; i++){
        new_argv[i-start_index] = argv[i];   // we need to save teh arguments in order int he new array 
                                   // cuz argument 0 is xargs and we dont wan to save that
                                   // we saving echo and bye inthe array 
    }

    int base_argc = argc - start_index; // cout of static arguamnes
    int current_piped_count = 0; // count of current arguments added from pipe


    char buf[512]; // Buffer to build words/lines character by character
    char c;
    int m=0; // current position in buffer 

    while (read(0, &c, 1) > 0){
        if (c=='\n' || c == '\t' || c==' '){  // Treat spaces, tabs, and newlines as delimiters for "words"
            if (m==0){
                continue; // Skip if we hit multiple spaces in a row
            }
            buf[m]=0; // Null-terminate the string in the buffer
            // Allocate memory for the WHOLE line
            char *line_copy = malloc(m + 1);
            memcpy(line_copy, buf, m + 1);
            
            // Add the whole line as ONE argument
            new_argv[base_argc + current_piped_count] = line_copy;
            current_piped_count++;
            m = 0;

            // Trigger ONLY when we have enough lines to satisfy -n
            if (n_limit != -1 && current_piped_count == n_limit) {
                new_argv[base_argc + current_piped_count] = 0;
                int pid = fork();
                if (pid == 0) {
                    exec(new_argv[0], new_argv);
                    exit(1); // only hreahc her if excte failss :<
                }
                else{
                    wait(0);
                }

                current_piped_count = 0; 
            }
        }
        else{
            buf[m++]=c; // add charter to buffer 
        }
    }

    if (current_piped_count > 0) {
        new_argv[base_argc + current_piped_count] = 0;
        int pid = fork();
        if (pid == 0) {
            exec(new_argv[0], new_argv);
            exit(1);
        }
        else{
            wait(0);
        }
    }
    exit(0);
}


