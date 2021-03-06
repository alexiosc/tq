#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include <string.h>
#include "tq.h"


#if 0
static int opt_tabsize=8;
static int opt_tabify=1;


static char *
tabify(int spaces)
{
  static char retval[4096];
  int i,pos=0;

  if(opt_tabify && (spaces>opt_tabsize)){
    memset(retval,'\t',spaces/opt_tabsize);
    retval[pos=(spaces/opt_tabsize)]=0;
    spaces=spaces%opt_tabsize;
  }
  for(i=0;i<spaces;i++)retval[pos++]=' ';
  retval[pos]=0;
  return retval;
}
#endif


void ascii_save(int blockmode)
{
  FILE *fp;
  int x,y,c=0;
  int spaces;
  int x1,x2,y1,y2;
#if 0
  char buf[256];

  /* Ask for options */

  find_area();
  if(!blockmode){
    sprintf(buf,"Drawing size is %d x %d. Use tabs in output?",numcolumns,numlines);
  } else {
    sprintf(buf,"Drawing size is %d x %d. Use tabs in output?",bx2-bx1+1,by2-by1+1);
  }
  opt_tabify=yesno(buf,opt_tabify);
  strcpy(statusline,"{Tab width? (no need to change, usually) } ");
  status();
  for(;;){
    move(LINES-2,strlen(statusline)-2);
    sprintf(buf,"%d",opt_tabsize);
    if(mygetline(buf,COLS-strlen(statusline)+1)<0) goto end_save;
    if(atoi(buf)<2)beep();
    else break;
  }
#else
  find_area();
#endif


  /* Open the file */
  
  if((fp=fopen(filename,"w"))==NULL){
    show_ioerror("Save");
    goto end_save;
  }

  
  /* Output the drawing, tabifying if we have to */  
  
  if(!blockmode){
    x1=y1=0;
    x2=originalwidth-1;
    y2=MAXLINES-1;
  } else {
    x1=bx1;
    x2=bx2;
    y1=by1;
    y2=by2;
  }

  for(y=y1;y<=y2;y++){
    spaces=0;
    for(x=x1;x<=x2;x++){
      c=editbuffer[y*originalwidth+x].attrs&0xff;

      if(isspace(c)) spaces++;
      else {
	
	/* Tabify */
	
	if(spaces>0){
	  int i;
	  for(i=0;i<spaces;i++)fputc(32,fp);
#if 0
	  fprintf(fp,"%s",tabify(spaces));
#endif
	  spaces=0;
	}
	
	/* And output the character */
	
	fputc(c,fp);
      }
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


void ascii_load(int blockmode)
{
  FILE *fp;
  int x,y;
  int x1,x2,y1,y2;


  /* Open the file */
  
  if((fp=fopen(filename,"r"))==NULL){
    show_ioerror("Load");
    goto end_load;
  }


  /* Calculate the drawing extents */

  if(!blockmode){
    x1=y1=0;
    x2=originalwidth-1;
    y2=MAXLINES-1;
    clr();
  } else {
    x1=bx1;
    y1=by1;
    x2=bx2;
    y2=by2;
    eraseblock();
  }

  
  /* Input the drawing. */  
  
  x=x1;
  y=y1;
  for(;;){
    unsigned char line[4096], *cp;
    if(!fgets(line,sizeof(line),fp))break;

    if((cp=strchr(line,10))!=NULL)*cp=0;
    if((cp=strchr(line,13))!=NULL)*cp=0;

    for(x=x1,cp=line;x<=x2 && *cp;x++,cp++){
      editbuffer[y*originalwidth+x].attrs=(DEFAULT_ATTR<<8)|(*cp&0xff);
    }

    y++;
    if(y>y2)break;
  }
  
  fclose(fp);

 end_load:
  statusline[0]=0;
  dumpscreen();
  status();
}
