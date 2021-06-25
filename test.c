#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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

void StringToHex(char *str, unsigned char *strhex)
{
	uint8_t i,cnt=0;
	char *p = str;             
	     
	for (i = 0; i < 4; i ++)  
	{
		if ((*p >= '0') && (*p <= '9'))
			strhex[cnt] = *p - '0' + 0x30;

		if ((*p >= 'A') && (*p <= 'Z')) 
			strhex[cnt] = *p - 'A' + 0x41;

		if ((*p >= 'a') && (*p <= 'z'))
			strhex[cnt] = *p - 'a' + 0x61;

		p ++;
		cnt ++;  
	}

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
	
	//char* c2 = NULL;
	//StringToHex(body,c2);
	//printf("%#x,%#x,%#x,%#x",body[0],body[1],body[2],body[3]);
	
	char c0[11];
	char c1[11];
	char c2[11];
	char c3[11];

	snprintf(c0, 11, "%#x", body[0]);
	snprintf(c1, 11, "%#x", body[1]);
	snprintf(c2, 11, "%#x", body[2]);
	snprintf(c3, 11, "%#x", body[3]);

	char* str;
	unsigned int *q;
	q = (unsigned int*)malloc(4);
	memset(q,0,4);
    	int i0 = strtol(c0, &str, 16);
	int i1 = strtol(c1, &str, 16);
	int i2 = strtol(c2, &str, 16);
	int i3 = strtol(c3, &str, 16);
	
	q |= i0;
	q++;
	q |= i0;
	q++;
	q |= i0;
	//printf("%d\n",i0);
	//printf("%d\n",i1);
	//printf("%d\n",i2);
	//printf("%d\n",i3);
	 //unsigned int x=((st >> 24 & 0xFF)|(st >> 8 & 0xFF00)|(st << 8 & 0xFF0000)|(st << 24 & 0xFF000000))


	
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
