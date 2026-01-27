#include "kernel/types.h"
#include "user/user.h"

int main (int argc, char * argv[])
{
    char c;
    char buf[512];
    int m = 0;
    char *sep = " \t\n";

    char *arg = (argc > 1) ? argv[1] : "";

    while (read(0, &c, 1) > 0) 
    {
        if (strchr(sep, c)) {
            if(m == 0) continue;
            buf[m] = 0;

            printf("%s %s\n", arg, buf);

            m = 0;
        }
        else {
            // FIX: You want to add characters as long as there is SPACE (m < 511)
            // Not only when it is over 511!
            if (m < 511) { 
                buf[m++] = c;
            }
        }
    }
    
    // This part handles the case where the pipe ends (EOF) 
    // but there was no newline at the end of the input.
    if (m > 0) {
        buf[m] = 0;
        printf("%s %s\n", arg, buf);
    }
    exit(0);
}
