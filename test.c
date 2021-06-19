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
	char tmp[4] = {body[5],body[6],body[7],body[8]};
	
	int x = ((tmp >> 24 & 0xFF)|(tmp >> 8 & 0xFF00)|(tmp << 8 & 0xFF00)|(tmp << 24 & 0xFF000000));
	
	printf("%#x\n",body[673]);
	printf("%#x\n",body[674]);
	printf("%#x\n",body[675]);
	printf("%#x\n",body[676]);
	printf("%#x\n",body[677]);
	printf("%#x\n",body[678]);
	// 4 字节长度
	//int x = len[0] + len[1]*256 + len[2]*65536 + len[3]*16777216;
	//int x = body[8] + body[7]*256 + body[6]*65536 + body[5]*16777216;
	printf("%d\n",x);
	// 1 字节类型
	//printf("%#x\n",body[9]);
	
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
