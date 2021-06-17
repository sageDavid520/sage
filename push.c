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
	//unsigned int tmp;
	//fread(&tmp,1,1,fp);
	//*u8 = ((tmp>>8&0xff)|(tmp<<8&0xff00));
	fread(u8,1,1,fp);
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

static void read_ts(FILE *fp,unsigned int *ts){
	unsigned int tmp;
	fread(&tmp,1,4,fp);
	
	*ts = ((tmp >> 16) & 0xFF) | ((tmp << 16) & 0xFF0000) | (tmp & 0xff00) | (tmp & 0xFF000000);
	return ;
}

static int read_data(FILE *fp,RTMPPacket **packet){
	int ret = 1;
	int dataTmpSize = 0;
	
	unsigned int tt;
	unsigned int tagDataSize;
	unsigned int ts;
	//unsigned int tsExt;
	unsigned int streamId;
	
	read_u8(fp,&tt);
	read_u24(fp,&tagDataSize);
	read_ts(fp,&ts);
	//read_u24(fp,&ts);
	//read_u8(fp,&tsExt);
	read_u24(fp,&streamId);
	
	printf("tt:%d\n",tt);
	printf("tagDataSize:%d\n",tagDataSize);
	//printf("ts:%d\n",ts);
	//printf("tsExt:%d\n",tsExt);
	printf("ts:%d\n",ts);
	printf("streamId:%d\n",streamId);
	printf("=======================================\n");
	
	dataTmpSize = fread((*packet)->m_body,1,tagDataSize,fp);
	if(dataTmpSize!=tagDataSize){
		printf("Failed to read tag body from flv,(tagDataSize=%zu:dataTmpSize=%d)\n",tagDataSize,dataTmpSize);
		return ret;
	}
	
	(*packet)->m_headerType = RTMP_PACKET_SIZE_LARGE;
	(*packet)->m_nTimeStamp = ts;
	(*packet)->m_packetType = tt;
	(*packet)->m_nBodySize = dataTmpSize;
	
	fseek(fp,4,SEEK_CUR);
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
	unsigned int preTs = 0;
	unsigned int diffTs = 0;
	packet = alloc_packet();
	packet->m_nInfoField2 = rtmp->m_stream_id;
	
	printf("=======================================\n");
	int i=1;
	while(1){
		if(i==5){
		//	break;
		}
		if(read_data(fp,&packet)){
			printf("over\n");
			break;
		}

		if(!RTMP_IsConnected(rtmp)){
			printf("Disconnect...\n");
			break;
		}
		diffTs = packet->m_nTimeStamp - preTs;
		
		usleep(diffTs * 1000);
		
		RTMP_SendPacket(rtmp,packet,0);
		
		preTs = packet->m_nTimeStamp;
		
		//usleep(500000);
		usleep(5000);	
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
