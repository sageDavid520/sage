#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_u8(FILE *fp,unsigned int *u8){
	unsigned int tmp;
	unsigned int ret;
	
	ret = fread(&tmp,1,1,fp);
	if(ret != 1){
		return 1;
	}
	
	*u8 = (tmp & 0xff);
	return 0;
}

static int read_u32(FILE *fp,unsigned int *u32){
	unsigned int tmp;
	unsigned int ret;
	
	ret = fread(&tmp,1,4,fp);
	if(ret != 4){
		return 1;
	}

	*u32 = ((tmp >> 24 & 0xFF)|(tmp >> 8 & 0xFF00)|(tmp << 8 & 0xFF00)|(tmp << 24 & 0xFF000000));
	return 0;
}

static FILE* open_flv(char *file_name){
	FILE* fp = NULL;
	
	fp = fopen(file_name,"rb");
	if(!fp){
		return fp;
	}
	
	fseek(fp,416,SEEK_SET);
	fseek(fp,5,SEEK_CUR);
	// 长度 4字节
	unsigned int u32;
	unsigned int u322;
	unsigned int u8;
	unsigned int u82;
	size_t dataTmpSize;

	read_u32(fp,&u32);
	// 673
	printf("读取4个字节，长度为:%d\n",u32);
	
	// 类型 1字节
	read_u8(fp,&u8);
	printf("读取1个字节，类型为:%d,0x16:%#x\n",u8);
	
	// 读取数据
	char* body;
	body = (char*)malloc(u32 - 1);
	dataTmpSize = fread(body,1,u32 - 1 ,fp);
	printf("读取%d个字节主体数据,读取了%d个字节\n",u32 - 1,dataTmpSize);
	
	// 下一个
	read_u32(fp,&u322);
	//printf("读取4个字节，长度为:%d\n",u322);// 这里有错误
	
	// 类型 1字节
	read_u8(fp,&u82);
	printf("读取1个字节，类型为:%d\n",u82);
	
	// 读取数据
	u322 = 93891 - 1;
	body = (char*)malloc(u322);
	dataTmpSize = fread(body,1,u322,fp);
	printf("读取%d个字节主体数据\n",u322);
	
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
