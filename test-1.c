#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(){
	unsigned int i[4] = {
    0x00,
    0x01,
    0x6e,
    0xc3
  };
  printf("%#x\n",i[0]);
  printf("%#x\n",i[1]);
  printf("%#x\n",i[2]);
  printf("%#x\n",i[3]);
  printf("===%#x===\n",(
	  ((0x000000FF & i[0]) << 24) | 
	  ((0x000000FF & i[1]) << 16) | 
	  ((0x000000FF & i[2]) << 8) | 
	  (0x000000FF & i[3])));
  return ;
}
