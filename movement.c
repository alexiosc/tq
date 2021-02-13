#include "tq.h"


void
goleft()
{
  if(x>0){
    newdir=3;
    if(linedraw) dolinedraw();
    
    x--;
  } else if(y>0){
    x=originalwidth-1;
    goup();
  }

  if(!linedraw && attrpaint) doattr();

  if(block>1)dumpscreen();
  status();
}



void
goright()
{
  if(x<(originalwidth-1)){
    newdir=4;
    if(linedraw)dolinedraw();
    x++;
  } else {
    x=0;
    godown();
  }
  if(!linedraw && attrpaint) doattr();
  if(block>1)dumpscreen();
  status();
}


void
godown()
{
  if(y<(MAXLINES-1)){
    newdir=2;
    if(linedraw) dolinedraw();

    y++;

    if((y-starty)>=(rows-2))scrolldown(slowscroll);
  }

  if(!linedraw && attrpaint) doattr();
  if(block>1)dumpscreen();
  status();
}


void
goup()
{
  if(y>0){
    newdir=1;
    if(linedraw) dolinedraw();
      
    y--;
    if(y<starty)scrollup(slowscroll);
    
    if(!linedraw && attrpaint) doattr();
    
  }
  if(block>1)dumpscreen();
  status();
}


void scrollup(int bylines)
{
  starty=max(0,starty-bylines);
  dumpscreen();
}


void scrolldown(int bylines)
{
  starty=min(starty+bylines,MAXLINES-(rows-2));
  dumpscreen();
}


void
pageup()
{
  y=max(y-rows/2,0);
  scrollup(rows/2);
}



void
pagedown()
{
  y=min(y+rows/2,MAXLINES-1);
  scrolldown(rows/2);
}
























void
ansiend()
{
}



void
ansihome()
{
}
