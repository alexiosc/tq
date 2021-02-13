#include "tq.h"


void
cgaattr(unsigned int cga_attr)
{
  int fg=cga_attr&0x07;
  int bg=(cga_attr&0x70)>>4;
  attrset(0);
  if(cga_attr==0x00){
    attron(A_INVIS);
  }
  if(fg==0&&bg==0&&(cga_attr&8)){
    if(cga_attr&0x80)attron(A_BLINK);
    attron(COLOR_PAIR(7)+A_DIM);
  } else {
    if(cga_attr&0x08)attron(A_BOLD);
    if(cga_attr&0x80)attron(A_BLINK);
    attron(COLOR_PAIR((bg*8+fg)));
  }
}


void
setforeback(int b, int fg, int bg)
{
  int bold=fg&0x8;
  fg&=0x7;
  attrset(0);
  if(fg==0&&bg==0&&bold){
    if(b)attron(A_BLINK);
    attron(COLOR_PAIR(7)+A_DIM);
  } else {
    if(fg&0x8)attron(A_BOLD);
    if(b)attron(A_BLINK);
    attron(COLOR_PAIR(bg*8+fg));
  }
}


void
redraw()
{
  dumpscreen();
  clearok(stdscr,TRUE);
  refresh();
  wrefresh(stdscr);
}
