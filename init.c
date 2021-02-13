/*****************************************************************************\
 **                                                                         **
 **  FILE:     init.h                                                       **
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
 * $Id: init.c,v 0.2 1999/05/27 19:56:19 alexios Exp bbs $
 *
 * $Log: init.c,v $
 * Revision 0.2  1999/05/27 19:56:19  alexios
 * Added code to display the version number on the welcome
 * screen.
 *
 * Revision 0.1  1999/05/27 19:43:16  alexios
 * Initial revision, as adapted from Duh Draw.
 *
 *
 */


#ifndef RCS_VER 
#define RCS_VER "$Id: init.c,v 0.2 1999/05/27 19:56:19 alexios Exp bbs $"
#endif


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/kd.h>
#include <sys/ioctl.h>

#include "tq.h"
#include "image_intro.h"



static struct termios oldttyflags;
static int            oldmetaflag;


void
storetty()
{
  struct termios newttyflags;
  tcgetattr(0,&oldttyflags);
  memcpy(&newttyflags,&oldttyflags,sizeof(newttyflags));
  ioctl(0,KDGKBMETA,&oldmetaflag);
  ioctl(0,KDSKBMETA,K_ESCPREFIX);
  newttyflags.c_cc[VINTR]=CTRL('C');
  newttyflags.c_cc[VQUIT]=0;
  newttyflags.c_cc[VERASE]=0;
  newttyflags.c_cc[VKILL]=0;
  newttyflags.c_cc[VEOF]=0;
  newttyflags.c_cc[VSUSP]=0;
  tcsetattr(0,TCSANOW,&newttyflags);
  tcsetattr(1,TCSANOW,&newttyflags);	/* All three are the same, anyway */
  tcsetattr(2,TCSANOW,&newttyflags);	/* But it won't hurt doing all of them */
}



void
restoretty()
{
  ioctl(0,KDSKBMETA,oldmetaflag);
  (void) tcsetattr(0,TCSANOW,&oldttyflags);
}



void
graceful_exit()
{
  restoretty();
}



static void
setgeometry()
{
  originalwidth=COLS;
  statusline=(char*)malloc(cols+1);
  buffersize=MAXLINES*cols;
  editbuffer=(cell_t*)malloc(sizeof(cell_t)*buffersize);
  x=y=starty=0;
}


static void
intro()
{
  int c,x,y;
  show_bin(screen_intro);
  if(LINES>26){
    cgaattr(0x47);
    for(x=1;x<=51;x++)for(y=0;y<(LINES-25)/2;y++)mvaddch(y,x+(COLS-80)/2,' ');
  }
  if(LINES>25){
    cgaattr(0x17);
    for(x=19;x<43;x++)for(y=25;y<LINES;y++)mvaddch(y+(LINES-25)/2,x+(COLS-80)/2,' ');
  }
  if(COLS>80){
    cgaattr(0x27);
    for(x=79;x<COLS;x++)for(y=6;y<22;y++)mvaddch(y+(LINES-25)/2,x+(COLS-80)/2,' ');
  }
  cgaattr(0xb);
  mvprintw((LINES-25)/2+24,
	   (COLS-80)/2+11,
	   "%d.%02d",
	   MAJOR_VERSION,MINOR_VERSION);
  move(LINES-1,COLS-1);
  c=mygetchar();
}




void init(int argc, char **argv)
{
  storetty();
  atexit(graceful_exit);
  initscr();

  setgeometry();
  
  noecho();			/* shuttoff tty echoing */
  nonl();			/* don't wait for carriage return */
  cbreak();
  /*raw();*/
  keypad(stdscr,TRUE);		/* Enable processing of function keys */
  scrollok(stdscr,FALSE);
  leaveok(stdscr,FALSE);

  meta(stdscr,TRUE);

  if(has_colors()){
    int i,j;
    int cols[8]={COLOR_BLACK,COLOR_BLUE,COLOR_GREEN,COLOR_CYAN,
		 COLOR_RED,COLOR_MAGENTA,COLOR_YELLOW,COLOR_WHITE};
  
    start_color();
    for(i=0;i<8;i++)for(j=0;j<8;j++)init_pair(i+j*8,cols[i],cols[j]);
  }
  
  clr(); /* clear buffer and screen */

  strcpy(filename,"");
  if(argc>1){
    if(strcmp(argv[1],"-help")==0) dohelp();
    else {
      strcpy(filename,argv[1]);
      readfile();
    }
  }
  
  intro();

  signal(SIGINT,SIG_IGN);
  signal(SIGQUIT,SIG_IGN);
  signal(SIGABRT,SIG_IGN);
  signal(SIGSTOP,SIG_IGN);
  signal(SIGUSR1,SIG_IGN);
  signal(SIGUSR2,SIG_IGN);

  signal(SIGINT,int_handler);
  signal(SIGQUIT,int_handler);
  signal(SIGABRT,int_handler);
  signal(SIGSTOP,int_handler);
  signal(SIGUSR1,int_handler);
  signal(SIGUSR2,int_handler);

  fixvars();
  dumpscreen();
  status();
}
