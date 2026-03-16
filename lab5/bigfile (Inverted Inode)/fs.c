// You must rewrite the order of operations in bmap() and itrunc() to check the higher-capacity structures at the beginning of the array, completely flipping the standard array indexes.
//In this scenario, the exam twist flips the array indexing so that the "heaviest" pointers are at the front of the inode's array.

static uint
bmap(struct inode *ip, uint bn)
{
  uint addr, *a;
  struct buf *bp;

  // 1. Direct Blocks
  // Logically, blocks 0-10 are still direct blocks.
  if(bn < NDIRECT){
    // ---------------------------------------------------------
    // -> EXAM CHANGE HERE: DIRECT BLOCKS OFFSET <-
    // Direct blocks now start at index 2, so we add 2 to 'bn'.
    // ---------------------------------------------------------
    if((addr = ip->addrs[bn + 2]) == 0){ 
      addr = balloc(ip->dev);
      if(addr == 0)
        return 0;
      ip->addrs[bn + 2] = addr; 
    }
    return addr;
  }
  bn -= NDIRECT;

  // 2. Singly-Indirect Blocks
  if(bn < NINDIRECT){
    // ---------------------------------------------------------
    // -> EXAM CHANGE HERE: SINGLY-INDIRECT INDEX <-
    // The singly-indirect map is now at index 1.
    // ---------------------------------------------------------
    if((addr = ip->addrs[1]) == 0){ 
      addr = balloc(ip->dev);
      if(addr == 0)
        return 0;
      ip->addrs[1] = addr; 
    }
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[bn]) == 0){
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
    
    // ---------------------------------------------------------
    // -> EXAM CHANGE HERE: DOUBLY-INDIRECT INDEX <-
    // The doubly-indirect Master Map is now at index 0.
    // ---------------------------------------------------------
    if((addr = ip->addrs[0]) == 0){ 
      addr = balloc(ip->dev);
      if(addr == 0)
        return 0;
      ip->addrs[0] = addr; 
    }
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;

    // Calculate Indices (Math stays exactly the same!)
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


///////////////////////////////////////////////////////////////////////////////////////

void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *bp;
  uint *a;

  // 1. Clear Direct Blocks
  for(i = 0; i < NDIRECT; i++){
    // ---------------------------------------------------------
    // -> EXAM CHANGE HERE: DIRECT BLOCKS OFFSET <-
    // We must check indexes 2 through 12.
    // ---------------------------------------------------------
    if(ip->addrs[i + 2]){ 
      bfree(ip->dev, ip->addrs[i + 2]); 
      ip->addrs[i + 2] = 0; 
    }
  }

  // 2. Clear Singly-Indirect Blocks
  // ---------------------------------------------------------
  // -> EXAM CHANGE HERE: SINGLY-INDIRECT INDEX <-
  // Check index 1 instead of NDIRECT.
  // ---------------------------------------------------------
  if(ip->addrs[1]){ 
    bp = bread(ip->dev, ip->addrs[1]); 
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT; j++){
      if(a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[1]); 
    ip->addrs[1] = 0; 
  }

  // 3. Clear Doubly-Indirect Blocks
  struct buf *bp1, *bp2;
  uint *a1, *a2;

  // ---------------------------------------------------------
  // -> EXAM CHANGE HERE: DOUBLY-INDIRECT INDEX <-
  // Check index 0 instead of NDIRECT+1.
  // ---------------------------------------------------------
  if(ip->addrs[0]){ 
    bp1 = bread(ip->dev, ip->addrs[0]); // Open Master Map at index 0
    a1 = (uint*)bp1->data;

    for(i = 0; i < NINDIRECT; i++){ 
      if(a1[i]){
        bp2 = bread(ip->dev, a1[i]); // Open Secondary Map
        a2 = (uint*)bp2->data;

        for(j = 0; j < NINDIRECT; j++){ 
          if(a2[j])
            bfree(ip->dev, a2[j]); // Free Data Block
        }
        brelse(bp2); 
        bfree(ip->dev, a1[i]); 
      }
    }
    brelse(bp1); 
    bfree(ip->dev, ip->addrs[0]); // Free Master Map Block
    ip->addrs[0] = 0; // Clear index 0
  }

  ip->size = 0;
  iupdate(ip);
}











