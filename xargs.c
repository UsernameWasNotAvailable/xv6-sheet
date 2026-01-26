#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

int main(int argc, char *argv[]) {
  char buf[512], c;                  // buf = temporary word storage; c = current character
  char *cmd_argv[MAXARG];            // Array of pointers for the final command
  int m = 0, cmd_argc = 0;           // m = buffer index; cmd_argc = argument counter
  int use_n = 0;                     // Flag to check if "-n" mode is on

  if (argc < 2) exit(1);             // Error if no command followed 'xargs'

  // --- 1. SETUP THE COMMAND ---
  if (strcmp(argv[1], "-n") == 0) {  // If the user typed "-n"
    use_n = 1;                       // Turn on "Quick Fire" mode
    for (int i = 3; i < argc; i++)   // Command starts at 3rd position
      cmd_argv[cmd_argc++] = argv[i];
  } else {                           // Normal mode (no -n)
    for (int i = 1; i < argc; i++)   // Command starts at 1st position
      cmd_argv[cmd_argc++] = argv[i];
  }

  int base_count = cmd_argc;         // Save the original command length (e.g., "echo")

  // --- 2. THE READING EYE ---
  while (read(0, &c, 1) > 0) {       // Read from Pipe (FD 0) one byte at a time
    // If we hit a "Stop Sign" (Space, Tab, or Newline)
    if (c == ' ' || c == '\t' || c == '\n') {
      if (m > 0) {                   // If we have collected letters in buf
        buf[m] = '\0';               // Terminate the string properly
        char *arg = malloc(m + 1);   // Grab memory for this specific word
        strcpy(arg, buf);            // Copy the word into that memory
        cmd_argv[cmd_argc++] = arg;  // Add it to our list of arguments
        m = 0;                       // Reset buffer for the next word

        if (use_n) {                 // IF -n MODE: Run command for every single word
          cmd_argv[cmd_argc] = 0;    // List must end in 0 for exec
          if (fork() == 0) {         // Create a child twin
            exec(cmd_argv[0], cmd_argv); // Transform child into the command
            exit(1);                 // Exit if transformation fails
          }
          wait(0);                   // Parent waits for child to finish
          cmd_argc = base_count;     // Reset list back to just the base command
        }
      }
    } else {
      buf[m++] = c;                  // It's a letter, keep building the word
    }
  }

  // --- 3. FINAL EXECUTION ---
  if (!use_n && cmd_argc > base_count) { // If normal mode, run ONCE with all words
    cmd_argv[cmd_argc] = 0;              // Add the mandatory 0 at the end
    if (fork() == 0) {
      exec(cmd_argv[0], cmd_argv);
      exit(1);
    }
    wait(0);
  }

  exit(0);                           // Tell the OS we are done
}
