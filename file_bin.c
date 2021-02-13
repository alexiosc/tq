#include <stdio.h>
#include <ctype.h>
#include <sys/errno.h>
#include <string.h>
#include <endian.h>
#include "tq.h"


void bin_save(int blockmode)
{
  FILE *fp;
  int x,y;
  int x1,x2,y1,y2;

  /* Ask for options */

  find_area();



  /* Open the file */
  
  if((fp=fopen(filename,"w"))==NULL){
    show_ioerror("Save");
    goto end_save;
  }


  /* Output the drawing size, horizontal, then vertical */
  
  if(!blockmode){
    x1=y1=0;
    x2=originalwidth-1;
    y2=numlines-1;
  } else {
    x1=bx1;
    x2=bx2;
    y1=by1;
    y2=by2;
  }
  
  {
    unsigned short int x=x2-x1+1;
    unsigned short int y=y2-y1+1;

    /* Make sure these are saved in little endian format, since binary
       files more or less match the IBM PC text framebuffer format. */
    
#if __BYTE_ORDER == __BIG_ENDIAN
    x=((x&0xff)<<8)|(x>>8);
    y=((y&0xff)<<8)|(y>>8);
#endif

    if(fwrite(&x,sizeof(x),1,fp)!=1){
      show_ioerror("Save");
      goto end_save;
    }
    if(fwrite(&y,sizeof(y),1,fp)!=1){
      show_ioerror("Save");
      goto end_save;
    }
  }


  /* Output the drawing */  
  
  for(y=y1;y<=y2;y++){
    for(x=x1;x<=x2;x++){
      unsigned char a[2];
      /* Ensure the order is (attribute,char), regardless of CPU's byte order */
      a[0]=(editbuffer[y*originalwidth+x].attrs >> 8)&0xff;
      a[1]=editbuffer[y*originalwidth+x].attrs&0xff;
      if(fwrite(a,sizeof(unsigned char),2,fp)!=2){
	show_ioerror("Save");
	goto end_save;
      }
    }
  }

  fclose(fp);

 end_save:
  statusline[0]=0;
  status();

}



void bin_load(int blockmode)
{
  FILE *fp;
  int x,y,w,h;
  int x1,x2,y1,y2;

  /* Ask for options */

  find_area();



  /* Open the file */
  
  if((fp=fopen(filename,"r"))==NULL){
    show_ioerror("Load");
    goto end_load;
  }


  /* Read the drawing size, horizontal, then vertical */
  
  {
    unsigned short int x;
    unsigned short int y;

    if(fread(&x,sizeof(x),1,fp)!=1){
      show_ioerror("Load");
      goto end_load;
    }

    if(fread(&y,sizeof(y),1,fp)!=1){
      show_ioerror("Load");
      goto end_load;
    }

    /* These are saved in little endian format. If we're running on a
       big endian, reverse the byte order. */
    
#if __BYTE_ORDER == __BIG_ENDIAN
    w=((x&0xff)<<8)|(x>>8);
    h=((y&0xff)<<8)|(y>>8);
#else
    w=x;
    h=y;
#endif
  }

  /* Calculate the drawing extents */

  if(!blockmode){
    x1=y1=0;
    x2=min(originalwidth-1,w-1);
    y2=min(MAXLINES-1,h-1);
    clr();
  } else {
    x1=bx1;
    y1=by1;
    x2=x1+min(bx2-bx1,w);
    y2=y1+min(by2-by1,h);
    eraseblock();
  }

  sprintf(statusline,
	  "Size is {%d} x {%d}. Press any key to load, [Ctrl-C] to cancel.",
	  x2-x1+1,y2-y1+1);
  status();
  mygetchar();
  
  /* Input the drawing */  
  
  for(y=y1;y<=y2;y++){
    for(x=x1;x<=x2;x++){
      unsigned char a[2];
      if(fread(a,sizeof(unsigned char),2,fp)!=2){
	show_ioerror("Load2");
	goto end_load;
      }
      editbuffer[y*originalwidth+x].attrs=(a[0]<<8)|(a[1]&0xff);
    }

    /* Skip to the next row in the binary file */
    if(w>(x2-x1+1)){
      unsigned char a[2];
      for(x=x2-x1+1;x<w;x++){
	if(fread(a,2*sizeof(unsigned char),1,fp)!=1){
	  show_ioerror("Load");
	  goto end_load;
	}
      }
    }
  }
  
  fclose(fp);

 end_load:
  statusline[0]=0;
  dumpscreen();
  status();
}


