#include <stdio.h>
#include <stdlib.h>

void
main()
{
  int c,i;

  c=0;
  fread(&c,4,1,stdin);
  printf("unsigned short __change_my_name__[]={");
  for(i=0;i<2000;i++){
    c=0;
    fread(&c,2,1,stdin);
    c=((c&0xff)<<8)|(c>>8);
    if((i%10)==0)printf("\n  ");
    printf("0x%04x%s",c,i<1999?",":"");
  }
  printf("\n};\n");
}

