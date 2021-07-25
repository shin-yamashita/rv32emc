/*------------------------------------------------------------------------/
/  Bitbanging MMCv3/SDv1/SDv2 (in SPI mode) control module
/-------------------------------------------------------------------------/
/
/  Copyright (C) 2012, ChaN, all right reserved.
/
/ * This software is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial products UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/--------------------------------------------------------------------------/
 Features and Limitations:

 * Very Easy to Port
   It uses only 4 bit of GPIO port. No interrupt, no SPI port is used.

 * Platform Independent
   You need to modify only a few macros to control GPIO ports.

 * Low Speed
   The data transfer rate will be several times slower than hardware SPI.

/-------------------------------------------------------------------------*/


#include "stdio.h"
#include "../lib/diskio.h"		/* Common include file for FatFs and disk I/O layer */
#include "ff.h"
#include <ctype.h>

#include "ulib.h"


/* Name status flags */
#define NS          11      /* Index of name status byte in fn[] */
#define NS_LOSS     0x01    /* Out of 8.3 format */
#define NS_LFN      0x02    /* Force to create LFN entry */
#define NS_LAST     0x04    /* Last segment */
#define NS_BODY     0x08    /* Lower case flag (body) */
#define NS_EXT      0x10    /* Lower case flag (ext) */
#define NS_DOT      0x20    /* Dot entry */


/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */
#define MIN_FAT16   4086    /* Minimum number of clusters for FAT16 */
#define MIN_FAT32   65526   /* Minimum number of clusters for FAT32 */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because the structure is not binary compatible between
/ different platforms */

#define BS_jmpBoot      0   /* Jump instruction (3) */
#define BS_OEMName      3   /* OEM name (8) */
#define BPB_BytsPerSec      11  /* Sector size [byte] (2) */
#define BPB_SecPerClus      13  /* Cluster size [sector] (1) */
#define BPB_RsvdSecCnt      14  /* Size of reserved area [sector] (2) */
#define BPB_NumFATs     16  /* Number of FAT copies (1) */
#define BPB_RootEntCnt      17  /* Number of root dir entries for FAT12/16 (2) */
#define BPB_TotSec16        19  /* Volume size [sector] (2) */
#define BPB_Media       21  /* Media descriptor (1) */
#define BPB_FATSz16     22  /* FAT size [sector] (2) */
#define BPB_SecPerTrk       24  /* Track size [sector] (2) */
#define BPB_NumHeads        26  /* Number of heads (2) */
#define BPB_HiddSec     28  /* Number of special hidden sectors (4) */
#define BPB_TotSec32        32  /* Volume size [sector] (4) */
#define BS_DrvNum       36  /* Physical drive number (2) */
#define BS_BootSig      38  /* Extended boot signature (1) */
#define BS_VolID        39  /* Volume serial number (4) */
#define BS_VolLab       43  /* Volume label (8) */
#define BS_FilSysType       54  /* File system type (1) */
#define BPB_FATSz32     36  /* FAT size [sector] (4) */
#define BPB_ExtFlags        40  /* Extended flags (2) */
#define BPB_FSVer       42  /* File system version (2) */
#define BPB_RootClus        44  /* Root dir first cluster (4) */
#define BPB_FSInfo      48  /* Offset of FSInfo sector (2) */
#define BPB_BkBootSec       50  /* Offset of backup boot sectot (2) */
#define BS_DrvNum32     64  /* Physical drive number (2) */
#define BS_BootSig32        66  /* Extended boot signature (1) */
#define BS_VolID32      67  /* Volume serial number (4) */
#define BS_VolLab32     71  /* Volume label (8) */
#define BS_FilSysType32     82  /* File system type (1) */
#define FSI_LeadSig     0   /* FSI: Leading signature (4) */
#define FSI_StrucSig        484 /* FSI: Structure signature (4) */
#define FSI_Free_Count      488 /* FSI: Number of free clusters (4) */
#define FSI_Nxt_Free        492 /* FSI: Last allocated cluster (4) */
#define MBR_Table       446 /* MBR: Partition table offset (2) */
#define SZ_PTE          16  /* MBR: Size of a partition table entry */
#define BS_55AA         510 /* Boot sector signature (2) */

#define DIR_Name        0   /* Short file name (11) */
#define DIR_Attr        11  /* Attribute (1) */
#define DIR_NTres       12  /* NT flag (1) */
#define DIR_CrtTime     14  /* Created time (2) */
#define DIR_CrtDate     16  /* Created date (2) */
#define DIR_FstClusHI       20  /* Higher 16-bit of first cluster (2) */
#define DIR_WrtTime     22  /* Modified time (2) */
#define DIR_WrtDate     24  /* Modified date (2) */
#define DIR_FstClusLO       26  /* Lower 16-bit of first cluster (2) */
#define DIR_FileSize        28  /* File size (4) */
#define LDIR_Ord        0   /* LFN entry order and LLE flag (1) */
#define LDIR_Attr       11  /* LFN attribute (1) */
#define LDIR_Type       12  /* LFN type (1) */
#define LDIR_Chksum     13  /* Sum of corresponding SFN entry */
#define LDIR_FstClusLO      26  /* Filled by zero (0) */
#define SZ_DIR          32  /* Size of a directory entry */
#define LLE         0x40    /* Last long entry flag in LDIR_Ord */
#define DDE         0xE5    /* Deleted directory enrty mark in DIR_Name[0] */
#define NDDE            0x05    /* Replacement of a character collides with DDE */


static void dly_us(int n)
{
    int i;
    volatile u8 d;
    for(i = 0; i < n * 20; i++){
        d = *GPIO;
    }
}

//#define	INIT_PORT()	(*GPIO = 0x85)	/* Initialize MMC control port
//					 (CS=H, CLK=L, DI=H, DO=in) */
#define DLY_US(n)	dly_us(n)	/* Delay n microseconds */

//#define	CS_H()		(*GPIO |= 0x01)	/* Set MMC CS "high" */
//#define CS_L()		(*GPIO &= 0xFE)	/* Set MMC CS "low" */
//#define CK_H()		(*GPIO |= 0x02)	/* Set MMC SCLK "high" */
//#define	CK_L()		(*GPIO &= 0xFD)	/* Set MMC SCLK "low" */
//#define DI_H()		(*GPIO |= 0x04)	/* Set MMC DI "high" */
//#define DI_L()		(*GPIO &= 0xFB)	/* Set MMC DI "low" */
//#define DO		(*GPIO & 0x08)	/* Test for MMC DO ('H':true, 'L':false) */

static  void spi_br(int br)
{
    *SPIBR = br;
}
static  void spi_cs(int cs)
{
        *SPISCS = cs;
}
static  void spi_tx(u8 data)
{
        while(*SPIFLG & 0x2);   // txfull
        *SPITRX = data;
}
//---
static  void spi_tx_blk(u8 data)
{
//        while(*SPIFLG & 0x2);   // txfull
        *SPITRXBLK = data;
}
static  u8 spi_rx()
{
        u8 data;

        while(*SPIFLG & 0x2);   // txfull (wait tx complete)
        data = *SPITRX;
        return data;
}
static  u8 spi_trx()      // ok
{
//      while(*SPIFLG & 0x2);   // txfull
        *SPITRX = 0xff;
        while(*SPIFLG & 0x2);   // txfull (wait tx complete)
        return *SPITRX;
}
static  u8 spi_trx_blk()  // ok
{
//      while(*SPIFLG & 0x2);   // txfull
        *SPITRXBLK = 0xff;
//      while(*SPIFLG & 0x2);   // txfull (wait tx complete)
        return *SPITRXBLK;
}

#define CS_H()	spi_cs(1)
#define CS_L()	spi_cs(0)

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* MMC/SD command (SPI mode) */
#define CMD0	(0)		/* GO_IDLE_STATE */
#define CMD1	(1)		/* SEND_OP_COND */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)		/* SEND_IF_COND */
#define CMD9	(9)		/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define CMD13	(13)		/* SEND_STATUS */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD41	(41)		/* SEND_OP_COND (ACMD) */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* Card type flags (CardType) */
#define CT_MMC		0x01	/* MMC ver 3 */
#define CT_SD1		0x02	/* SD ver 1 */
#define CT_SD2		0x04	/* SD ver 2 */
#define CT_SDC		0x06	/* SD */
#define CT_BLOCK	0x08	/* Block addressing */


static
DSTATUS Stat = STA_NOINIT;	/* Disk status */

static
BYTE CardType;			/* b0:MMC, b1:SDv1, b2:SDv2, b3:Block addressing */



/*-----------------------------------------------------------------------*/
/* Transmit bytes to the card (bitbanging)                               */
/*-----------------------------------------------------------------------*/

static
void xmit_mmc (
    const BYTE* buff,	/* Data to be sent */
    UINT bc			/* Number of bytes to send */
)
{
    do{
        spi_tx(*buff++);
    //	spi_tx_blk(*buff++);
    } while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Receive bytes from the card (bitbanging)                              */
/*-----------------------------------------------------------------------*/

static
void rcvr_mmc (
    BYTE *buff,	/* Pointer to read buffer */
    UINT bc		/* Number of bytes to receive */
)
{
//        spi_rx();       // wait flag
        do {
        spi_tx(0xff);
        *buff++ = spi_rx();
    //	*buff++ = spi_trx();
        //	*buff++ = spi_trx_blk();
        } while (--bc);
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready (void)	/* 1:OK, 0:Timeout */
{
    BYTE d;
    UINT tmr;

    for (tmr = 50000; tmr; tmr--) {	/* Wait for ready in timeout of 500ms */
        rcvr_mmc(&d, 1);
//if(d) printf("wr:%x\n",d);
        if (d == 0xFF) break;
        DLY_US(100);
    }
//printf("wait_ready():%d\n",tmr);
    return tmr ? 1 : 0;
}

/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect (void)
{
    BYTE d;

    CS_H();
    rcvr_mmc(&d, 1);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static
int select (void)	/* 1:OK, 0:Timeout */
{
    BYTE d;

    CS_L();
    rcvr_mmc(&d, 1);	/* Dummy clock (force DO enabled) */

    if (wait_ready()) return 1;	/* OK */
    deselect();
    return 0;			/* Failed */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from the card                                   */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock (	/* 1:OK, 0:Failed */
    BYTE *buff,			/* Data buffer to store received data */
    UINT btr			/* Byte count */
)
{
    BYTE d[2];
    UINT tmr;


    for (tmr = 1000; tmr; tmr--) {	/* Wait for data packet in timeout of 100ms */
        rcvr_mmc(d, 1);
        if (d[0] != 0xFF) break;
        DLY_US(100);
    }
    if (d[0] != 0xFE) return 0;		/* If not valid data token, return with error */

    rcvr_mmc(buff, btr);			/* Receive the data block into buffer */
    rcvr_mmc(d, 2);					/* Discard CRC */

    return 1;						/* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to the card                                        */
/*-----------------------------------------------------------------------*/

static
int xmit_datablock (	/* 1:OK, 0:Failed */
    const BYTE *buff,	/* 512 byte data block to be transmitted */
    BYTE token			/* Data/Stop token */
)
{
    BYTE d[2];


    if (!wait_ready()) return 0;
//printf("xmit_datablock(tok:%x)\n", token);
    rcvr_mmc(d, 1); // 1 byte gap
    d[0] = token;
    xmit_mmc(d, 1);				/* Xmit a token */
    if (token != 0xFD) {		/* Is it data token? */
        xmit_mmc(buff, 512);	/* Xmit the 512 byte data block to MMC */
        rcvr_mmc(d, 2);			/* Xmit dummy CRC (0xFF,0xFF) */
        rcvr_mmc(d, 1);			/* Receive data response */
//        rcvr_mmc(d, 2);         /* Receive data response */
        //printf(" resp:%x\n", d[0]);
        if ((d[0] & 0x1F) != 0x05)   /* If not accepted, return with error */
//        if ((d[1] & 0x1F) != 0x05)
            return 0;
    }

    return 1;
}


/*-----------------------------------------------------------------------*/
/* Send a command packet to the card                                     */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (		/* Returns command response (bit7==1:Send failed)*/
    BYTE cmd,	/* Command byte */
    DWORD arg	/* Argument */
)
{
    BYTE n, d, buf[6];

//printf("send:%2x,%4x\n", cmd, arg);
    if (cmd & 0x80) {	/* ACMD<n> is the command sequense of CMD55-CMD<n> */
        cmd &= 0x7F;
        n = send_cmd(CMD55, 0);
        if (n > 1) return n;
    }

    /* Select the card and wait for ready */
    deselect();
    if (!select()) return 0xFF;
    spi_cs(0);

    /* Send a command packet */
    buf[0] = 0x40 | cmd;			/* Start + Command index */
    buf[1] = (BYTE)(arg >> 24);		/* Argument[31..24] */
    buf[2] = (BYTE)(arg >> 16);		/* Argument[23..16] */
    buf[3] = (BYTE)(arg >> 8);		/* Argument[15..8] */
    buf[4] = (BYTE)arg;				/* Argument[7..0] */
    n = 0x01;						/* Dummy CRC + Stop */
    if (cmd == CMD0) n = 0x95;		/* (valid CRC for CMD0(0)) */
    if (cmd == CMD8) n = 0x87;		/* (valid CRC for CMD8(0x1AA)) */
    buf[5] = n;
    xmit_mmc(buf, 6);

    /* Receive command response */
    if (cmd == CMD12) rcvr_mmc(&d, 1);	/* Skip a stuff byte when stop reading */
    n = 10;								/* Wait for a valid response in timeout of 10 attempts */
    do
        rcvr_mmc(&d, 1);
    while ((d & 0x80) && --n);
//printf("send_cmd(%d):resp:%x\n", cmd, d);

    return d;			/* Return with the response value */
}


/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS _disk_status (
    BYTE drv			/* Drive number (always 0) */
)
{
    DSTATUS s;
    BYTE d;


    if (drv) return STA_NOINIT;

    /* Check if the card is kept initialized */
    s = Stat;
    if (!(s & STA_NOINIT)) {
        if (send_cmd(CMD13, 0))	/* Read card status */
            s = STA_NOINIT;
        rcvr_mmc(&d, 1);		/* Receive following half of R2 */
        deselect();
    }
    Stat = s;

    return s;
}



/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS _disk_initialize (
    BYTE drv		/* Physical drive nmuber (0) */
)
{
    BYTE n, ty, cmd, buf[4];
    UINT tmr;
    DSTATUS s;

    if (drv) return RES_NOTRDY;

//	INIT_PORT();				/* Initialize control port */
    spi_br(105);
//	spi_br(5);
    spi_cs(1);

    for (n = 10; n; n--) rcvr_mmc(buf, 1);	/* 80 dummy clocks */

    ty = 0;
    if (send_cmd(CMD0, 0) == 1) {		/* Enter Idle state */
        spi_br(1);
//printf("CMD0 -> Idle\n");
        if (send_cmd(CMD8, 0x1AA) == 1) {/* SDv2? */
            rcvr_mmc(buf, 4);				/* Get trailing return value of R7 resp */
//printf("CMD8 buf:%2x %2x %2x %2x\n", buf[0],buf[1],buf[2],buf[3]);
            if (buf[2] == 0x01 && buf[3] == 0xAA) {		/* The card can work at vdd range of 2.7-3.6V */
                for (tmr = 1000; tmr; tmr--) {		/* Wait for leaving idle state (ACMD41 with HCS bit) */
                    if (send_cmd(ACMD41, 1UL << 30) == 0) break;
                    DLY_US(1000);
                }
//printf("ACMD41 tmr:%d\n", tmr);
                if (tmr && send_cmd(CMD58, 0) == 0) {	/* Check CCS bit in the OCR */
                    rcvr_mmc(buf, 4);
                    ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;	/* SDv2 */
//printf("CMD58 buf:%2x\n", buf[0]);
                }
            }
//printf("CMD8 ty:%d\n", ty);
        } else {					/* SDv1 or MMCv3 */
            if (send_cmd(ACMD41, 0) <= 1) 	{
                ty = CT_SD1; cmd = ACMD41;	/* SDv1 */
            } else {
                ty = CT_MMC; cmd = CMD1;	/* MMCv3 */
            }
            for (tmr = 1000; tmr; tmr--) {		/* Wait for leaving idle state */
                if (send_cmd(cmd, 0) == 0) break;
                DLY_US(1000);
            }
            if (!tmr || send_cmd(CMD16, 512) != 0)	/* Set R/W block length to 512 */
                ty = 0;
//printf("ACMD41 ty:%d\n", ty);
        }
    }
    CardType = ty;
    s = ty ? 0 : STA_NOINIT;
    Stat = s;

    deselect();

    return s;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT _disk_read (
    BYTE drv,		/* Physical drive nmuber (0) */
    BYTE *buff,		/* Pointer to the data buffer to store read data */
    DWORD sector,		/* Start sector number (LBA) */
    BYTE count		/* Sector count (1..128) */
)
{
    int csect = 1;

    if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
    if (!count) return RES_PARERR;
    if (!(CardType & CT_BLOCK)){
        sector *= 512;	/* Convert LBA to byte address if needed */
        csect = 512;
    }
#if 1
    if (count == 1) {	/* Single block read */
        if ((send_cmd(CMD17, sector) == 0)	/* READ_SINGLE_BLOCK */
            && rcvr_datablock(buff, 512))
            count = 0;
    }
    else {			/* Multiple block read */
printf("read multi %x %d\n", sector, count);
        if (send_cmd(CMD18, sector) == 0) {	/* READ_MULTIPLE_BLOCK */
            do {
                if (!rcvr_datablock(buff, 512)) break;
                buff += 512;
            } while (--count);
            send_cmd(CMD12, 0);		/* STOP_TRANSMISSION */
        }
    }
#else
    while (count) {	/* Single block read */
        if (send_cmd(CMD17, sector) == 0) {	/* READ_SINGLE_BLOCK */
            if(!rcvr_datablock(buff, 512)) break;
            buff += 512;
            sector += csect;
            count--;
        }else{
            break;
        }
    }
#endif
    deselect();

    return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT _disk_write (
    BYTE drv,		/* Physical drive nmuber (0) */
    const BYTE *buff,	/* Pointer to the data to be written */
    DWORD sector,		/* Start sector number (LBA) */
    BYTE count		/* Sector count (1..128) */
)
{
    if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;
    if (!count) return RES_PARERR;
    if (!(CardType & CT_BLOCK)){
        sector *= 512;	/* Convert LBA to byte address if needed */
    }
    if (count == 1) {	/* Single block write */
        int rv1, rv2=-1;
        if (((rv1=send_cmd(CMD24, sector)) == 0)	/* WRITE_BLOCK */
            && (rv2=xmit_datablock(buff, 0xFE)))
            count = 0;
        //printf("_disk_write(sect:%x,cnt:%d) rv1:%d rv2:%d\n", sector, count, rv1, rv2);
    }
    else {			/* Multiple block write */
        if (CardType & CT_SDC) send_cmd(ACMD23, count);
        if (send_cmd(CMD25, sector) == 0) {	/* WRITE_MULTIPLE_BLOCK */
            do {
                if (!xmit_datablock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);
            if (!xmit_datablock(0, 0xFD))	/* STOP_TRAN token */
                count = 1;
        }
    }
    deselect();

    return count ? RES_ERROR : RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT _disk_ioctl (
    BYTE drv,		/* Physical drive nmuber (0) */
    BYTE ctrl,		/* Control code */
    void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT res;
    BYTE n, csd[16];
    WORD cs;


    if (disk_status(drv) & STA_NOINIT) return RES_NOTRDY;	/* Check if card is in the socket */

    res = RES_ERROR;
    switch (ctrl) {
        case CTRL_SYNC :		/* Make sure that no pending write process */
            if (select()) {
                deselect();
                res = RES_OK;
            }
            break;

        case GET_SECTOR_COUNT :	/* Get number of sectors on the disk (DWORD) */
            if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
                if ((csd[0] >> 6) == 1) {	/* SDC ver 2.00 */
                    cs= csd[9] + ((WORD)csd[8] << 8) + 1;
                    *(DWORD*)buff = (DWORD)cs << 10;
                } else {					/* SDC ver 1.XX or MMC */
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    cs = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = (DWORD)cs << (n - 9);
                }
                res = RES_OK;
            }
            break;

        case GET_BLOCK_SIZE :	/* Get erase block size in unit of sector (DWORD) */
            *(DWORD*)buff = 128;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    deselect();

    return res;
}

/*-----------------------------------------------------------------------*/


//
// mmc
// mm8 MMC card test
// 2016/01/09
//

#include "stdio.h"
#include "ulib.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "../lib/diskio.h"

#define limit(x, min, max)	((x)>(max)?(max):((x)<(min)?(min):(x)))

void dump(u8 *pt, int nblk)
{
        int i, j, blk;

    for(blk = 0; blk < nblk; blk++){
        for(i = 0; i < 512; i+=32){
                for(j = 0; j < 32; j++){
                        printf("%02x ", pt[j]);
                }
                for(j = 0; j < 32; j++){
                        char c = pt[j];
                        putchar(c >= 0x20 && c < 0x7f ? c : '.');
                }
                putchar('\n');
                pt += 32;
        }
        putchar('\n');
    }
        return;
}

// ref: http://elm-chan.org/docs/fat.html#sections

#define PT_LbaOfs   8
#define PT_LbaSize  12

u32 lbast, lbasz;

void bpb_read()
{
    int rv;
    u8 buf[512], *pt;
    char str[20];
    rv = _disk_read(0, buf, 0, 1);  // read MBR
    pt = &buf[MBR_Table];
    lbast = *((u32*)&pt[PT_LbaOfs]);
    lbasz = *((u32*)&pt[PT_LbaSize]);
    printf("MBR table: start 0x%x size %d\n", lbast, lbasz);
    rv = _disk_read(0, buf, lbast, 1);
    printf("*** read Boot sect : %x  %d\n", lbast, rv);
    strncpy(str, (const char*)&buf[BS_OEMName],8);
    str[8] = 0;
    printf("BS_OEMName:'%s'\n", str);
    printf("BPB_BytsPerSec: %d\n",*((u16*)(&buf[BPB_BytsPerSec])));
    printf("BPB_SecPerClus: %d\n", buf[BPB_SecPerClus]);
    strncpy(str, (const char*)&buf[BS_VolLab],11);
    str[11] = 0;
    printf("BS_VolLab: '%s'\n", str);
    strncpy(str, (const char*)&buf[BS_FilSysType], 8);
    str[8] = 0;
    printf("BS_FilSysType: '%s'\n", str);
}

u32 prsg;
u8 prs()
{
    prsg = (48271 * prsg) % 0x7fffffff;
    return prsg;
}
void wr_test(int sect, char ir, int n)
{
    int i, rv, err;
    u8 wbuf[512], rbuf[512];
    prsg = n + 1;
    for(i = 0; i < 512; i++){
        switch(ir){
        case 'i': wbuf[i] = i;  break;
        case 'r': wbuf[i] = prs();  break;
        default:  wbuf[i] = -i;
        }
    }
    printf("%d wr test. %c\n", n, ir);
    while(n > 0){
        n--;
        rv = _disk_write(0, wbuf, sect, 1);
        if(rv) {
            printf("%d wr err: %d\n", n, rv);
            return;
        }
        rv = _disk_read(0, rbuf, sect, 1);
        if(rv) {
            printf("%d rd err: %d\n", n, rv);
            return;
        }
        for(err = i = 0; i < 512; i++){
            if(wbuf[i] != rbuf[i]){
                printf("%d  w:%x r:%x\n", i, wbuf[i], rbuf[i]);
                err++;
            }
        }
        if(err){
            printf("%d cmp err: %d\n", n, err);
        }
    }
}

int main(void)
{
    int c, rv;
    char str[140], *tok, ir;
    int sect = 0, nblk = 1;
    u8 buf[512*10];
    int irqdis = 1;

//    zero_bss();_
    _disk_initialize(0);

    bpb_read();

    while(1)
    {
        printf("%4.1f' %4.2fV mmc %d $ ",fu(get_temp()), fu(get_vbatt()), *SPIBR);
        getstr(str);
        tok = strtok(str, " \n");
        if(irqdis) disable_timer();
        if(!strcmp("r", tok)){
            tok = strtok(NULL, " \n");
            if(tok){
                sect = strtoul(tok, NULL, 16);
            }
            rv = _disk_read(0, buf, sect, 1);
            printf("*** read sect %x : %d\n", sect, rv);
            dump(buf, 1);
            sect+=1;
        }else if(!strcmp("mr", tok)){
            tok = strtok(NULL, " \n");
            if(tok){
                sect = strtoul(tok, NULL, 16);
            }
            tok = strtok(NULL, " \n");
            if(tok){
                nblk = strtoul(tok, NULL, 10);
            }
            nblk = limit(nblk, 1, 10);
            rv = _disk_read(0, buf, sect, nblk);
            printf("*** read sect %x x %d : %d\n", sect, nblk, rv);
            dump(buf, nblk);
            sect += nblk;
        }else if(!strcmp("wr", tok)){
            c = 1;
            ir = 'i';
            tok = strtok(NULL, " \n");
            if(tok){
                if(isalpha(*tok)){
                    ir = *tok;
                    tok = strtok(NULL, " \n");
                    if(tok) c = strtoul(tok, NULL, 10);
                }else{
                    c = strtoul(tok, NULL, 10);
                }
            }
            wr_test(lbast+1, ir, c);    // rsved sect
        }else if(!strcmp("irq", tok)){
            irqdis = !irqdis;
            printf(" timer irq %s\n", irqdis?"disable":"enable");
        }else if(!strcmp("bpb", tok)){
            bpb_read();
        }else if(!strcmp("i", tok)){
            _disk_ioctl(0, GET_SECTOR_COUNT, buf);
            c = *((u32*)buf);
            printf(" sect count : %d (%x)\n", c, c);
        }else if(!strcmp("h", tok)){
            puts(
            "    r <sect>       : read sector\n"
            "    mr <sect> <nb> : read multi sect\n"
            "    wr <i/r> <count> : 1 sector write read test\n"
            "    i              : sector count\n"
            "    irq            : timer irq disable (toggle)\n"
            "    bpb            : Read BIOS Parameter Block\n"
            );
        }else if(!strcmp("q", tok)){
            break;
        }
        enable_timer();
    }
    return 0;
}

