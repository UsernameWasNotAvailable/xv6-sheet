// If the exam asks you to actively delete the symlink from the disk if the target file has been removed. This is also a modification inside sys_open().

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

    // =========================================================
    // -> EXAM CHANGE HERE: DANGLING LINK CLEANUP <-
    // DO NOT iunlockput(ip) yet! We must hold onto the current link 
    // inode just in case the target is dead and we need to delete the link.
    // =========================================================
    
    struct inode *next_ip;
    
    if((next_ip = namei(path)) == 0){ 
      // The target file does not exist. The link is dangling.
      
      // 1. Clear the symlink's data blocks
      itrunc(ip);
      
      // 2. Mark the inode as free (type 0) and push update to disk
      ip->type = 0;
      iupdate(ip);
      
      // 3. Now it is safe to unlock and release the dead link
      iunlockput(ip);
      end_op(); 
      return -1; // Return error to the user program
    }
    
    // Target DOES exist! 
    // Unlock the current link, assign the new target, and lock it.
    iunlockput(ip); 
    ip = next_ip;
    ilock(ip); 
  }

  // ... (The rest of sys_open continues normally) ...
