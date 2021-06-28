#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include "librtmp/rtmp.h"

static FILE* open_flv(char *file_name){
	FILE* fp = NULL;
	
	fp = fopen(file_name,"rb");
	if(!fp){
		return fp;
	}
	
	fseek(fp,9,SEEK_SET);
	fseek(fp,4,SEEK_CUR);
	return fp;
}

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

static int read_u24(FILE *fp,unsigned int *u24){
	unsigned int tmp;
	unsigned int ret;
	
	ret = fread(&tmp,1,3,fp);
	if(ret != 3){
		return 1;
	}
	
	*u24 = (((tmp>>16) & 0xFF) | ((tmp << 16) & 0xFF0000) | (tmp & 0xff00));
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

static int read_ts(FILE *fp,unsigned int *ts){
	unsigned int tmp;
	unsigned int ret;
	
	ret = fread(&tmp,1,4,fp);
	if(ret != 4){
		return 1;
	}
		
	*ts = ((tmp >> 16) & 0xFF) | ((tmp << 16) & 0xFF0000) | (tmp & 0xff00) | (tmp & 0xFF000000);
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


static int read_data(FILE *fp,RTMPPacket **packet){
	int ret = 1;
	size_t dataTmpSize = 0;
	
	unsigned int tt;
	unsigned int tagDataSize;
	unsigned int ts;
	unsigned int streamId;
	unsigned int preDataSize;
	char *body;
	
	
	if(read_u8(fp,&tt)){
		printf("Failed to read 1 byte type\n");
		goto __ERROR;
	}
	
	if(read_u24(fp,&tagDataSize)){
		printf("Failed to read 3 byte tag data size\n");
		goto __ERROR;
	}
	
	if(read_ts(fp,&ts)){
		printf("Failed to read 4 byte ts\n");
		goto __ERROR;
	}
	
	if(read_u24(fp,&streamId)){
		printf("Failed to read 3 byte stream id\n");
		goto __ERROR;
	}
	
	printf("tt:%d\t tagDataSize:%d\t ts:%d\t streamId:%d\t\n",tt,tagDataSize,ts,streamId);

	
	body = (char*)malloc(tagDataSize);
	
	dataTmpSize = fread(body,1,tagDataSize,fp);
	
	// audio
	if(tt == 8){
		printf("transform audio\n");
		disVideoTagData(body,tagDataSize);
	}

	// video
	if(tt == 9){
		printf("transform video\n");
		disVideoTagData(body,tagDataSize);
	}
	
	
	if(dataTmpSize != tagDataSize){
		printf("Failed to read tag body from flv,(tagDataSize=%zu:dataTmpSize=%d)\n",tagDataSize,dataTmpSize);
		goto __ERROR;
	}
	
	(*packet)->m_headerType = RTMP_PACKET_SIZE_LARGE;
	(*packet)->m_nTimeStamp = ts;
	(*packet)->m_packetType = tt;
	(*packet)->m_body = body;
	(*packet)->m_nBodySize = dataTmpSize;

	if(read_u32(fp,&preDataSize)){
		printf("Failed to read 4 byte pre tag data size\n");
		goto __ERROR;
	}
	
	return 0;
__ERROR:
	return 1;
}

static RTMPPacket* alloc_packet(){
	RTMPPacket *pack = NULL;
	
	pack =(RTMPPacket*) malloc(sizeof(RTMPPacket));
	if(!pack){
		printf("Failed to alloc memory\n");
		return NULL;
	}
	RTMPPacket_Alloc(pack,64*1024);
	RTMPPacket_Reset(pack);
	pack->m_hasAbsTimestamp = 0;
	pack->m_nChannel = 0x04;
	return pack;
}

static void send_data(FILE *fp,RTMP *rtmp){
	RTMPPacket *packet = NULL;
	unsigned int preTs = 0;
	unsigned int diffTs = 0;
	packet = alloc_packet();
	packet->m_nInfoField2 = rtmp->m_stream_id;
	
	while(1){
		if(read_data(fp,&packet)){
			printf("over\n");
			break;
		}
		diffTs = packet->m_nTimeStamp - preTs;
		//printf("diffTs:%d\n",(diffTs * 1000));
		usleep(diffTs * 1000);
		if(!RTMP_IsConnected(rtmp)){
			printf("Disconnect...\n");
			break;
		}
		if(!RTMP_SendPacket(rtmp,packet,0)){
			printf("Failed to send packet\n");
			break;
		}
		preTs = packet->m_nTimeStamp;
	}
	return ;
}
static RTMP* connect_rtmp_server(char *addr){
	RTMP *rtmp = NULL;
	
	rtmp = RTMP_Alloc();
	if(!rtmp){
		printf("Failed to alloc memory\n");
		goto __ERROR;
	}
	
	RTMP_Init(rtmp);
	
	rtmp->Link.timeout = 10;
	RTMP_SetupURL(rtmp,addr);
	RTMP_EnableWrite(rtmp);
	
	if(!RTMP_Connect(rtmp,NULL)){
		printf("Failed to connect rtmp server\n");
		goto __ERROR;
	}
	
	if(!RTMP_ConnectStream(rtmp,0)){
		printf("Failed to connect rtmp server stream\n");
		goto __ERROR;
	}
	
	return rtmp;
	
__ERROR:
	if(rtmp){
		RTMP_Close(rtmp);
		RTMP_Free(rtmp);
	}
	return NULL;

}

void pushlish_stream(){
	char *flv = "./SampleVideo_1280x720_20mb.flv";
	// need h264 + acc format flv video 
	char *rtmpaddr = "rtmp://ai-livepush.zbitcloud.com/live/5_1624015825?txSecret=361f2ece71d6788b5c16154e67c37855&txTime=60ccae01";
	FILE *fp = open_flv(flv);
	RTMP *rtmp = connect_rtmp_server(rtmpaddr);
	send_data(fp,rtmp);
	return ;
}


int main(){
	pushlish_stream();
	return 0;
}
