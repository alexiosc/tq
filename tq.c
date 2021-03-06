/*****************************************************************************\
 **                                                                         **
 **  FILE:     tq.c                                                         **
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
 * $Id: tq.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $
 *
 * $Log: tq.c,v $
 * Revision 0.1  1999/05/27 19:43:16  alexios
 * Initial revision, as adapted from Duh Draw.
 *
 *
 */


#ifndef RCS_VER 
#define RCS_VER "$Id: tq.c,v 0.1 1999/05/27 19:43:16 alexios Exp bbs $"
#endif


/* Original copyright notice follows: */

/*****************************************************************************
 *                                  Duh DRAW 
 *                                  for Linux
 *                          (c) Copyleft February 1996
 *                                 Ben Fowler
 *
 ****************************************************************************/


#include "tq.h"

int x=0;			/* X co-ordinate (zero-based) */
int y=0;			/* Y co-ordinate (zero-based) */
int starty=0;			/* line shown on top of screen */

unsigned int attr=DEFAULT_ATTR;	/* Current draw attribute */
char         *statusline;	/* Status line string */
char          filename[256];	/* Name of file being edited */

cell_t        *editbuffer;	/* Edit buffer (attr + char space) */
cell_t        *blockbuffer=NULL;/* Temporary Block buffer */
int            buffersize;	/* Size of the buffer in chars */
int            originalwidth;	/* Number of columns at start of program. */

int            attrpaint=0;	/* Attribute paint mode */
int            block=0;		/* Block mode */
int            linedraw=0;	/* Line drawing mode */
int            insert=0;	/* Insert mode */
int            chrset=0;	/* F-key character set */
int            changed=0;	/* File changed since last save */
int            olddir=0;	/* Old line drawing direction */
int            newdir=0;	/* New line drawing direction */
int            lastchar=32;	/* Last character typed */
int            dragtype=0;	/* Front/Back/Transparent */
int            cancel=0;	/* Just canceled something */

int            bx1,bx2,by1,by2;	/* Current block */
int            lx1,lx2,ly1,ly2;	/* Last block */

int            numlines=0;	/* Number of lines in drawing */
int            numcolumns=0;	/* Number of columns in drawing */


/* The graphic characters used for the F-keys */

int graphchars [15][11] = {
  {218,191,192,217,196,179,195,180,193,194,197}, /* single frame */
  {201,187,200,188,205,186,204,185,202,203,206}, /* double frame */
  {214,183,211,189,196,186,199,182,208,210,215}, /* double vert  */
  {213,184,212,190,205,179,198,181,207,209,216}, /* double horiz */
  {176,177,178,219,220,223,221,222, 22,254, -1}, /* solid blocks */     
  {197,206,215,216,  1,  2,  3,  4,  5,  6, -1},
  { 16, 17, 18, 19, 21, 23, 25, 29, 30, 31, -1},
  { 28,168,127,128,129,130,131,132,133,134, -1},
  {135,136,137,138,139,140,141,142,143,144, -1},
  {145,146,147,148,149,150,151,152,153,154, -1},
  {155,156,157,158,159,160,161,162,163,164, -1},
  {165,166,167,169,170,171,172,173,174,175, -1},             
  {224,225,226,227,228,229,230,231,232,233, -1},
  {234,235,236,237,238,239,240,241,242,243, -1},
  {244,245,246,247,248,249,250,251,252,253, -1}
};


/* All combinations of old/newdir as frame modifications for line
   drawing. Sequence, up, down, left, right. Most significant bit pair
   represents old direction, least significant is new direction.

*/

int modframe [16][11] = {
  { 6, 7, 6, 7,10, 5, 6, 7,10,10,10},	/* 0000 up -> up */
  { 6, 7, 6, 7, 9, 5, 6, 7,10, 9,10},	/* 0001 up -> down */
  { 9, 1,10, 7, 9, 7,10, 7,10, 9,10},	/* 0010 up -> left */
  { 0, 9, 6,10, 9, 6, 6,10,10, 9,10},	/* 0011 up -> right */
  { 6, 7, 6, 7, 8, 5, 6, 7, 8,10,10},	/* 0100 down -> up */
  { 6, 7, 6, 7,10, 5, 6, 7,10,10,10},	/* 0101 down -> down */
  {10, 7, 8, 3, 8, 7,10, 7, 8,10,10},	/* 0110 down -> left */
  { 6,10, 2, 8, 8, 6, 6,10, 8,10,10},	/* 0111 down -> right */
  { 6,10, 2, 8, 8, 6, 6,10, 8,10,10},	/* 1000 left -> up */
  { 0, 9, 6,10, 9, 6, 6,10,10, 9,10},   /* 1001 left -> down */
  { 9, 9, 8, 8, 4,10,10,10, 8, 9,10},	/* 1010 left -> left */
  { 9, 9, 8, 8, 4, 6,10,10, 8, 9,10},	/* 1011 left -> right */
  {10, 7, 8, 3, 8, 7,10, 7, 8,10,10},	/* 1100 right -> up */
  { 9, 1,10, 7, 9, 7,10, 7,10, 9,10},	/* 1101 right -> down */
  { 9, 9, 8, 8, 4, 7,10, 7, 8, 9,10},	/* 1110 right -> left */
  { 9, 9, 8, 8, 4,10,10,10, 8, 9,10}	/* 1111 right -> right */
};


int main(int argc,char *argv[])
{
  init(argc,argv);
  editloop();
  clear();
  refresh();
  return 0;
}
