/*****************************************************************************\
 **                                                                         **
 **  FILE:     files.c                                                      **
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
 * $Id: files.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $
 *
 * $Log: files.c,v $
 * Revision 0.1  1999/05/27 19:43:16  alexios
 * Initial revision, as adapted from Duh Draw.
 *
 *
 */


#ifndef RCS_VER 
#define RCS_VER "$Id: files.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $"
#endif


#include <ctype.h>
#include <sys/errno.h>
#include <string.h>

#include "tq.h"

static char *save_methods="ASB";


void readfile()
{
  clr();
  ansi_load(0);
}


void savefile(int blocksave)
{
  char *cp;
  int c, method=0;

  /* Ask for filename */

  if(blocksave)strcpy(statusline,"{Filename to save block as:} ");
  else strcpy(statusline,"{Filename to save as:} ");
  status();
  move(LINES-2,strlen(statusline)-2);
  if(mygetline(filename,COLS-strlen(statusline)+1)<0) goto save_abort;
  
  /* Ask for format */

  strcpy(statusline,"Format to save: [A]NSI, A[S]CII, [B]inary, or [Ctrl-C] to abort.");
  status();
  
  for(;;){
    c=toupper(mygetchar());
    if((cp=strchr(save_methods,toupper(c)))!=NULL){
      method=(int)(cp-save_methods);
      break;
    }
    beep();
  }

  switch(method){
  case 1:			/* ASCII save */
    ascii_save(blocksave);
    break;
  case 2:
    bin_save(blocksave);
    break;
  case 0:			/* ANSI save */
  default:			/* Default is also ANSI, just in case */
    ansi_save(blocksave);
  }


  strcpy(statusline,"{File saved. Press any key.}");
  status();
  mygetchar();

 save_abort:
  statusline[0]=0;
  status();
}


void loadfile(int blockload)
{
  char *cp;
  int c, method=0;

  /* Ask for filename */

  if(blockload)strcpy(statusline,"{Filename to load block from:} ");
  else strcpy(statusline,"{Filename to load:} ");
  status();
  move(LINES-2,strlen(statusline)-2);
  if(mygetline(filename,COLS-strlen(statusline)+1)<0) goto load_abort;
  
  /* Ask for format */

  strcpy(statusline,"Format to load: [A]NSI, A[S]CII, [B]inary, or [Ctrl-C] to abort.");
  status();
  
  for(;;){
    c=toupper(mygetchar());
    if((cp=strchr(save_methods,toupper(c)))!=NULL){
      method=(int)(cp-save_methods);
      break;
    }
    beep();
  }

  switch(method){
  case 1:			/* ASCII load */
    ascii_load(blockload);
    break;
  case 2:			/* Binary load */
    bin_load(blockload);
    break;
  case 0:			/* ANSI load */
  default:			/* Default is also ANSI, just in case */
    ansi_load(blockload);
  }

  strcpy(statusline,"{File loaded. Press any key.}");
  status();
  mygetchar();

 load_abort:
  statusline[0]=0;
  status();
}



