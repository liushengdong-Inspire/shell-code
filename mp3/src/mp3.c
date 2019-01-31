#include "mp3.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



//判断 MP3 文件是 ID3V1 还是 ID3V2
int mp3_type(char *mp3_path)
{
	if( mp3_path == NULL )
	{
		printf("mp3_path = NULL !\n");
		return -1;
	}
	
	//打开 MP3 文件
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		fprintf(stderr,"Open %s Error:%s\n",mp3_path,strerror(errno));
		return -1;
	}
	
	//类型判断空间
	char *mp3_type = (char *)malloc(sizeof(char)*MP3_TYPE_SIZE);
	if( mp3_type == NULL )
	{
		fprintf(stderr,"Malloc mp3_type failed:%s\n",strerror(errno));
		fclose(mp3_fp);
		return -1;
	}
	
	int type = -1;
	//判断是否是 ID3V2 封装格式
	memset(mp3_type,0,MP3_TYPE_SIZE);
	fread(mp3_type,sizeof(char),MP3_TYPE_SIZE,mp3_fp);
	if( mp3_type[0] == 'I' && mp3_type[1] == 'D' && mp3_type[2] == '3' )
	{
		type = 2;
	}
	else
	{
		//判断类型是否是 ID3V1 封装格式
		memset(mp3_type,0,MP3_TYPE_SIZE);
		fseek(mp3_fp,-MP3_ID3V1_INFO_LENTH*sizeof(char),SEEK_END);
		fread(mp3_type,sizeof(char),MP3_TYPE_SIZE,mp3_fp);
		if(mp3_type[0] == 'T' && mp3_type[1] == 'A' && mp3_type[2] == 'G' )
		{
			type = 1;
		}
		else
		{
			type = 0;
		}
	}
	
	free(mp3_type);
	fclose(mp3_fp);
	return type;
}

// 解析 MP3 ID3V1 相关内容
int read_mp3_ID3V1_info(p_MP3_ID3V1 id3v1_info,char *mp3_path)
{
	if( id3v1_info == NULL || mp3_path == NULL )
	{
		printf("id3v1_info or mp3_path == NULL,Please check!\n");
		return -1;
	}
	
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		fprintf(stderr,"Open %s Error:%s\n",mp3_path,strerror(errno));
		return -1;
	}
	
	fseek(mp3_fp,-MP3_ID3V1_INFO_LENTH*sizeof(char),SEEK_END);
	fread(id3v1_info,sizeof(MP3_ID3V1),1,mp3_fp);
	fclose(mp3_fp);
	return 0;
}

// 获取 MP3 ID3V2 头部信息
int read_mp3_ID3VX_HEAD_info(FILE *fp,p_MP3_ID3X_HEADER mp3_id3x_info)
{
	if( fp == NULL || mp3_id3x_info == NULL )
	{
		printf("read_mp3_ID3VX_HEAD_info fp or mp3_id3x_info == NULL \n");
		return -1;
	}
	
	fseek(fp,0,SEEK_SET);
	fread(mp3_id3x_info,sizeof(MP3_ID3X_HEADER),1,fp);
	return 0;
}

// 返回 MP3 ID3V2 标签大小
int read_back_ID3V2_note_size(char *mp3_path)
{
	if( mp3_path == NULL )
	{
		printf("Mp3 path == NULL\n");
		return -1;
	}
	
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		fprintf(stderr,"Open %s Error:%s\n",mp3_path,strerror(errno));
		return -1;
	}
	
	MP3_ID3X_HEADER mp3_id3x_info;
	if(read_mp3_ID3VX_HEAD_info(mp3_fp,&mp3_id3x_info) != -1)
	{
		int len = mp3_id3x_info.Size[0] << 21 | mp3_id3x_info.Size[1] << 14 | mp3_id3x_info.Size[2] << 7 | mp3_id3x_info.Size[3];
		return len;
	}
	return 0;
	
}

// 解析帧内容信息
int read_mp3_ID3VX_info_size(char *mp3_path,int current_pos,char *content,int *now_pos,int *info_pos,char *charset)
{
	if( mp3_path == NULL )
	{
		printf("read_mp3_ID3VX_info mp3_path == NULL \n");
		return -1;
	}
	
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		fprintf(stderr,"Open %s Error:%s\n",mp3_path,strerror(errno));
		return -1;
	}
	
	fseek(mp3_fp,current_pos,SEEK_SET);
	
	if( current_pos == 0 )
	{
		MP3_ID3X_HEADER mp3_id3x_header;
		read_mp3_ID3VX_HEAD_info(mp3_fp,&mp3_id3x_header);
	}
	
	// 读取帧内容信息
	MP3_ID3X_INFO mp3_id3x_info;
	memset(&mp3_id3x_info,0,sizeof(MP3_ID3X_INFO));
	fread(&mp3_id3x_info,sizeof(MP3_ID3X_INFO),1,mp3_fp);
	
	//获取帧内容大小
	int length = mp3_id3x_info.Size[0] << 24 | mp3_id3x_info.Size[1] << 16 | mp3_id3x_info.Size[2] << 8 | mp3_id3x_info.Size[3];
	//printf("length = %d\n",length);
	
	//获取帧内容，如果内容为空，则不往下处理，直接放回返回当前位置
	int frame_length = length - 1;
	if( frame_length < 0 ){
		*info_pos = -1;
		*now_pos = ftell(mp3_fp);
		fclose(mp3_fp);
		return -1;
	}
	
	char *frame_content = (char*)malloc(sizeof(char)*frame_length);
	if( frame_content == NULL )
	{
		fprintf(stderr,"Malloc frame_content == NULL Error:%s\n",strerror(errno));
		fclose(mp3_fp);
		return -1;
	}
	
	//读取编码格式
	char charset_read;
	fread(&charset_read,sizeof(char),1,mp3_fp);
	
	
	fread(frame_content,(length-1),1,mp3_fp);
	//读取帧内容信息，主要获取作者名，歌曲名，还有就是背景图
	int i = 0;
	int info_positon = -1;
	for(i = 0; i < 72;i ++ ) {
		if(!strncmp(mp3_id3x_info.FrameID,FrameID_array[i],4))
		{
			//printf(" %d %s : %s\n",i,mp3_id3x_info.FrameID,FrameID_array[i]);
			info_positon = i;
			break;
		}
	}
	
	if(info_positon == -1 )
	{
		//返回信息中，info_pos 为负数，代表是没有读取到信息
		*info_pos = -1;
		* now_pos = ftell(mp3_fp);
		free(frame_content);
		fclose(mp3_fp);
		return 0;
	}

	//返回信息，包括了当前文件指针位置，读取到的帧内容信息，还有就是帧内容信息位置
	* now_pos = ftell(mp3_fp);
	strncpy(content,frame_content,length);
	*info_pos = info_positon;
	*charset = charset_read;
	
	free(frame_content);
	fclose(mp3_fp);
	return 0;
}