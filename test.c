#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static FILE* open_flv(char *file_name){
	FILE* fp = NULL;
	
	fp = fopen(file_name,"rb");
	if(!fp){
		return fp;
	}
	
	fseek(fp,416,SEEK_SET);
	char *body;
	body = (char*)malloc(94577);
	fread(body,1,94577,fp);
	int i;
	for(i=0;i<94577;i++){
		if((i + 1) % 16 == 0){
			printf("%#x\n",body[i]);
		}else{
			printf("%#x ",body[i]);
		}
	}
	
	printf("=========\n");
	return fp;
}

void pushlish_stream(){
	char *flv = "./SampleVideo_1280x720_20mb.flv";
	FILE *fp = open_flv(flv);
	
	return ;
}

int main(){
	pushlish_stream();
	return 0;
}
