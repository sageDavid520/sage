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

	*u32 = ((tmp >> 24 & 0xFF)|(tmp >> 8 & 0xFF00)|(tmp << 8 & 0xFF0000)|(tmp << 24 & 0xFF000000));
	return 0;
}

static int dis_video_data(FILE *fp){
}

static FILE* open_flv(char *file_name){
	FILE* fp = NULL;
	
	fp = fopen(file_name,"rb");
	if(!fp){
		return fp;
	}
	
	size_t dataTmpSize;

	
	fseek(fp,416,SEEK_SET);
	fseek(fp,5,SEEK_CUR);
	
	char* body;
	body = (char*)malloc(94577);
	dataTmpSize = fread(body,1,94577 ,fp);
	
	char c1[4];
	memcpy(c1,body,4);
	printf("%d,%d,%d,%d\n",c1[0],c1[1],c1[2],c1[3]);
	
	unsigned int k[4] = {c1[0]-'0',c1[1]-'0',c1[2]-'0',c1[3]-'0'};
	
	printf("%#x,%#x,%#x,%#x",k[0],k[1],k[2],k[3]);
	/*
	
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
	printf("读取4个字节，长度为:%d\n",u322);// 这里有错误
	
	// 类型 1字节
	read_u8(fp,&u82);
	printf("读取1个字节，类型为:%d,0x16:%#x\n",u82);
	
	// 读取数据
	
	u322 = 93891 - 1;
	body = (char*)malloc(u322);
	dataTmpSize = fread(body,1,u322,fp);
	printf("读取%d个字节主体数据\n",u322);
	
	
	body = (char*)malloc(u322-1);
	dataTmpSize = fread(body,1,u322-1,fp);
	printf("读取%d个字节主体数据\n",u322-1);
	printf("%#x,%#x,%#x,%#x",body[28351],body[28352],body[28353],body[28354]);
	
	// 音频 跳过1字节
	// 下一个
	// 类型 1字节
	printf("读取1个字节，0x16:%#x\n",body[93889]);
	*/
	
	
	
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
