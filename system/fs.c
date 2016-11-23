#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#if FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD];
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock);

/* YOUR CODE GOES HERE */

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}
     
int fs_mkfs(int dev, int num_inodes) {
  int i;
  
  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8; 
  
  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }
  
  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }
  
  fsd.inodes_used = 0;
  
  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));
  
  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}

int fs_mount(int device)
{
	intmask	mask;			
	struct	nmentry	*namptr;	
	int32	i;			
        mask = disable();

	if ( (isbaddev(device)) || (nnames >= NNAMES) ) {
		restore(mask);
		return SYSERR;
	}
	namptr = &nametab[nnames];	
	namptr->ndevice = device;	
        nnames++;			
	restore(mask);
	return OK;
}

int fs_getfreeblock()
{
	int i;
	for(i=0;i<dev0_numblocks;i++)
	{
		if(fs_getmaskbit(i) == 0)
			return i;
	}
	return -1;
}

int fs_open(char *filename, int flags){
	
	int tmpfd, i, fd;
	
	struct directory rtDir;
	rtDir = fsd.root_dir;
	
	if(fsd.root_dir.numentries != 0){
		for(i=0;i<rtDir.numentries;i++){
			if(strncmp(filename, rtDir.entry[i].name, FILENAMELEN) == 0){
				for(tmpfd = 0; tmpfd<NUM_FD; tmpfd++){
	                                if(next_open_fd == -1){
						return SYSERR;
					}				
	                                if(oft[next_open_fd].state == FSTATE_OPEN ){
						printf("File is open.");
						return tmpfd;
					}
				}
				oft[next_open_fd].fileptr = 0;
				oft[next_open_fd].de = getmem(sizeof(struct dirent));
				oft[next_open_fd].de->inode_num = rtDir.entry[i].inode_num;					
				oft[next_open_fd].flags = flags;
				oft[next_open_fd].state = FSTATE_OPEN;
				
				strncpy(oft[next_open_fd].de->name, rtDir.entry[i].name, FILENAMELEN);
				fs_get_inode_by_num(dev0,oft[next_open_fd].de->inode_num, &oft[next_open_fd].in); 
				fd = next_open_fd;
				for(tmpfd = 0;tmpfd<NUM_FD; tmpfd++){
					if(oft[tmpfd].state == FSTATE_CLOSED){
						next_open_fd = tmpfd;
					}
				}
				if(tmpfd == NUM_FD){
					next_open_fd = -1;
				} 
			}
		}	
	}
	return fd;
}

int fs_close(int fd){
	fs_put_inode_by_num(dev0,oft[fd].in.id, &oft[fd].in);
	if( freemem((char *)oft[fd].de,sizeof(struct dirent)) == SYSERR){
		return SYSERR;
	}
	oft[fd].state = FSTATE_CLOSED;
	return OK;
}

int fs_create(char *filename, int mode){
	int i;
	struct inode *inodeX;
	struct directory rtDir;
	rtDir = fsd.root_dir;
	if(mode != O_CREAT){
		kprintf("Wrong mode passed. Only O_CREAT supported!!");
		return SYSERR;
	}
	if(fsd.root_dir.numentries != 0){
		for(i=0;i<rtDir.numentries;i++){
			if(strncmp(filename, rtDir.entry[i].name, FILENAMELEN )==0){	
				kprintf("file already exists !!");
				return -1;
			}
		}
	}
	
	fsd.inodes_used=fsd.inodes_used+1;
	
	inodeX = getmem(sizeof(struct inode));
	inodeX->id = fsd.inodes_used +1;
	inodeX->device = dev0;
	inodeX->type = INODE_TYPE_FILE;
	inodeX->nlink = 1;
	inodeX->size = 0;

	for(i=0;i<INODEBLOCKS;i++){
		inodeX->blocks[i] = -1;
	}
	
	fs_put_inode_by_num(dev0, inodeX->id, inodeX);
	fsd.root_dir.entry[rtDir.numentries].inode_num  = inodeX->id;
	strncpy(fsd.root_dir.entry[rtDir.numentries].name, filename, FILENAMELEN);
	fsd.root_dir.numentries++; 
	return fs_open(filename, O_RDWR);
}

int fs_seek(int fd, int offset){
	int ptr;
	ptr = oft[fd].fileptr;
	ptr = ptr+offset;
	if( (ptr > oft[fd].in.size) || (ptr < 0) ){
		return SYSERR;
	}
	oft[fd].fileptr = ptr;
	return OK;
}

int fs_read(int fd, void *buf, int nbytes){
	int readCnt = 0, storedCnt = 0, rdbytes;
	int bl_idx,bl_num,offset;
  	bl_idx = oft[fd].fileptr / dev0_blocksize;
	bl_num = oft[fd].in.blocks[bl_idx];
	offset = oft[fd].fileptr % dev0_blocksize;
	storedCnt = nbytes;
	if( oft[fd].flags != O_RDWR && oft[fd].flags != O_RDONLY ){
                kprintf("File should be opened in read mode");
                return SYSERR;
        }
	if( oft[fd].state == FSTATE_CLOSED ){
                kprintf("File is already closed");
                return SYSERR;
        }
	while(readCnt < nbytes){
		if(oft[fd].fileptr >= oft[fd].in.size){
			return readCnt;
		}
		if(bl_num == -1){ 
			kprintf("Could not read block"); 
			return SYSERR;
		}
		if(dev0_blocksize - offset >  storedCnt){
			rdbytes = storedCnt;
		}	
		else{
			rdbytes = dev0_blocksize - offset;
		}		
		if(bs_bread(dev0,bl_num, offset, &buf[readCnt], rdbytes) == SYSERR){
			oft[fd].fileptr = oft[fd].fileptr + readCnt;
			kprintf("Could not read file");
			return readCnt;
		}
		readCnt = readCnt + rdbytes;
		oft[fd].fileptr = oft[fd].fileptr + rdbytes;
		storedCnt = storedCnt - rdbytes;
		bl_idx++;
		bl_num = oft[fd].in.blocks[bl_idx];
		offset = 0;
	}
        return readCnt;
}

int fs_write(int fd, void *buf, int nbytes){
	int write;
	int writtenCnt = 0; 
	int writbytes = 0;
	int bl_idx,bl_num,offset;
	struct inode in;
	
	if(oft[fd].flags != O_WRONLY && oft[fd].flags != O_RDWR){
		kprintf("File has to be open in write mode");
		return SYSERR;
	}
	if(oft[fd].state == FSTATE_CLOSED){
		kprintf("The file is already closed");
		return SYSERR;
	}
	in = oft[fd].in;
	in.size = in.size - (in.size - oft[fd].fileptr);
	bl_idx = oft[fd].fileptr / dev0_blocksize;
        bl_num = oft[fd].in.blocks[bl_idx];
	if(bl_num== -1){
		bl_num = fs_getfreeblock();
		oft[fd].in.blocks[bl_idx] = bl_num;
	}
	offset = oft[fd].fileptr % dev0_blocksize;
	write = nbytes;
	while (write > 0){
		fs_setmaskbit(bl_num);
		if(dev0_blocksize-offset > write){
			writbytes = write;
		}
		else{
			writbytes = dev0_blocksize - offset;
		}
		if(bs_bwrite(dev0, bl_num,offset, &buf[writtenCnt], writbytes) == SYSERR){
			kprintf("Error while writting to file");
			oft[fd].in.size += writtenCnt;
			return writtenCnt;
		}
		writtenCnt = writtenCnt + writbytes;
		write = write - (writbytes);
		bl_idx++;
		oft[fd].in.blocks[bl_idx] = fs_getfreeblock();
		bl_num =  oft[fd].in.blocks[bl_idx]; 
		offset = 0;
	}
	oft[fd].in.size = oft[fd].in.size + writtenCnt; 
	oft[fd].fileptr = oft[fd].in.size;
	return writtenCnt;
}
#endif /* FS */