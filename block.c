/*****************************************************************************\
 **                                                                         **
 **  FILE:     block.h                                                      **
 **  AUTHORS:  Alexios (originally: Ben Fowler)                             **
 **  LEGALESE:                                                              **
 **                                                                         **
 **  This program is free software; you  can redistribute it and/or modify  **
 **  it under the terms of the GNU  General Public License as published by  **
 **  the Free Software Foundation; either version 2 of the License, or (at  **
 **  your option) any later version.                                        **
 **                                                                         **
 **  This program is distributed  in the hope  that it will be useful, but  **
 **  WITHOUT    ANY WARRANTY;   without  even  the    implied warranty  of  **
 **  MERCHANTABILITY or  FITNESS FOR  A PARTICULAR  PURPOSE.   See the GNU  **
 **  General Public License for more details.                               **
 **                                                                         **
 **  You  should have received a copy   of the GNU  General Public License  **
 **  along with    this program;  if   not, write  to  the   Free Software  **
 **  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.              **
\*****************************************************************************/


/*
 * $Id: block.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $
 *
 * $Log: block.c,v $
 * Revision 0.1  1999/05/27 19:43:16  alexios
 * Initial revision, as adapted from Duh Draw.
 *
 *
 */


#ifndef RCS_VER 
#define RCS_VER "$Id: block.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $"
#endif


#include "tq.h"


static unsigned char mirrorchars[256]={
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 11, 10, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 27, 26, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 41, 40, 42, 43, 44, 45, 46, 92,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 62, 61, 60, 63,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 93, 47, 91, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,125,124,123,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,170,169,171,172,173,175,174,
  176,177,178,179,195,198,199,214,213,204,186,201,200,211,212,218,
  217,193,194,180,196,197,181,182,188,187,202,203,185,205,206,207,
  208,209,210,189,190,184,183,215,216,192,191,219,220,222,221,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,243,242,244,245,246,247,248,249,250,251,252,253,254,255
};


static unsigned char flipchars[256]={
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 25, 24, 26, 27, 28, 29, 31, 30,
   32,173, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62,168,
   64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
   80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167, 63,169,170,171,172, 33,174,175,
  176,177,178,179,180,181,182,189,190,185,186,188,187,183,184,217,
  218,194,193,195,196,197,198,199,201,200,203,202,204,205,206,209,
  210,207,208,214,213,212,211,215,216,191,192,219,223,221,222,220,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255,
};


static void
mirrorblock()
{
  int i,j,k;

  for(i=by1;i<=by2;i++){
    for(j=bx1,k=bx2;j<k;j++,k--){
      cell_t *c1=&(editbuffer[i*originalwidth+j]);
      cell_t *c2=&(editbuffer[i*originalwidth+k]);
      cell_t  tmp;
      memcpy(&tmp,c1,sizeof(cell_t));
      memcpy(c1,c2,sizeof(cell_t));
      memcpy(c2,&tmp,sizeof(cell_t));
      c1->attrs=(c1->attrs&~0xff)|(mirrorchars[c1->attrs&0xff]);
      c2->attrs=(c2->attrs&~0xff)|(mirrorchars[c2->attrs&0xff]);
    }
  }
}


static void
flipblock()
{
  int i,j,k;

  for(i=bx1;i<=bx2;i++){
    for(j=by1,k=by2;j<k;j++,k--){
      cell_t *c1=&(editbuffer[j*originalwidth+i]);
      cell_t *c2=&(editbuffer[k*originalwidth+i]);
      cell_t  tmp;
      memcpy(&tmp,c1,sizeof(cell_t));
      memcpy(c1,c2,sizeof(cell_t));
      memcpy(c2,&tmp,sizeof(cell_t));
      c1->attrs=(c1->attrs&~0xff)|(flipchars[c1->attrs&0xff]);
      c2->attrs=(c2->attrs&~0xff)|(flipchars[c2->attrs&0xff]);
    }
  }
}


void
eraseblock()
{
  int i,j;
  for(i=by1;i<=by2;i++)for(j=bx1;j<=bx2;j++)
    editbuffer[i*originalwidth+j].attrs=(DEFAULT_ATTR<<8)|(DEFAULT_CHAR);
}



static void
fillblock()
{
  int mask=0, i,j, c, ok=0;

  strcpy(statusline,"[F]oreground,Bac[K]ground,[B]oth,[C]haracter,[A]ll (colour+char): ");
  status();

  while(!ok){
    ok=1;
    switch(c=mygetchar()) {
    case 'F':
    case 'f':
      mask=0xf00;
      break;
    case 'K':
    case 'k':
      mask=0xf000;
      break;
    case 'B':
    case 'b':
      mask=0xff00;
      break;
    case 'C':
    case 'c':
      mask=0x00ff;
      break;
    case 'A':
    case 'a':
      mask=0xffff;
      break;
    default:
      beep();
      ok=0;
    }
  }

  if(mask&1){			/* Do we need a character? */
    strcpy(statusline,"Press character to fill block with, [Meta-C] for last character: ");
    status();
    
    for(;;){
      switch(c=mygetchar()){
      case KEY_F(1):		/* F1 */
      case KEY_F(2):		/* F2 */
      case KEY_F(3):		/* F3 */
      case KEY_F(4):		/* F4 */
      case KEY_F(5):		/* F5 */
      case KEY_F(6):		/* F6 */
      case KEY_F(7):		/* F7 */
      case KEY_F(8):		/* F8 */
      case KEY_F(9):		/* F9 */
      case KEY_F(10):		/* F10 */
	c=graphchars[chrset][c-KEY_F(1)];
	break;
      case 27:			/* Kludge: meta-[anything] is lastchar */
	c=lastchar;
	break;
      }
      if(c<255)break;
      else beep();
    }
  }

  for(i=by1;i<=by2;i++) for(j=bx1;j<=bx2;j++){
    editbuffer[i*originalwidth+j].attrs&=~mask;
    editbuffer[i*originalwidth+j].attrs|=mask&((attr<<8)|(c&0xff));
  }
}


static void
startcopy()
{
  int i,j,pos;
  blockbuffer=(cell_t*)malloc(sizeof(cell_t)*(bx2-bx1+1)*(by2-by1+1));
  for(pos=0,i=by1;i<=by2;i++){
    for(j=bx1;j<=bx2;j++,pos++){
      memcpy(&blockbuffer[pos],&editbuffer[i*originalwidth+j],sizeof(cell_t));
    }
  }
  
  x=bx1;
  y=by1;
  
  strcpy(statusline,"[S]tamp, [F]ront, [B]ack, [T]ransparent, [Enter]=done, [Ctrl-C]=abort.");
  block=3;

  dumpscreen();
  status();
}



static void
startmove(){
  startcopy();
  block=4;
  dumpscreen();
  status();
  eraseblock();
}



void
stampblock()
{
  int i,j,pos;
  
  for(pos=0,i=by1;i<=by2;i++){
    for(j=bx1;j<=bx2;j++,pos++){
      int c=blockbuffer[pos].attrs;
      
      cgaattr(blockbuffer[pos].attrs>>8);
      switch(dragtype){
      case 0:		/* Front, opaque */
	editbuffer[i*originalwidth+j].attrs=c;
	break;
      case 1:		/* Front, transparent */
	if((editbuffer[i*originalwidth+j].attrs&0xff)==32)
	  editbuffer[i*originalwidth+j].attrs=c;
	break;
      case 2:		/* Back */
	if((c&0xff)!=32)editbuffer[i*originalwidth+j].attrs=c;
	break;
      }
    }
  }
}


void
leaveblock()
{
  statusline[0]=0;
  stampblock();
  free(blockbuffer);
  blockbuffer=NULL;
  block=0;
  lx1=bx1;
  lx2=bx2;
  ly1=by1;
  ly2=by2;
}


void
abortblock()
{
  statusline[0]=0;
  bx1=lx1;
  bx2=lx2;
  by1=ly1;
  by2=ly2;
  stampblock();
  free(blockbuffer);
  blockbuffer=NULL;
  block=0;
  dumpscreen();
  status();
}



void
doblock()
{
  int c;
  int done=0;

  /* Sort the block corner co-ordinates */
  if(bx1>x){
    bx2=bx1;
    bx1=x;
  } else bx2=x;
  if(by1>y){
    by2=by1;
    by1=y;
  } else by2=y;

  /* Copy the new block to the last block register */
  lx1=bx1;
  lx2=bx2;
  ly1=by1;
  ly2=by2;
  block=0;

  strcpy(statusline,"[S]ave,[L]oad,Mirror-[X],Mirror-[Y],[M]ove,[C]opy,[F]ill,[E]rase,[Ctrl-C]=Abort: ");
  status();
  while(!done){
    c=mygetchar();
    
    switch(c) {
    case 13:
      done=1;			/* Exit */
      break;
      
    case 'X':			/* Mirror block */
    case 'x': 
      mirrorblock();
      done=1;
      break;

    case 'y':
    case 'Y':			/* Invert (flip) block */
      flipblock();
      done=1;
      break;
      
    case 'E':			/* Erase block */
    case 'e':
      eraseblock();
      done=1;
      break;

    case 'F':			/* Fill block */
    case 'f':
      fillblock();
      done=1;
      break;

    case 'c':			/* Copy block */
    case 'C':
      startcopy();
      done=1;
      return;
      
    case 'm':			/* Move block */
    case 'M':
      startmove();
      done=1;
      return;

    case 'S':			/* Save block */
    case 's':
      savefile(1); 
      done=1;
      break;

    case 'L':			/* Load block */
    case 'l':
      loadfile(1); 
      done=1;
      break;

    default:
      beep();
      strcpy(statusline,"[S]ave,[L]oad,Mirror-[X],Mirror-[Y],[M]ove,[C]opy,[F]ill,[E]rase,[Ctrl-C]=Abort: ");
      status();
      break;
    }
    
    strcpy(statusline,"");
    dumpscreen();
    status();
  }

}
