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
	char len[4];
	body = (char*)malloc(94577);
	fread(body,1,94577,fp);
	memcpy(len,body+5,4);
	int tmp = atoi(len);
	printf("%d\n",tmp);
	// 4 字节长度
	int x = len[0] + len[1]*256 + len[2]*65536 + len[3]*16777216;
	printf("%d\n",x);
	printf("%#x\n",len[0]);
	printf("%#x\n",len[1]);
	printf("%#x\n",len[2]);
	printf("%#x\n",len[3]);
	// 1 字节类型
	printf("%#x\n",body[9]);
	
	/*int i;
	
	printf("%#x\n",body[0]);
	printf("%#x\n",body[94576]);*/
	/*for(i=0;i<94577;i++){
		if((i + 1) % 16 == 0){
			printf("%#x\n",body[i]);
		}else{
			printf("%#x ",body[i]);
		}
	}*/
	
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
