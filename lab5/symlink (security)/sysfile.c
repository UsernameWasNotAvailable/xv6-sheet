//If the exam asks you to prevent symlinks from pointing to sensitive hardware devices or directories. This is a modification applied to sys_open().

uint64
sys_open(void)
{
  // ... (Initial sys_open variable declarations and namei lookup) ...

  int depth = 0;
  while(ip->type == T_SYMLINK && !(omode & O_NOFOLLOW)){
    if(++depth > 10){ 
      iunlockput(ip);
      end_op();
      return -1;
    }
    if(readi(ip, 0, (uint64)path, 0, MAXPATH) <= 0){
      iunlockput(ip);
      end_op();
      return -1;
    }
    iunlockput(ip); 
    if((ip = namei(path)) == 0){ 
      end_op(); 
      return -1;
    }
    ilock(ip); 
  }

  // =========================================================
  // -> EXAM CHANGE HERE: RESTRICT TARGET TYPES <-
  // At this point in the code, 'ip' is the final target.
  // We check 'depth > 0' to confirm this file was reached via a symlink.
  // If the target is a device (or T_DIR, if requested), block it.
  // =========================================================
  if(depth > 0 && ip->type == T_DEVICE) { 
    iunlockput(ip);
    end_op();
    return -1;
  }

  // ... (The rest of sys_open continues normally) ...
  if(ip->type == T_DIR && omode != O_RDONLY){
    iunlockput(ip);
    end_op();
    return -1;
  }
  // ...
