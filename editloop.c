/*****************************************************************************\
 **                                                                         **
 **  FILE:     editloop.h                                                   **
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
 * $Id: editloop.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $
 *
 * $Log: editloop.c,v $
 * Revision 0.1  1999/05/27 19:43:16  alexios
 * Initial revision, as adapted from Duh Draw.
 *
 *
 */


#ifndef RCS_VER 
#define RCS_VER "$Id: editloop.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $"
#endif


#include <signal.h>
#include <setjmp.h>

#include "tq.h" 



static sigjmp_buf cancel_point;
static int        metamode=0;


void fixvars()
{
  if(block>=3 && blockbuffer!=NULL) abortblock();
  statusline[0]=0;
  attrpaint=0;
  block=0;
  linedraw=0;
  insert=0;
  metamode=0;
}

void int_handler(int signum)
{
  cancel=1;
  fixvars();
  dumpscreen();
  status();
  siglongjmp(cancel_point,1);	/* Back to the edit loop */
}


void editloop()
{
  int c;
  c=0;

  metamode=0;
  
  for(;;){
    sigsetjmp(cancel_point,1);
    c=mygetchar();
    
    if(metamode){		/* Mark META+key combinations */
      c+=META;
      metamode=0;
    }

    /* First come commands that work regardless of block mode */
    
    switch (c) {
    case CTRL('L'):		/* Redraw the screen */
    case CTRL('R'):
      redraw();
      break;
      
    case CTRL('B'):		/* Left */
    case KEY_LEFT:
      goleft();
      break;
      
    case CTRL('F'):		/* Right */
    case KEY_RIGHT:
      goright();
      break; 
      
    case CTRL('N'):		/* Down */
    case KEY_DOWN:
      godown();
      break;
      
    case CTRL('P'):		/* Up */
    case KEY_UP:
      goup();
      break;
      
    case CTRL('U'):		/* Page up */
    case KEY_PPAGE:
    case KEY_PREVIOUS:
      pageup();
      break;
      
    case CTRL('V'):		/* Page down */
    case KEY_NPAGE:
    case KEY_NEXT:
      pagedown();
      break;

    case KEY_HOME:		/* Beginning of line/text */
    case CTRL('A'):
      if(x>0){
	x=0;
	if(block>1)dumpscreen();
	status();
      } else {
	x=y=starty=0;
	dumpscreen();
	status();
      }
      break;
      
    case KEY_END:		/* Beginning of line/text */
    case CTRL('E'):
      if(x<(originalwidth-1)){
	x=originalwidth-1;
	if(block>1)dumpscreen();
	status();
      } else {
	x=originalwidth-1;
	y=MAXLINES-1;
	starty=MAXLINES-(rows-2);
	dumpscreen();
	status();
      }
      break;
      
    case META+'H':		/* Help screen */
    case META+'h':
    case META+'/':
    case META+'?':
      dohelp();
      break;
    }


    /* Commands that only work in block mode 0 (edit mode) */

    
    if(block==0)switch(c){
    case 27:			/* Meta mode */
      metamode=1;
      break;

    case CTRL('H'):		/* Backspace one character */
    case KEY_BACKSPACE:
      backspace();
      break;
      
    case CTRL('D'):		/* Delete character to the right */
    case KEY_DC:
      deletechar();
      break;
      
    case KEY_IC:		/* Toggle insert mode */
      insert=!insert;
      status();
      break;
      
    case KEY_EIC:		/* Exit insert mode */
      insert=0;
      status();
      break;
      
    case KEY_ENTER:		/* Start a new line */
    case 10:
    case 13:
      newline();
      break;
	
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
      if(!block)
	dochar(graphchars[chrset][c-KEY_F(1)]);
      break;

    case KEY_F(0):		/* F10 on some systems */
      if(!block)
	dochar(graphchars[chrset][9]);
      break;
      
    case META+'1':		/* Change character sets */
    case META+'2':
    case META+'3':
    case META+'4':
    case META+'5':
    case META+'6':
    case META+'7':
    case META+'8':
    case META+'9':
      chrset=c-(META+'1');
      status();
      break;

    case META+'0':
      chrset=10;
      status();
      break;

    case META+'-':
    case META+'_':
      chrset=13;
      status();
      break;

    case META+'+':
    case META+'=':
      chrset=14;
      status();
      break;

    case META+',':		/* Change character set */
    case META+'<':
      if(chrset > 0 )
	chrset--;
      status();
      break;
      
    case META+'.':		/* Change character set */
    case META+'>':
      if(chrset < 14)
	chrset++;
      status();
      break;

    case META+'d':		/* Toggle line drawing mode */
    case META+'D':
      linedraw=!linedraw;
      if(linedraw){
	olddir=-1;
	if(chrset>3)chrset=0;
      }
      status();
      break;
      
    case META+KEY_RIGHT:	/* Increase foreground colour */
    case META+'q':
      {
	int fg=attr;
	attr&=~0xf;
	attr|=(fg+1)&0xf;
	break;
      }
      
    case META+KEY_LEFT:		/* Decrease foreground colour */
    case META+'Q':
      {
	int fg=attr;
	attr&=~0xf;
	attr|=(fg-1)&0xf;
	break;
      }
      
    case META+KEY_UP:		/* Increase background colour */
    case META+'w':
      {
	int bg=attr;
	attr&=~0xf0;
	attr|=(bg+16)&0xf0;
	break;
      }
      
    case META+KEY_DOWN:		/* Decrease background colour */
    case META+'W':
      {
	int bg=attr;
	attr&=~0xf0;
	attr|=(bg-16)&0xf0;
	break;
      }

    case META+'U':		/* Grab background colour */
    case META+'u':
      attr=(editbuffer[y*originalwidth+x].attrs&~0xff)>>8;
      status();
      break;

    case META+'a':		/* Toggle attribute paint */
    case META+'A':
      attrpaint=!attrpaint;
      status();
      break;
      
    case META+'c':		/* Stamp last character */
    case META+'C':
      if(!block)
	dochar(lastchar);
      break;

    case META+'i':		/* Insert line */
    case META+'I':
      insertline();
      break;
      
    case META+'y':		/* Delete (yank) line */
    case META+'Y':
      deleteline();
      break;

    case META+'j':		/* Insert column */
    case META+'J':
      insertcolumn();
      break;
      
    case META+'k':		/* Delete column */ 
    case META+'K':
      deletecolumn();
      break;
      
    case META+'f' :		/* Color selection */
    case META+'F' :		/* Color selection */
      colorsel();
      break;
      
    case META+'b':		/* Block mode */
    case META+'B':
      attrpaint=linedraw=0;
      block=1;
      strcpy(statusline,"Use [L]ast block, [A]ll screen, or move to first corner and press [Space].");
      status();
      break;

    case META+'p':		/* Pick up character */
    case META+'P':
      lastchar=editbuffer[y*originalwidth+x].attrs&0xff;
      break;

    case META+'e':		/* Erase drawing */
    case META+'E':
      erasedrawing();
      break;

    case META+'s':		/* Save file */
    case META+'S':
      savefile(0);
      break;

    case META+'l':		/* Load file */
    case META+'L':
      loadfile(0);
      break;

    case META+'x':		/* Exit */ 
    case META+'X':
      if(yesno("Quit Duh Draw?",0)){
	/*if(changed)
	  savefile(0);*/
	return;
      }
      status();
      break;
    }


    /* Typing etc */

    if(block==1){
      if(c==32){
	bx1=x;
	by1=y;
	block=2;
	strcpy(statusline,"Move to other corner and press [Space].");
	status();
	continue;
      } else if(c=='l'||c=='L'){
	bx1=lx1;
	bx2=lx2;
	x=bx2=lx2;
	y=by2=ly2;
	block=2;
	dumpscreen();
	refresh();
	doblock();
	continue;
      } else if(c=='a'||c=='A'){
	bx1=0;
	by1=0;
	x=bx2=originalwidth-1;
	y=by2=MAXLINES-1;
	block=2;
	dumpscreen();
	refresh();
	doblock();
	continue;
      }
    } else if(block==2 && c==32){
      doblock();
      continue;
    } else if(block>=3){
      if(c==13||c==10||c==KEY_ENTER){
	leaveblock();
	continue;
      } else if(c=='F'||c=='f'){
	dragtype=0;
	dumpscreen();
      } else if(c=='B'||c=='b'){
	dragtype=1;
	dumpscreen();
	continue;
      } else if(c=='T'||c=='t'){
	dragtype=2;
	dumpscreen();
	continue;
      } else if(c=='S'||c=='s'){
	stampblock();
	continue;
      }
    } else if((c>=32)&&(c<256)&&(block==0))dochar(c);
      
    /* All done, update the screen */
    
    status();
    refresh();
  }
}
