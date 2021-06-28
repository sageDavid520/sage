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
  return ;
}
