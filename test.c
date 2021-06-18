#include <stdio.h>

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

void pushlish_stream(){
	char *flv = "./SampleVideo_1280x720_20mb.flv";
	// need h264 + acc format flv video 
	char *rtmpaddr = "rtmp://ai-livepush.zbitcloud.com/live/5_1624015825?txSecret=361f2ece71d6788b5c16154e67c37855&txTime=60ccae01";
	FILE *fp = open_flv(flv);
	RTMP *rtmp = connect_rtmp_server(rtmpaddr);
	send_data(fp,rtmp);
	return ;
}
