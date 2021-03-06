#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include <string.h>
#include "tq.h"

static int _cga2ansi[8]={0,4,2,6,1,5,3,7};
static int opt_clear=1;
static int opt_home=1;


#define cga2ansi(c) (_cga2ansi[c%8])


static char *
encode_attr(int a)
{
  static char directive[256];
  char tmp[16];
  strcpy(directive,"0");

  if((a&7)!=7){
    sprintf(tmp,";%d",30+cga2ansi(a&0x7));
    strcat(directive,tmp);
  }

  if(((a&0x70)>>4)!=0){
    sprintf(tmp,";%d",40+cga2ansi((a&0x70)>>4));
    strcat(directive,tmp);
  }
  if(a&0x8)strcat(directive,";1");
  if(a&0x80)strcat(directive,";5");
  return directive;
}


char *
optimise_spaces(int spaces, int attr)
{
  static char retval[4096];

  sprintf(retval,"\e[%dC",spaces);
  if(attr&0x70)return retval;
  if(spaces<=strlen(retval)){
    memset(retval,' ',spaces);
    retval[spaces]=0;
  }

  return retval;
}


static char *
optimise_colour_changes(int oldattr, int a)
{
  static char esc[256];
  int firstparm=1;
  int old_bg=30+cga2ansi(oldattr&0x7);
  int old_fg=40+cga2ansi((oldattr&0x70)>>4);
  int old_blink=oldattr&0x80;
  int old_bold=oldattr&0x8;
  int new_bg=30+cga2ansi(a&0x7);
  int new_fg=40+cga2ansi((a&0x70)>>4);
  int new_blink=a&0x80;
  int new_bold=a&0x8;
  
  esc[0]=0;

  /* That braindead PC ANSI doesn't support turning off attributes,
     which forces us to reset using '0' and rebuild the entire
     attribute set from scratch, minus the attribute we want to turn
     off. This probably makes sense to IBM suits. */

  if(new_bold==0 && new_blink==0 && new_bg==0 && new_fg==7){
    strcpy(esc,"0");
  } else if((old_bold!=0 && new_bold==0)||
     (old_blink!=0 && new_blink==0)){
    strcpy(esc,encode_attr(a));
  } else {
    if(old_bg!=new_bg){
      char tmp[16];
      if(!firstparm)strcat(esc,";");
      sprintf(tmp,"%d",new_bg);
      strcat(esc,tmp);
      firstparm=0;
    }
    if(old_fg!=new_fg){
      char tmp[16];
      if(!firstparm)strcat(esc,";");
      sprintf(tmp,"%d",new_fg);
      strcat(esc,tmp);
      firstparm=0;
    }
    if(old_bold!=new_bold){ /* We only get here if new_bold!=0 */
      if(!firstparm)strcat(esc,";1");
      else strcat(esc,"1");
      firstparm=0;
    }
    if(old_blink!=new_blink){ /* We only get here if new_blink!=0 */
      if(!firstparm)strcat(esc,";5");
      else strcat(esc,"5");
      firstparm=0;
    }
  }
  return esc;
}



void ansi_save(int blockmode)
{
  FILE *fp;
  int x,y,oldattr,a=0,c=0;
  int spaces;
  char buf[256], colour_change[256];
  int x1,x2,y1,y2;

  /* Ask for options */
  
  find_area();
  if(!blockmode){
    sprintf(buf,"Drawing size is %d x %d. Clear screen?",numcolumns,numlines);
  } else {
    sprintf(buf,"Drawing size is %d x %d. Clear screen?",bx2-bx1+1,by2-by1+1);
  }
  opt_clear=yesno(buf,opt_clear);
  opt_home=yesno("Home cursor?",opt_home);
  #if 0
  strcpy(statusline,"{Intended screen width ([-1{=none) ?} ");
  status();
  for(;;){
    move(LINES-2,strlen(statusline)-4);
    strcpy(buf,"80");
    if(mygetline(buf,COLS-strlen(statusline)+3)<0) goto end_save;
    if(!atoi(buf))beep();
    else break;
  }
  #endif

  /* Open the file */
  
  if((fp=fopen(filename,"w"))==NULL){
    show_ioerror("Save");
    goto end_save;
  }

  /* Begin the output file */

  fprintf(fp,"\e[0m");
  if(opt_home)fprintf(fp,"\e[H");
  if(opt_clear)fprintf(fp,"\e[2J");

  /* Output the drawing, optimising attribute directives and cursor motion */  

  oldattr=DEFAULT_ATTR;
  spaces=0;

  if(!blockmode){
    x1=y1=0;
    x2=originalwidth;
    y2=numlines;
  } else {
    x1=bx1;
    x2=bx2;
    y1=by1;
    y2=by2;
  }

  for(y=y1;y<y2;y++){
    spaces=0;
    for(x=x1;x<x2;x++){
      a=editbuffer[y*originalwidth+x].attrs>>8;
      c=editbuffer[y*originalwidth+x].attrs&0xff;

      if(a==DEFAULT_ATTR && c==DEFAULT_CHAR) spaces++;
      else {

	/* Optimise horizontal spacing */
	
	if(spaces>0){
	  fprintf(fp,"%s",optimise_spaces(spaces,oldattr));
	  spaces=0;
	}
	
	/* Optimise attribute changes */

	if(a!=oldattr){
	  sprintf(colour_change,"\e[%sm",optimise_colour_changes(oldattr,a));
	  fprintf(fp,"%s",colour_change);
	} else colour_change[0]=0;
	oldattr=a;

	/* And output the character */

	/*fprintf(fp,"%s(%x)",colour_change,c);*/
	fputc(c,fp);
      }
    }

    /* Scrolling causes the new line to appear in the colour of the
       current background -- we most probably don't want this. We
       could take advantage of it, but there are too many problems to
       solve and the returns aren't worth it. We strip the 8th bit,
       too -- some setups use it to allow 16 background colours, so
       we'll consider it a background bit. This won't hurt setups
       where the 8th bit denotes blinking. */
    
    if(oldattr&0xf0){
      fprintf(fp,"\e[0m");
      oldattr=DEFAULT_ATTR;
    }


    /* DOS (and quite a few BBS programs out there) has Ye Evil Right
       Edge Feature, whereupon wrapping is *always* caused by a
       character printed in the 80th column (or whatever). Therefore,
       if the cursor is on the rightmost column as specified by the
       user in opt_len, we will not issue a newline since the cursor
       is assumed to have wrapped around. This does not break
       terminals clever enough to wrap around only when
       needed. Strategy: if the last character is anything but a space
       (not output but used to increment the spaces counter), then DOS
       has just wrapped around. */
    
    if(!blockmode){
      if(spaces)fprintf(fp,"%c%c",13,10);
    } else {
      if(spaces||((x2-x1+1)<originalwidth))fprintf(fp,"%c%c",13,10);
    }
  }

  fclose(fp);
    
 end_save:
  statusline[0]=0;
  status();
}



void
ansi_load(int blockmode)
{
  if(blockmode)eraseblock();
  else clr();
  load_and_interpret(blockmode);
  dumpscreen();
  status();
}
