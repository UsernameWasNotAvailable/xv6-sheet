// 1. HELPER: Runs a command on a found file
void run_exec(char **cmd_argv, int cmd_argc, char *file_path) {
  char *argv[MAXARG];
  int i;

  // Copy existing command (e.g., "echo") into a new array
  for(i = 0; i < cmd_argc; i++) {
    argv[i] = cmd_argv[i];
  }
  argv[i] = file_path;      // Add the file path as the LAST argument
  argv[i+1] = 0;            // Mandatory NULL terminator for exec()

  int pid = fork();         // Create a child process
  if(pid == 0) {
    exec(argv[0], argv);    // Replace child with the command
    exit(1);                // Exit if exec fails
  } 
  wait(0);                  // Parent waits for command to finish
}

// 2. SEARCH: The main recursive finding logic
void find(char *path, char *target, char **exec_argv, int exec_argc) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, O_RDONLY)) < 0) return;
  if(fstat(fd, &st) < 0) { close(fd); return; }

  // IF MATCH FOUND: If it's a file and the name matches
  if(st.type == T_FILE && strcmp(fmtname(path), target) == 0) {
    if(exec_argv != 0) {
      run_exec(exec_argv, exec_argc, path); // Run the -exec command
    } else {
      printf("%s\n", path);                 // Otherwise, just print path
    }
  }

  // IF DIRECTORY: Dig deeper (Recursion)
  if(st.type == T_DIR) {
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)) {
      if(de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue; // Skip empty entries and recursion loops (. and ..)

      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      find(buf, target, exec_argv, exec_argc); // Pass -exec info down
    }
  }
  close(fd);
}
