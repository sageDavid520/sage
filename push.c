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

static void read_u8(FILE *fp,unsigned int *u8){
	unsigned int tmp;
	fread(&tmp,1,1,fp);
	*u8 = ((tmp>>8&0xff)|(tmp<<8&0xff00));
	return ;
}

static void read_u24(FILE *fp,unsigned int *u24){
	unsigned int tmp;
	fread(&tmp,1,3,fp);
	*u24 = (((tmp>>16) & 0xFF) | ((tmp << 16) & 0xFF0000) | (tmp & 0xff00));
	return ;
}

static void read_u32(FILE *fp,unsigned int *u32){
	unsigned int tmp;
	fread(&tmp,1,4,fp);
	*u32 = ((tmp>>24&0xff)|(tmp>>8&0xff00)|(tmp<<8&0xff0000)|(tmp<<24&0xff000000));
	return ;
}

static int read_data(FILE *fp,RTMPPacket **packet){
	int ret =1;
	int dataTmpSize = 0;
	
	unsigned int tt;
	unsigned int tagDataSize;
	unsigned int ts;
	unsigned int streamId;
	
	read_u8(fp,&tt);
	read_u24(fp,&tagDataSize);
	read_u32(fp,&ts);
	read_u24(fp,&streamId);
	
	dataTmpSize = fread((*packet)->m_body,1,tagDataSize,fp);
	if(dataTmpSize!=tagDataSize){
		printf("Failed to read tag body from flv,(tagDataSize=%zu:dataTmpSize=%d)\n",tagDataSize,dataTmpSize);
		return ret;
	}
	
	(*packet)->m_headerType = RTMP_PACKET_SIZE_LARGE;
	(*packet)->m_nTimeStamp = ts;
	(*packet)->m_nBodySize = dataTmpSize;
	
	return 0;
	
}

static RTMPPacket* alloc_packet(){
	RTMPPacket *pack = NULL;
	
	pack =(RTMPPacket*) malloc(sizeof(RTMPPacket));
	if(!pack){
		printf("No Memory");
		return NULL;
	}
	RTMPPacket_Alloc(pack,64*1024);
	RTMPPacket_Reset(pack);
	pack->m_hasAbsTimestamp = 0;
	pack->m_nChannel = 0x4;
	return pack;
}

static void send_data(FILE *fp,RTMP *rtmp){
	RTMPPacket *packet = NULL;
	packet = alloc_packet();
	packet->m_nInfoField2 = rtmp->m_stream_id;
	while(1){
		if(read_data(fp,&packet)){
			printf("over\n");
			break;
		}

		if(!RTMP_IsConnected(rtmp)){
			printf("Disconnect...\n");
			break;
		}
		
		RTMP_SendPacket(rtmp,packet,0);
	}
	return ;
}
static RTMP* connect_rtmp_server(char *addr){
	RTMP *rtmp = NULL;
	
	rtmp = RTMP_Alloc();
	if(!rtmp){
		goto __ERROR;
	}
	
	RTMP_Init(rtmp);
	
	rtmp->Link.timeout = 10;
	RTMP_SetupURL(rtmp,addr);
	
	if(!RTMP_Connect(rtmp,NULL)){
		goto __ERROR;
	}
	
	RTMP_EnableWrite(rtmp);
	
	RTMP_ConnectStream(rtmp,0);
	
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
	char *rtmpaddr = "rtmp://ai-livepush.zbitcloud.com/live/5_1623903184?txSecret=d9394830d56731ec1815382360f7ce59&txTime=60caf600";
	FILE *fp = open_flv(flv);
	RTMP *rtmp = connect_rtmp_server(rtmpaddr);
	send_data(fp,rtmp);
	return ;
}


int main(){
	pushlish_stream();
	return 0;
}
