1. kernel/syscall.h
Add the system call number:

#define SYS_symlink 22 -----

2. kernel/syscall.c
Hook the handler to the system call table:

extern uint64 sys_symlink(void);

static uint64 (*syscalls[])(void) = {
...
[SYS_symlink] sys_symlink, -----
};

user/usys.pl & user/user.h
Add the entry point for the user-space stub and the function prototype:

// user.h
int symlink(const char*, const char*); -----

// usys.pl
entry("symlink"); -----
