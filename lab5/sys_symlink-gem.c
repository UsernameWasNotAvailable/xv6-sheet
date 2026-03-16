uint64
sys_symlink(void)
{
  char target[MAXPATH], path[MAXPATH];
  struct inode *ip;

  // 1. Fetch arguments from user space [cite: 401, 771]
  if(argstr(0, target, MAXPATH) < 0 || argstr(1, path, MAXPATH) < 0)
    return -1;

  begin_op(); // Start transaction [cite: 402, 776]

  // ---------------------------------------------------------
  // -> EXAM CHANGE HERE: SYMLINK TYPE <-
  // The test might ask you to name the type something else, 
  // like T_SHORTCUT instead of T_SYMLINK.
  // ---------------------------------------------------------
  if((ip = create(path, T_SYMLINK, 0, 0)) == 0){ // [cite: 403, 777]
    end_op();
    return -1;
  }

  // 2. Write the target path into the inode's data blocks [cite: 404, 779]
  if(writei(ip, 0, (uint64)target, 0, strlen(target)) != strlen(target)){
    iunlockput(ip);
    end_op();
    return -1;
  }

  iunlockput(ip); // [cite: 406]
  end_op(); // End transaction [cite: 407]
  return 0;
}
