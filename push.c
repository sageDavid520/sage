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
	int ret;
	unsigned int tmp;
	
	ret = fread(&tmp,1,1,fp);
	if(ret != 1){
		return 1;
	}
	
	*u8 = (tmp & 0xff);
	return 0;
}

static int read_u24(FILE *fp,unsigned int *u24){
	int ret;
	unsigned int tmp;
	
	ret = fread(&tmp,1,3,fp);
	if(ret != 3){
		return 1;
	}
	
	*u24 = (((tmp>>16) & 0xFF) | ((tmp << 16) & 0xFF0000) | (tmp & 0xff00));
	return 0;
}

static int read_u32(FILE *fp,unsigned int *u32){
	int ret;
	unsigned int tmp;
	
	ret = fread(&tmp,1,4,fp);
	if(ret != 4){
		return 1;
	}
	
	*u32 = ((tmp >> 24 & 0xFF)|(tmp >> 8 & 0xFF00)|(tmp << 8 & 0xFF00)|(tmp << 24 & 0xFF000000));
	return 0;
}

static int read_ts(FILE *fp,unsigned int *ts){
	int ret;
	unsigned int tmp;
	
	ret = fread(&tmp,1,4,fp);
	if(ret != 4){
		return 1;
	}
		
	*ts = ((tmp >> 16) & 0xFF) | ((tmp << 16) & 0xFF0000) | (tmp & 0xff00) | (tmp & 0xFF000000);
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
	
	dataTmpSize = fread((*packet)->m_body,1,tagDataSize,fp);
	
	if(dataTmpSize!=tagDataSize){
		printf("Failed to read tag body from flv,(tagDataSize=%zu:dataTmpSize=%d)\n",tagDataSize,dataTmpSize);
		goto __ERROR;
	}

	(*packet)->m_headerType = RTMP_PACKET_SIZE_LARGE;
	(*packet)->m_nTimeStamp = ts;
	(*packet)->m_packetType = tt;
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
	unsigned int preTs = 0;
	unsigned int diffTs = 0;
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
		
		diffTs = packet->m_nTimeStamp - preTs;
		//printf("diffTs:%d\n",(diffTs * 1000));
		usleep(diffTs * 1000);
		RTMP_SendPacket(rtmp,packet,0);
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
	
	if(!RTMP_Connect(rtmp,NULL)){
		printf("Failed to connect rtmp server\n");
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
	char *rtmpaddr = "rtmp://ai-livepush.zbitcloud.com/live/5_1623907100?txSecret=f37811802ebd354dd90be955e291f716&txTime=60cb054c";
	FILE *fp = open_flv(flv);
	RTMP *rtmp = connect_rtmp_server(rtmpaddr);
	send_data(fp,rtmp);
	return ;
}


int main(){
	pushlish_stream();
	return 0;
}
