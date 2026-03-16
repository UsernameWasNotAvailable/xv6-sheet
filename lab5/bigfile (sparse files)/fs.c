//In this variation, bmap must take a third parameter (int alloc). If alloc is 0 (meaning a read operation), bmap should just return 0 instead of trying to assign disk space to empty blocks.

// =========================================================
// -> EXAM CHANGE HERE: ADD 'alloc' FLAG TO SIGNATURE <-
// =========================================================
static uint
bmap(struct inode *ip, uint bn, int alloc) 
{
  uint addr, *a;
  struct buf *bp;

  // 1. Direct Blocks
  if(bn < NDIRECT){
    if((addr = ip->addrs[bn]) == 0){
      // =========================================================
      // -> EXAM CHANGE HERE: EARLY EXIT FOR READS <-
      // =========================================================
      if(alloc == 0) return 0; 
      
      addr = balloc(ip->dev);
      if(addr == 0) return 0;
      ip->addrs[bn] = addr;
    }
    return addr;
  }
  bn -= NDIRECT;

  // 2. Singly-Indirect Blocks
  if(bn < NINDIRECT){
    if((addr = ip->addrs[NDIRECT]) == 0){
      // =========================================================
      // -> EXAM CHANGE HERE: EARLY EXIT <-
      // =========================================================
      if(alloc == 0) return 0; 
      
      addr = balloc(ip->dev);
      if(addr == 0) return 0;
      ip->addrs[NDIRECT] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[bn]) == 0){
      // =========================================================
      // -> EXAM CHANGE HERE: EARLY EXIT <-
      // Remember to brelse(bp) before returning!
      // =========================================================
      if(alloc == 0){
        brelse(bp);
        return 0;
      }
      addr = balloc(ip->dev);
      if(addr){
        a[bn] = addr;
        log_write(bp);
      }
    }
    brelse(bp);
    return addr;
  }

  // 3. Doubly-Indirect Blocks
  bn -= NINDIRECT;
  if(bn < NINDIRECT * NINDIRECT){
    // Level 1: Master Map
    if((addr = ip->addrs[NDIRECT+1]) == 0){
      if(alloc == 0) return 0; // -> EXAM CHANGE <-
      addr = balloc(ip->dev);
      if(addr == 0) return 0;
      ip->addrs[NDIRECT+1] = addr;
    }
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;

    uint index1 = bn / NINDIRECT; 
    uint index2 = bn % NINDIRECT; 

    // Level 2: Secondary Map
    if((addr = a[index1]) == 0){
      if(alloc == 0){ // -> EXAM CHANGE <-
        brelse(bp);
        return 0;
      }
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
      if(alloc == 0){ // -> EXAM CHANGE <-
        brelse(bp);
        return 0;
      }
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
