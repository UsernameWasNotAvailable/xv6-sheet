#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  char buf[512], c;
  char *cmd_argv[MAXARG];
  int m = 0, cmd_argc = 0, use_n = 0;

  if (argc < 2) exit(1);                       // Error if no command is provided

  // 1. Handle Options and Base Command
  if (strcmp(argv[1], "-n") == 0) {            // Check for -n flag
    use_n = 1;
    for (int i = 3; i < argc; i++)             // Command starts at argv[3]
      cmd_argv[cmd_argc++] = argv[i];
  } else {
    for (int i = 1; i < argc; i++)             // Command starts at argv[1]
      cmd_argv[cmd_argc++] = argv[i];
  }

  // 2. Read from Pipe (stdin / FD 0)
  int base_count = cmd_argc;                   // Save the initial command length
  while (read(0, &c, 1) > 0) {
    if (c == ' ' || c == '\t' || c == '\n') {  // If we hit a separator
      if (m > 0) {                             // If we have a word in buffer
        buf[m] = '\0';
        char *arg = malloc(m + 1);             // Allocate memory for the word
        strcpy(arg, buf);
        cmd_argv[cmd_argc++] = arg;            // Add word to argument list
        m = 0;

        if (use_n) {                           // IF -n: Run for every word
          cmd_argv[cmd_argc] = 0;
          if (fork() == 0) { exec(cmd_argv[0], cmd_argv); exit(1); }
          wait(0);
          cmd_argc = base_count;               // Reset to base command
        }
      }
    } else {
      buf[m++] = c;                            // Build word char by char
    }
  }

  // 3. Final Execution (Normal Mode)
  if (!use_n && cmd_argc > base_count) {       // Run once with all words
    cmd_argv[cmd_argc] = 0;
    if (fork() == 0) { exec(cmd_argv[0], cmd_argv); exit(1); }
    wait(0);
  }

  exit(0);
}