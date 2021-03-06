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
	printf("%#x\n",tmp);
	*u32 = ((tmp >> 24 & 0xFF)|(tmp >> 8 & 0xFF00)|(tmp << 8 & 0xFF0000)|(tmp << 24 & 0xFF000000));
	return 0;
}

static int disVideoTagData(char *body,unsigned int length){
	// 前5个字节为描述 第二个字节为AVCPackeType 0 配置 1具体数据
	if(body[1] != 0x01){
		return 0;	
	}
	unsigned int len;
	unsigned int len_byte_0;
	unsigned int len_byte_1;
	unsigned int len_byte_2;
	unsigned int len_byte_3;
	char byte_0;
	char byte_1;
	char byte_2;
	char byte_3;

	unsigned int data_st;
	unsigned int data_sp;

	unsigned int index = 5;
	unsigned int i;

	while(1){
		printf("当前下标:%d\n",index);
		if(index >= length){
			break;
		}
		len_byte_0 = index + 0;
		len_byte_1 = index + 1;
		len_byte_2 = index + 2;
		len_byte_3 = index + 3;
		byte_0 = body[len_byte_0];
		byte_1 = body[len_byte_1];
		byte_2 = body[len_byte_2];
		byte_3 = body[len_byte_3];

		//printf("解析到的长度数据0位:%#x\n",byte_0);
		//printf("解析到的长度数据1位:%#x\n",byte_1);
		//printf("解析到的长度数据2位:%#x\n",byte_2);
		//printf("解析到的长度数据3位:%#x\n",byte_3);


		//len =  ((0x000000FF & byte_0 << 24) | (0x000000FF & byte_1 << 16) | (0x000000FF & byte_2) << 8) | (0x000000FF & byte_3);
		len = (
		  ((0x000000FF & byte_0) << 24) | 
		  ((0x000000FF & byte_1) << 16) | 
		  ((0x000000FF & byte_2) << 8) | 
		  (0x000000FF & byte_3));

		//printf("解析到的数据长度:%d\n",len);

		data_st = index + 4;
		data_sp = data_st + len;

		//printf("开始下标:%d,结束下标:%d\n",data_st,data_sp);

		//printf("0:%#x\n",(0xff & body[data_st+0]));
		//printf("1:%#x\n",(0xff & body[data_st+1]));
		//printf("2:%#x\n",(0xff & body[data_st+2]));
		//printf("3:%#x\n",(0xff & body[data_st+3]));

		// 判断只要是nula数据直接取反
		for(i = data_st; i < data_sp; i++){
			body[i] = ~body[i];
		}

		index = data_sp;
	}
	return 0;
}
static int disAudioTagData(char *body,unsigned int len){
	// 前2个字节为描述 第二个字节为AVCPackeType 0 配置 1具体数据
	if(body[1] != 0x01){
		return 0;
	}
	unsigned int i = 2;
	for(i = 2; i < len; i++){
		body[i] = ~body[i];
	}
	return 0;
}

static FILE* open_flv(char *file_name){
	FILE* fp = NULL;
	
	fp = fopen(file_name,"rb");
	if(!fp){
		return fp;
	}
	
	size_t dataTmpSize;
	unsigned int u32;
	
	fseek(fp,416,SEEK_SET);
	//read_u32(fp,&u32);
	
	//printf("%#x\n%d\n",u32,u32);
	//return NULL;
	char* body;
	body = (char*)malloc(94577);
	dataTmpSize = fread(body,1,94577 ,fp);
	
	//disVideoTagData(body,94577);
	//disAudioTagData(body,94577);
	
	
	
	//len =  ((0x000000FF & body[0] << 24) | (0x000000FF & body[1] << 16) | (0x000000FF & body[2]) << 8) | (0x000000FF & body[3]);
	//printf("%#x\n",len);
	//printf("%#x\n",(0x000000FF & body[2] << 8));
	//printf("%d\n",((0x000000FF & body[0] << 24) | (0x000000FF & body[1] << 16) | (0x000000FF & body[2]) << 8) | (0x000000FF & body[3]));
		
	//printf("%#x\n",((0x000000FF & body[2] << 8) | (0x000000FF & body[1] << 16) | (0x000000FF & body[0] << 24) | body[3]));
	/*
	int a = 0;
	int c = 0;
	int d = 0;
	a = *(int*)body;
	
	d = body[0] | body[1] | body[2] | body[3];
	a = body[0] << 24 | body[1] << 16 | body[2] << 8 | body[3];
	c = body[3] << 24 | body[2] << 16 | body[1] << 8 | body[0];
	printf("%#x\n",a);
	printf("%#x\n",c);
	printf("%#x\n",d);*/
	//char* c2 = NULL;
	//StringToHex(body,c2);
	//printf("%#x,%#x,%#x,%#x",(*body)[0],(*body)[1],(*body)[2],(*body)[3]);
	
	//char c0[11];
	//char c1[11];
	//char c2[11];
	//char c3[11];

	//snprintf(c0, 11, "%#x", body[0]);
	//snprintf(c1, 11, "%#x", body[1]);
	//snprintf(c2, 11, "%#x", body[2]);
	//snprintf(c3, 11, "%#x", body[3]);

	//char* str;
	//unsigned int *q;
	//unsigned int *x;
	//q = (unsigned int*)malloc(4);
	//memset(q,0,4);
    	//int i0 = strtol(c0, &str, 16);
	//int i1 = strtol(c1, &str, 16);
	//int i2 = strtol(c2, &str, 16);
	//int i3 = strtol(c3, &str, 16);
	
	//q[0] |= i0;
	//q[1] |= i1;
	//q[2] |= i2;
	//q[3] |= i3;
	//i0 = 0x00;
	//i1 = 0x00;
	//i2 = 0x02;
	//i3 = 0xA1;
	
	//unsigned int len[4] = {i0,i1,i2,i3};
	//printf("%#x\n",len);
	//printf("%d\n",len);
	//printf("%#x\n",*q);
	//printf("%#x,",i0);
	//printf("%#x,",i1);
	//printf("%#x,",i2);
	//printf("%#x\n",i3);
	//printf("%#x\n",0x000000FF);
	//printf("%d\n",(0xFFFFFFFF & i0 << 24 >> 24));
	//printf("%d\n",(0xFFFFFFFF  & i1 << 24 >> 16));
	//printf("%d\n",(0xFFFFFFFF & i2 << 24 >> 8));
	//printf("%d\n",(0xFFFFFFFF & i3 << 24));
	
	//*q = ((0xFFFFFFFF & i3 << 24) | (0xFFFFFFFF & i2 << 24 >> 8) | (0xFFFFFFFF  & i1 << 24 >> 16) | (0xFFFFFFFF & i0 << 24 >> 24));
	//printf("%d\n",q);

	//printf("%#x\n",((i0 & 0x000000FF) | (i1 & 0x000000FF << 8) | (i2 & 0x000000FF << 16) | (i3 & 0x000000FF << 24)));
	//printf("%d\n",(i0 | i1 | i2 | i3));
	//*x = ((*q >> 24 & 0xFF)|(*q >> 8 & 0xFF00)|(*q << 8 & 0xFF0000)|(*q << 24 & 0xFF000000));
	//*x = ((i0 >> 24 & 0xFF)|(i1 >> 8 & 0xFF00)|(i2 << 8 & 0xFF0000)|(i3 << 24 & 0xFF000000));
	//printf("%d\n",x);

	
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
