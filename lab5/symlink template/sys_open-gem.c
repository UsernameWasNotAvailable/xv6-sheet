// WARNING IDKA BOUT THISSS

uint64
sys_open(void)
{
  char path[MAXPATH];
  int fd, omode;
  struct file *f;
  struct inode *ip;
  int n;

  argint(1, &omode); // [cite: 418]
  if((n = argstr(0, path, MAXPATH)) < 0) // [cite: 419]
    return -1;

  begin_op(); // [cite: 420]

  if(omode & O_CREATE){ // [cite: 421]
    ip = create(path, T_FILE, 0, 0);
    if(ip == 0){
      end_op();
      return -1;
    }
  } else {
    if((ip = namei(path)) == 0){
      end_op();
      return -1;
    }
    ilock(ip);
  }

  // =========================================================
  // --- START OF SYMLINK RESOLUTION ---
  // =========================================================
  int depth = 0; // [cite: 424]
  
  // ---------------------------------------------------------
  // -> EXAM CHANGE HERE: THE NOFOLLOW FLAG <-
  // They might ask you to reverse the logic (e.g., O_FOLLOW) 
  // or use a different flag name.
  // ---------------------------------------------------------
  while(ip->type == T_SYMLINK && !(omode & O_NOFOLLOW)){ // [cite: 425, 444, 783]
    
    // ---------------------------------------------------------
    // -> EXAM CHANGE HERE: THE RECURSION GUARD <-
    // The lab uses a depth of 10. The exam might ask you to 
    // cap it at 5, 20, or something else to test if you know 
    // where the infinite loop protection is.
    // ---------------------------------------------------------
    if(++depth > 10){ // Recursion guard [cite: 453, 477, 791, 821]
      iunlockput(ip);
      end_op();
      return -1;
    }

    // Read the target path from the link [cite: 446, 786, 823]
    if(readi(ip, 0, (uint64)path, 0, MAXPATH) <= 0){
      iunlockput(ip);
      end_op();
      return -1;
    }

    iunlockput(ip); // Unlock current link [cite: 485, 814]
    
    // Find the new inode for the target path [cite: 448, 787, 812]
    if((ip = namei(path)) == 0){ 
      end_op(); // Target file was deleted / Dangling link [cite: 449, 482, 788, 805]
      return -1;
    }
    ilock(ip); // Lock the new target [cite: 471, 804]
  }
  // =========================================================
  // --- END OF SYMLINK RESOLUTION ---
  // =========================================================

  if(ip->type == T_DIR && omode != O_RDONLY){ // [cite: 427]
    iunlockput(ip);
    end_op();
    return -1;
  }

  // ... (The rest of the standard sys_open code continues here: 
  // checking T_DEVICE, allocating file descriptor, etc.) ...
  
  // Example of the rest of the file:
  if(ip->type == T_DEVICE && (ip->major < 0 || ip->major >= NDEV)){ // [cite: 428]
    iunlockput(ip);
    end_op();
    return -1;
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){ // [cite: 429]
    if(f) fileclose(f);
    iunlockput(ip);
    end_op();
    return -1;
  }

  if(ip->type == T_DEVICE){ // [cite: 430]
    f->type = FD_DEVICE;
    f->major = ip->major;
  } else { // [cite: 431]
    f->type = FD_INODE;
    f->off = 0;
  }
  f->ip = ip; // [cite: 432]
  f->readable = !(omode & O_WRONLY); // [cite: 433]
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR); // [cite: 435]

  if((omode & O_TRUNC) && ip->type == T_FILE){ // [cite: 436]
    itrunc(ip);
  }

  iunlock(ip); // [cite: 437]
  end_op(); // [cite: 438]

  return fd; // [cite: 439]
}
