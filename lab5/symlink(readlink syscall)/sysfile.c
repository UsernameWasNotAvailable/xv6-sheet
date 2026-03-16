//If the exam asks for a completely new system call to read the contents of the symlink "sticky note" itself (without jumping to the target file), you write this full function in kernel/sysfile.c.

// =========================================================
// -> EXAM CHANGE HERE: ENTIRELY NEW SYSTEM CALL <-
// Requires registration in syscall.c, syscall.h, user.h, usys.pl
// =========================================================
uint64
sys_readlink(void)
{
  char path[MAXPATH];
  uint64 ubuf;    // User buffer address
  int bufsize;    // Size of the user buffer
  struct inode *ip;
  int n;

  // 1. Fetch the 3 arguments from user space
  if(argstr(0, path, MAXPATH) < 0 || argaddr(1, &ubuf) < 0 || argint(2, &bufsize) < 0)
    return -1;

  begin_op(); // Start transaction

  // 2. Find the inode for the path
  if((ip = namei(path)) == 0){
    end_op();
    return -1;
  }
  ilock(ip);

  // 3. Verify it is actually a symlink (fail if it is a normal file or dir)
  if(ip->type != T_SYMLINK){
    iunlockput(ip);
    end_op();
    return -1;
  }

  // 4. Use readi() to copy the target string out of the inode's data blocks 
  // into the user's buffer ('ubuf').
  n = readi(ip, 1, ubuf, 0, bufsize); 

  iunlockput(ip);
  end_op(); // End transaction

  // Return the number of bytes read
  return n; 
}
