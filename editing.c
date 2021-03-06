#include <string.h>
#include "tq.h"

void dochar(int c)
{
  int lx=originalwidth-2;
  
  if(insert){
    while(lx>=x){
      editbuffer[(y*originalwidth)+lx+1]=editbuffer[(y*originalwidth)+lx];
      lx--;
    }
  }
  
  editbuffer[(y*originalwidth)+x].attrs=(c&255)|(attr<<8);
  
  if(x<(min(originalwidth,cols)-1)){
    x++;
    status();
  } else {
    newline();
  }
  /*if(ay>alastline)alastline=ay;*/
  changed=1;
  dumpline(y);
  lastchar=c;
  status();
}


void clr()
{
  register int pos;
  register unsigned int clear=((DEFAULT_ATTR<<8)|DEFAULT_CHAR);

  for(pos=0;pos<buffersize;pos++)editbuffer[pos].attrs=clear;
  
  attr=DEFAULT_ATTR;
  x=y=starty=0;
}


void erasedrawing()
{
  if(yesno("Erase entire drawing?",0)){
    clr();
    x=y=starty=0;
    dumpscreen();
  }
  status();
}


void
backspace()
{
  if(x>0) {
    if(insert){
      register int i, ofs=(y*originalwidth)+x;
      x--;
      for(i=x+1;i<originalwidth;i++,ofs++)editbuffer[ofs-1]=editbuffer[ofs];
      editbuffer[ofs].attrs=(attr<<8)|DEFAULT_CHAR;
    } else {
      x--;
      editbuffer[(y*originalwidth)+x].attrs=(attr<<8)|DEFAULT_CHAR;
    }
    dumpline(y);
    status();
  }
}


void
deletechar()
{
  register int i, ofs=(y*originalwidth)+x+1;
  for(i=x+1;i<originalwidth;i++,ofs++)editbuffer[ofs-1]=editbuffer[ofs];
  editbuffer[ofs].attrs=(DEFAULT_ATTR<<8)|DEFAULT_CHAR;
  dumpline(y);
  status();
}


void newline()
{
  /*
  if(block==3){
    moveblock();
    } else {*/
    x=0;
    if(y<MAXLINES-1)godown();
    else status();
    /*}*/
}


void dolinedraw()
{
  int c=editbuffer[y*originalwidth+x].attrs&0xff;
  int i;

  if(olddir<0)olddir=newdir;

  for(i=0;i<11;i++){
    if(graphchars[chrset][i]==c)break;
  }

  if(i==11){
    switch(newdir) {
    case 1: /* crs up */
      c=graphchars[chrset][5];
      if(olddir==3) c=graphchars[chrset][2];
      if(olddir==4) c=graphchars[chrset][3];
      break;
      
    case 2: /* crs down */
      c=graphchars[chrset][5];
      if(olddir==3) c=graphchars[chrset][0];
      if(olddir==4) c=graphchars[chrset][1];
      break;   
    
    case 3: /* crs left */
      c=graphchars[chrset][4];
      if(olddir==1) c=graphchars[chrset][1];
      if(olddir==2) c=graphchars[chrset][3];
      break;
      
    case 4 : /* crs right */
      c=graphchars[chrset][4];
      if(olddir==1) c=graphchars[chrset][0];
      if(olddir==2) c=graphchars[chrset][2];
      break;
    }
  } else {
    int k=(((olddir-1)&0x3)<<2)|((newdir-1)&0x3);
    int j=modframe[k][i];
    c=graphchars[chrset][j];
    /*    mvprintw(1,1,"(dir=%d%d%d%d (%d),i=%d,j=%d)",
	  (k&8)!=0,(k&4)!=0,(k&2)!=0,(k&1)!=0,k,i,j);*/
  }
    
  editbuffer[y*originalwidth+x].attrs=c|(attr<<8);
  dumpline(y);
  status();
  olddir=newdir;
  changed=1;
}


void insertline()
{
  int i;
  memmove(&(editbuffer[(y+1)*originalwidth]),
	  &(editbuffer[y*originalwidth]),
	  (MAXLINES-y)*originalwidth*sizeof(cell_t));
  for(i=0;i<80;i++) {
    editbuffer[y*originalwidth+i].attrs=(DEFAULT_ATTR<<8)|DEFAULT_CHAR;
  }
  /*insertln();*/
  dumpscreen();
  /*for(i=y-starty;(y-starty)<rows;i++)dumpline(i);*/
  status();
}


void deleteline()
{
  int i;
  memmove(&(editbuffer[y*originalwidth]),
	  &(editbuffer[(y+1)*originalwidth]),
	  (MAXLINES-y)*originalwidth*sizeof(cell_t));
  for(i=0;i<originalwidth;i++){
    editbuffer[(MAXLINES-1)*originalwidth+i].attrs=
      (DEFAULT_ATTR<<8)|DEFAULT_CHAR;
  }
  /*deleteln();*/
  dumpscreen();
  status();
}


void deletecolumn()
{
  int i;

  for(i=0;i<MAXLINES;i++)memmove(&editbuffer[i*originalwidth+x],
				 &editbuffer[i*originalwidth+x+1],
				 (originalwidth-(x+1))*sizeof(cell_t));
  for(i=0;i<MAXLINES;i++){
    editbuffer[(i+1)*originalwidth-1].attrs=(DEFAULT_ATTR<<8)|DEFAULT_CHAR;
  }
  dumpscreen();
  status();
}


void insertcolumn()
{
  int i;
  if(x==originalwidth)return;

  for(i=0;i<MAXLINES;i++)memmove(&editbuffer[i*originalwidth+x+1],
				 &editbuffer[i*originalwidth+x],
				 (originalwidth-(x+1))*sizeof(cell_t));
  for(i=0;i<MAXLINES;i++){
    editbuffer[i*originalwidth+x].attrs=(DEFAULT_ATTR<<8)|DEFAULT_CHAR;
  }
  dumpscreen();
  status();
}


void
doattr()
{
  editbuffer[y*originalwidth+x].attrs&=0xff;
  editbuffer[y*originalwidth+x].attrs|=attr<<8;
  dumpline(y);
}

void
moveblock()
{
}
