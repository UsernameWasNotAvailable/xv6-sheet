// implement this

uint64
sys_symlink(void)
{
  char target[MAXPATH], path[MAXPATH];
  struct inode *ip;

  if(argstr(0, target, MAXPATH) < 0 || argstr(1, path, MAXPATH) < 0)
    return -1;

  begin_op(); 

  // ---------------------------------------------------------
  // ⚠️ HOTSPOT 5: THE FILE TYPE NAME
  // If the exam tells you to define the file type as T_SHORTCUT
  // instead of T_SYMLINK, you MUST change it here, AND in stat.h!
  // ---------------------------------------------------------
  if((ip = create(path, T_SYMLINK, 0, 0)) == 0){
    end_op();
    return -1;
  }

  if(writei(ip, 0, (uint64)target, 0, strlen(target)) != strlen(target)){
    iunlockput(ip);
    end_op();
    return -1;
  }

  iunlockput(ip); 
  end_op(); 
  return 0;
}
