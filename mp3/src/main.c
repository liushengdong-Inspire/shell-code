#include <stdio.h>
#include "mp3.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "gbktoutf8.h"

#define BUF_SIZE 2048

int main(int argc,char *arv[])
{
	int type = mp3_type("./alzm.mp3");
	if(type == -1 || type == 0)
	{
		printf("UNKNOWN!\n");
	}
	else if(type == 1)
	{
		printf("ID3V1\n");
		p_MP3_ID3V1 id3v1_info = (p_MP3_ID3V1)malloc(sizeof(MP3_ID3V1));
		if(id3v1_info == NULL )
		{
			fprintf(stderr,"Malloc id3v1_info Error:%s\n",strerror(errno));
			return -1;
		}
		
		p_MP3_ID3V1 id3v1_info_out = (p_MP3_ID3V1)malloc(sizeof(MP3_ID3V1));
		if(id3v1_info_out == NULL )
		{
			fprintf(stderr,"Malloc id3v1_info Error:%s\n",strerror(errno));
			return -1;
		}
		memset(id3v1_info,0,sizeof(MP3_ID3V1));
		memset(id3v1_info_out,0,sizeof(MP3_ID3V1));
		read_mp3_ID3V1_info(id3v1_info,"./alzm.mp3");
		//printf("TAG : %s\n",id3v1_info->TAG);
		code_convert("gbk","utf-8",id3v1_info,sizeof(MP3_ID3V1),id3v1_info_out,sizeof(MP3_ID3V1));
		free(id3v1_info);
		free(id3v1_info_out);
		printf("%s\n",id3v1_info_out->Title);
	}
	else if(type == 2)
	{
		printf("ID3V2\n");
		int frame_len = read_back_ID3V2_note_size("./alzm.mp3");
		printf("frame_len = %d\n",frame_len);
		int i = 0;
		int pos = 0;
		int info_pos = 0;
		char *content = (char*)malloc(sizeof(char)*BUF_SIZE);
		for( i = 0; pos < frame_len; i++ )
		{
			memset(content,0,BUF_SIZE);
			//printf("before pos = %d\n",pos);
			char char_set;
			read_mp3_ID3VX_info_size("./alzm.mp3",pos,content,&pos,&info_pos,&char_set);
			if( info_pos != -1)
			{
				char content_out [BUF_SIZE] = {0};
				if( char_set == 0x00 )
				{
					code_convert("ISO_8859-1","utf-8",content,BUF_SIZE,content_out,BUF_SIZE);
				}
				else if( char_set == 0x01 )
				{
					code_convert("utf-16","utf-8",content,BUF_SIZE,content_out,BUF_SIZE);
				}
				else if( char_set == 0x02 )
				{
					code_convert("UTF-16BE","utf-8",content,BUF_SIZE,content_out,BUF_SIZE);
				}
				else if( char_set == 0x03 )
				{
					strncpy(content_out,content,BUF_SIZE);
				}
				else
				{
					printf("char_set = %x\n",char_set);
				}
				//printf("%s : %s\n",FrameID_array_info[info_pos],content);
				printf("%s : %s\n",FrameID_array_info[info_pos],content_out);
			}
			//printf("after pos = %d\n",pos);
			//printf("\n");
		}
		free(content);
	}
	return 0;
}
