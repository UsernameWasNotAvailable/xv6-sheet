//If the exam asks you to drop the singly-indirect block to save array space, ip->addrs[11] becomes the Doubly-Indirect Master Map directly. You completely remove the singly-indirect if statement.

static uint
bmap(struct inode *ip, uint bn)
{
  uint addr, *a;
  struct buf *bp;

  // 1. Direct Blocks
  if(bn < NDIRECT){
    if((addr = ip->addrs[bn]) == 0){
      addr = balloc(ip->dev);
      if(addr == 0)
        return 0;
      ip->addrs[bn] = addr;
    }
    return addr;
  }
  
  // Subtract the 11 direct blocks
  bn -= NDIRECT; 

  // =========================================================
  // -> EXAM CHANGE HERE: REMOVED SINGLY-INDIRECT LOGIC <-
  // The code immediately jumps to checking the Doubly-Indirect capacity.
  // =========================================================
  
  if(bn < NINDIRECT * NINDIRECT){
    
    // =========================================================
    // -> EXAM CHANGE HERE: NEW MASTER MAP INDEX <-
    // Master Map is now stored in ip->addrs[NDIRECT] instead of NDIRECT+1
    // =========================================================
    if((addr = ip->addrs[NDIRECT]) == 0){
      addr = balloc(ip->dev);
      if(addr == 0)
        return 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;

    // Calculate Indices
    uint index1 = bn / NINDIRECT; 
    uint index2 = bn % NINDIRECT; 

    // Level 2: Secondary Map
    if((addr = a[index1]) == 0){
      addr = balloc(ip->dev);
      if(addr == 0){
        brelse(bp);
        return 0;
      }
      a[index1] = addr;
      log_write(bp);
    }
    brelse(bp);

    // Level 3: Final Data Block
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[index2]) == 0){
      addr = balloc(ip->dev);
      if(addr){
        a[index2] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    return addr;
  }

  panic("bmap: out of range");
}
