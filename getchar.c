/*****************************************************************************\
 **                                                                         **
 **  FILE:     getchar.c                                                    **
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
 * $Id: getchar.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $
 *
 * $Log: getchar.c,v $
 * Revision 0.1  1999/05/27 19:43:16  alexios
 * Initial revision, as adapted from Duh Draw.
 *
 *
 */


#ifndef RCS_VER 
#define RCS_VER "$Id: getchar.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $"
#endif


#include "tq.h"



int mygetchar()
{
  int i;

  cursor_on();
  i=getch();
  if(cancel){			/* Reset the cancel condition */
    statusline[0]=0;
    cancel=0;
  }
  cursor_off();
  return i;
}



int mygetline(char *input, int maxlen)
{
  int x,y,pos=0,c;
  char tmp[255];

  cgaattr(0x7);
  maxlen=min(maxlen,255);
  strcpy(tmp,input);
  pos=strlen(tmp);
  getyx(stdscr,y,x);		/* Macro call with side-effects: y and x */

  for(;;){
    mvprintw(y,x,"%s",tmp);	/* Display line */
    clrtoeol();
    move(y,x+pos);		/* Go to the right place */

    switch(c=mygetchar()) {	/* Process keyboard input */

    case 13:			/* Enter: done */
    case 10:
    case KEY_ENTER:
      strcpy(input,tmp);
      return 0;
      
    case CTRL('X'):		/* ^X deletes the line */
    case KEY_DL:
    case KEY_CLEAR:
      tmp[0]=0;
      continue;

    case CTRL('K'):		/* ^K deletes to end of line, a la Emacs */
      tmp[pos]=0;
      continue;
      
    case CTRL('R'):		/* ^R restores the line to its default */
      strcpy(tmp,input);
      continue;
      
    case CTRL('A'):		/* ^A goes to the beginning of the line */
      pos=0;
      continue;
      
    case CTRL('E'):		/* ^E goes to the end of the line */
      pos=strlen(tmp);
      continue;
      
    case CTRL('B'):		/* ^B moves back (left) */
    case KEY_LEFT:
      if(pos)pos--;
      continue;
      
    case CTRL('F'):		/* ^F moves forwards (right) */
    case KEY_RIGHT:
      if(pos<strlen(tmp))pos++;
      continue;
      
    case CTRL('D'):		/* ^D and Del delete characters */
    case KEY_DC:
      if(tmp[pos]){		/* Can't be at the end of the line */
	strcpy(&tmp[pos],&tmp[pos+1]);
      }
      continue;

    case CTRL('H'):		/* Backspace and delete do so :-) */
    case KEY_BACKSPACE:
    case 127:
      if(pos>0){
	strcpy(&tmp[pos-1],&tmp[pos]);
	pos--;
      }
      continue;
	
    default:
      if(c>0xff || strlen(tmp)>maxlen){
	beep();
	continue;
      } else {
	char tmp2[256];
	strcpy(tmp2,&tmp[pos]);
	sprintf(&tmp[pos],"%c%s",c,tmp2);
	pos++;
      }
    }
  }
}





