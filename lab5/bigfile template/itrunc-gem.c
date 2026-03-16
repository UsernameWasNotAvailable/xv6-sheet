// Truncate inode (discard contents).
// Caller must hold ip->lock.
void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *bp;
  uint *a;

  // 1. Clear Direct Blocks
  for(i = 0; i < NDIRECT; i++){
    if(ip->addrs[i]){
      bfree(ip->dev, ip->addrs[i]);
      ip->addrs[i] = 0;
    }
  }

  // 2. Clear Singly-Indirect Blocks
  if(ip->addrs[NDIRECT]){
    bp = bread(ip->dev, ip->addrs[NDIRECT]);
    a = (uint*)bp->data;
    for(j = 0; j < NINDIRECT; j++){
      if(a[j])
        bfree(ip->dev, a[j]);
    }
    brelse(bp);
    bfree(ip->dev, ip->addrs[NDIRECT]);
    ip->addrs[NDIRECT] = 0;
  }

  // 3. Clear Doubly-Indirect Blocks
  struct buf *bp1, *bp2;
  uint *a1, *a2;

  // Check the 13th slot (index NDIRECT+1) for the Doubly-Indirect block
  if(ip->addrs[NDIRECT+1]){
    bp1 = bread(ip->dev, ip->addrs[NDIRECT+1]); // Open Master Map
    a1 = (uint*)bp1->data;

    // ---------------------------------------------------------
    // -> EXAM NOTE FOR HARD CAPS <-
    // Even if your file is capped at 10000 blocks, this loop 
    // safely runs up to NINDIRECT (256). 
    // If the master map entry is empty (because of the cap), 
    // the "if(a1[i])" check prevents it from doing unnecessary work.
    // ---------------------------------------------------------
    for(i = 0; i < NINDIRECT; i++){ // Loop 1
      if(a1[i]){
        bp2 = bread(ip->dev, a1[i]); // Open Secondary Map
        a2 = (uint*)bp2->data;

        for(j = 0; j < NINDIRECT; j++){ // Loop 2
          if(a2[j])
            bfree(ip->dev, a2[j]); // Free Data Block
        }
        brelse(bp2); // Close Secondary Map
        bfree(ip->dev, a1[i]); // Free Secondary Map Block
      }
    }
    brelse(bp1); // Close Master Map
    bfree(ip->dev, ip->addrs[NDIRECT+1]); // Free Master Map Block
    ip->addrs[NDIRECT+1] = 0; // Clear the 13th slot
  }

  ip->size = 0; 
  iupdate(ip); 
}
