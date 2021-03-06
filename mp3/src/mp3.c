#include "mp3.h"
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "LSD_LOG.h"
#include "save_json.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

//判断 MP3 文件是 ID3V1 还是 ID3V2
int mp3_type(char *mp3_path)
{
	if( mp3_path == NULL )
	{
		LSD_ERROR("mp3_path = NULL !\n");
		return -1;
	}
	
	//打开 MP3 文件
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		LSD_STDERROR("Open %s Error:%s\n",mp3_path,strerror(errno));
		return -1;
	}
	
	//类型判断空间
	char *mp3_type = (char *)malloc(sizeof(char)*MP3_TYPE_SIZE);
	if( mp3_type == NULL )
	{
		LSD_STDERROR("Malloc mp3_type failed:%s\n",strerror(errno));
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
	mp3_type = NULL;
	fclose(mp3_fp);
	return type;
}

// 解析 MP3 ID3V1 相关内容
int read_mp3_ID3V1_info(p_MP3_ID3V1 id3v1_info,char *mp3_path)
{
	if( id3v1_info == NULL || mp3_path == NULL )
	{
		LSD_ERROR("id3v1_info or mp3_path == NULL,Please check!\n");
		return -1;
	}
	
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		LSD_STDERROR("Open %s Error:%s\n",mp3_path,strerror(errno));
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
		LSD_ERROR("read_mp3_ID3VX_HEAD_info fp or mp3_id3x_info == NULL \n");
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
		LSD_ERROR("Mp3 path == NULL\n");
		return -1;
	}
	
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		LSD_STDERROR("Open %s Error:%s\n",mp3_path,strerror(errno));
		return -1;
	}
	
	MP3_ID3X_HEADER mp3_id3x_info;
	if(read_mp3_ID3VX_HEAD_info(mp3_fp,&mp3_id3x_info) != -1)
	{
		int len = mp3_id3x_info.Size[0] << 21 | mp3_id3x_info.Size[1] << 14 | mp3_id3x_info.Size[2] << 7 | mp3_id3x_info.Size[3];
		return len;
	}
	fclose(mp3_fp);
	return 0;
	
}

static int mp3_pic_number = 0;

//保存图片到文件夹中
void save_picture(char *mp3_path,int pic_pos,char *content_out,int frame_size,char **save_path_out)
{
	if( mp3_path == NULL || pic_pos <= 0 || content_out == NULL || save_path_out == NULL)
	{
		LSD_ERROR("save_picture Error,param is Null\n");
		return;
	}
	
	int read_file_begin_flag_pos = 0;
	int read_file_end_flag_pos = 0;
	FILE *pic_fp = NULL;
	FILE *mp3_fp = NULL;
	unsigned short pis[5];
	char *save_path = NULL;
	char *save_file_buf = NULL;
	char file_type[5] = {0};
	unsigned short  BMP=0x4D42,JPG=0xD8FF,JPG_END=0xD9FF,PNG[4]={0x5089,0x474E,0x0A0D,0x0A1A},GIF[3]={0x4947,0x3846,0x6139};
	
	if( !strncmp( content_out,"image/jpeg",strlen("image/jpeg")) || !strncmp( content_out,"image/jpeg",strlen("image/jpg"))) {
		strncpy(file_type,".jpg",strlen(".jpg"));
	}else if(!strncmp( content_out,"image/png",strlen("image/png"))) {
		strncpy(file_type,".png",strlen(".png"));
	}else if( !strncmp( content_out,"image/bmp",strlen("image/bmp")) ) {
		strncpy(file_type,".bmp",strlen(".bmp"));
	}
	
	save_path = (char*)malloc(sizeof(char)*BUF_SIZE);
	if( save_path == NULL )
	{
		LSD_STDERROR("Malloc save_path Error:%s\n",strerror(errno));
		return;
	}
	memset(save_path,0,BUF_SIZE);
	snprintf(save_path,BUF_SIZE,"%s/%s%s",PIC_SAVE_PATH,mp3_path,file_type);
	snprintf(save_path,BUF_SIZE,"%s/%d%s",PIC_SAVE_PATH,mp3_pic_number,file_type);
	mp3_pic_number++;
	
	if(access(save_path,F_OK) != 0)
		mkdir(PIC_SAVE_PATH,0755);
	
	mp3_fp = fopen(mp3_path,"r");
	pic_fp = fopen(save_path,"w+");
	if( pic_fp == NULL || mp3_fp == NULL )
	{
		LSD_STDERROR("Open mp3_path=%s Error:%s\n",mp3_path,strerror(errno));
		if( mp3_fp != NULL )
			fclose(mp3_fp);
		return;
	}
	fseek(mp3_fp,pic_pos,SEEK_SET);
	read_file_begin_flag_pos = ftell(mp3_fp);
	while(read_file_begin_flag_pos < frame_size) {
		fseek(mp3_fp,read_file_begin_flag_pos,SEEK_SET);
		fread(pis,8,1,mp3_fp);
		if(pis[0]==BMP) {
			break;
		}else if(pis[0]==JPG) {
			break;
		}else if(PNG[0]==pis[0]&&PNG[1]==pis[1]&&PNG[2]==pis[2]&&PNG[3]==pis[3]) {
			break;
		}else if(GIF[0]==pis[0]&&GIF[1]==pis[1]&&GIF[2]==pis[2]) {
			break;
		}
		read_file_begin_flag_pos ++;
	}
	
	read_file_end_flag_pos = ftell(mp3_fp);
	while(read_file_end_flag_pos < frame_size) {
		fseek(mp3_fp,read_file_end_flag_pos,SEEK_SET);
		fread(pis,8,1,mp3_fp);
		if(pis[0] == JPG_END)
			break;
		
		read_file_end_flag_pos ++;
	}
	
	int pic_file_size = read_file_end_flag_pos - read_file_begin_flag_pos+2;
	fseek(mp3_fp,read_file_begin_flag_pos,SEEK_SET);
	save_file_buf = (char*)malloc(sizeof(char)*pic_file_size);
	if( save_file_buf == NULL )
	{
		LSD_STDERROR("Malloc save_file_buf Error:%s\n",strerror(errno));
		free(save_path);
		fclose(mp3_fp);
		fclose(pic_fp);
		return;
	}
	
	LSD_ERROR("%s picture save to %s\n",mp3_path,save_path);
	
	memset(save_file_buf,0,pic_file_size);
	fread(save_file_buf,pic_file_size,1,mp3_fp);
	fwrite(save_file_buf,pic_file_size,1,pic_fp);
	strncpy(*save_path_out,save_path,BUF_SIZE);
	free(save_path);
	fclose(mp3_fp);
	fclose(pic_fp);
}

// 解析帧内容信息
int read_mp3_ID3VX_info_size(char *mp3_path,int current_pos,int *now_pos,char **save_path_out,char **title,char **singer)
{
	if( mp3_path == NULL )
	{
		LSD_ERROR("read_mp3_ID3VX_info mp3_path == NULL \n");
		return -1;
	}
	
	FILE *mp3_fp = fopen(mp3_path,"r");
	if( mp3_fp == NULL )
	{
		LSD_STDERROR("Open %s Error:%s\n",mp3_path,strerror(errno));
		return -1;
	}
	
	fseek(mp3_fp,current_pos,SEEK_SET);
	
	//将文件指针略过头部，准备开始读取帧信息
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
	LSD_DEBUG("length = %d\n",length);
	
	//获取帧内容，如果内容为空，则不往下处理，直接放回返回当前位置
	int frame_length = length;
	if( frame_length <= 0 ){
		*now_pos = ftell(mp3_fp);
		fclose(mp3_fp);
		return -1;
	}
	
	//读取编码格式
	char charset_read;
	fread(&charset_read,sizeof(char),1,mp3_fp);
	
	//读取帧内容信息，主要获取作者名，歌曲名，还有就是背景图
	int i = 0;
	int info_positon = -1;
	int pic_frameflag = 0;
	for(i = 0; i < 72;i ++ ) {
		if(!strncmp(mp3_id3x_info.FrameID,FrameID_array[i],4))
		{
			LSD_DEBUG(" %d %s : %s\n",i,mp3_id3x_info.FrameID,FrameID_array[i]);
			info_positon = i;
			break;
		}
	}
	
	//此时文件指针位置，为后面读取图片信息做准备
	int file_pos_read_charset = ftell(mp3_fp);
	
	char *frame_content = (char*)malloc(sizeof(char)*frame_length);
	char *content_out = (char*)malloc(sizeof(char)*frame_length);
	if( frame_content == NULL || content_out == NULL )
	{
		LSD_STDERROR("Malloc frame_content == NULL or content = NULL Error:%s\n",strerror(errno));
		if( frame_content != NULL) {
			free(frame_content);
			frame_content = NULL;
		}
		fclose(mp3_fp);
		return -1;
	}
	fread(frame_content,(length-1),1,mp3_fp);
	*now_pos = ftell(mp3_fp);
	fclose(mp3_fp);
	
	//未读取到任何信息
	if(info_positon == -1 )
	{
		free(frame_content);
		frame_content = NULL;
		free(content_out);
		content_out = NULL;
		return 0;
	}
	
	if( charset_read == 0x00 )
	{
		code_convert("ISO_8859-1","utf-8",frame_content,frame_length,content_out,frame_length);
	}
	else if( charset_read == 0x01 )
	{
		code_convert("utf-16","utf-8",frame_content,frame_length,content_out,frame_length);
	}
	else if( charset_read == 0x02 )
	{
		code_convert("UTF-16BE","utf-8",frame_content,frame_length,content_out,frame_length);
	}
	else if( charset_read == 0x03 )
	{
		strncpy(content_out,frame_content,frame_length);
	}
	else
	{
		LSD_DEBUG("charset_read = %x\n",charset_read);
	}
	LSD_DEBUG("%s : %s\n",FrameID_array_info[info_positon],content_out);
	if( !strncmp(FrameID_array_info[info_positon],"附加描述",strlen("附加描述")) && !strncmp( content_out,"image/",strlen("image/"))){
		LSD_INFO("%s : %s\n",FrameID_array_info[info_positon],content_out);
		save_picture(mp3_path,file_pos_read_charset,content_out,*now_pos,save_path_out);
	} else {
		LSD_INFO("%s : %s\n",FrameID_array_info[info_positon],content_out);
		if( !strncmp(FrameID_array_info[info_positon],"标题",strlen("标题")))
		{
			insert_jscon_content(g_array,&g_obj,"title",content_out);
			strncpy(*title,"title",strlen("title"));
			//insert_jscon_content(g_array,&g_obj,"picpath","");
		}
		else if(!strncmp(FrameID_array_info[info_positon],"作者",strlen("作者")))
		{
			insert_jscon_content(g_array,&g_obj,"singer",content_out);
			strncpy(*singer,"singer",strlen("singer"));
			//insert_jscon_content(g_array,&g_obj,"picpath","");
		}
	}
	
	free(frame_content);
	frame_content = NULL;
	free(content_out);
	content_out = NULL;
	return 0;
}

//处理未知 MP3 信息
int deal_UNKNOWN_MP3_info(char *mp3_path) 
{
	if( g_array == NULL )
	{
		init_jscon_framework(&g_array);
	}
	add_jscon_content(g_array,g_obj,mp3_path,"","",NULL);
}	

//处理 ID3V1 版本相关信息
int deal_ID3V1_info( char *mp3_file_name )
{
	if( mp3_file_name == NULL )
	{
		LSD_ERROR("deal_ID3V1_info mp3_file_name = NULL\n");
		return -1;
	}
	
	p_MP3_ID3V1 id3v1_info = (p_MP3_ID3V1)malloc(sizeof(MP3_ID3V1));
	if(id3v1_info == NULL )
	{
		LSD_STDERROR("Malloc id3v1_info Error:%s\n",strerror(errno));
		return -1;
	}
	
	p_MP3_ID3V1 id3v1_info_out = (p_MP3_ID3V1)malloc(sizeof(MP3_ID3V1));
	if(id3v1_info_out == NULL )
	{
		LSD_STDERROR("Malloc id3v1_info Error:%s\n",strerror(errno));
		return -1;
	}
	
	memset(id3v1_info,0,sizeof(MP3_ID3V1));
	memset(id3v1_info_out,0,sizeof(MP3_ID3V1));
	
	read_mp3_ID3V1_info(id3v1_info,mp3_file_name);
	LSD_DEBUG("TAG : %s\n",id3v1_info->TAG);
	
	if(strlen(id3v1_info->Title) > 30) {
		id3v1_info->Title[30] = '\0';
	}
	else
	{
		int len = strlen(id3v1_info->Title);
		id3v1_info->Title[len] = '\0';
		memset(&id3v1_info->Title[len],0,(30-len));
	}
	
	if( strlen( id3v1_info->Artist ) > 30 ) {
		id3v1_info->Artist[30] = '\0';
	}
	else
	{
		int len = strlen(id3v1_info->Artist);
		id3v1_info->Artist[len] = '\0';
		memset(&id3v1_info->Artist[len],0,(30-len));
	}
	
	code_convert("gbk","utf-8",id3v1_info,sizeof(MP3_ID3V1),id3v1_info_out,sizeof(MP3_ID3V1));
	LSD_DEBUG("Title:%s  Artist:%s\n",id3v1_info_out->Title,id3v1_info_out->Artist);
	if( g_array == NULL )
	{
		init_jscon_framework(&g_array);
	}
	add_jscon_content(g_array,g_obj,mp3_file_name,id3v1_info_out->Title,id3v1_info_out->Artist,NULL);
	free(id3v1_info);
	free(id3v1_info_out);
	id3v1_info = NULL;
	id3v1_info_out = NULL;
	return 0;
}

//处理 ID3V2 版本文件相关信息
int deal_ID3V2_info( char * mp3_file_name )
{
	if( mp3_file_name == NULL )
	{
		LSD_ERROR("deal_ID3V1_info mp3_file_name = NULL\n");
		return -1;
	}
	
	int i = 0;
	int pos = 0;
	int frame_len = read_back_ID3V2_note_size(mp3_file_name);
	LSD_DEBUG("frame_len = %d\n",frame_len);
	
	
	if( g_array == NULL )
	{
		init_jscon_framework(&g_array);
	}
	
	cJSON_AddItemToArray(g_array,g_obj=cJSON_CreateObject());
	cJSON_AddItemToObject(g_obj,"name",cJSON_CreateString(mp3_file_name));
	//insert_jscon_content(g_array,&g_obj,"singer","");
	//insert_jscon_content(g_array,&g_obj,"title","");
	char *save_path_out = (char*)malloc(sizeof(char)*BUF_SIZE);
	if( save_path_out == NULL )
	{
		LSD_STDERROR("Malloc save_path_out Error:%s\n",strerror(errno));
		return -1;
	}
	char *title = (char*)malloc(sizeof(char)*strlen("title"));
	if( title == NULL )
	{
		LSD_STDERROR("Malloc title Error:%s\n",strerror(errno));
	}
	
	char *singer = (char*)malloc(sizeof(char)*strlen("singer"));
	if( title == NULL )
	{
		LSD_STDERROR("Malloc singer Error:%s\n",strerror(errno));
	}
	
	memset(save_path_out,0,BUF_SIZE);
	memset(title,0,strlen("title"));
	memset(singer,0,strlen("singer"));
	
	for( i = 0; pos < frame_len; i++ )
	{
		//读取并且处理数据，将每次的位置返回
		read_mp3_ID3VX_info_size(mp3_file_name,pos,&pos,&save_path_out,&title,&singer);
	}
	
	if( strlen(title) < strlen("title") )
	{
		insert_jscon_content(g_array,&g_obj,"title","");
	}
	
	if( strlen(singer) < strlen("singer") )
	{
		insert_jscon_content(g_array,&g_obj,"singer","");
	}
	
	if( strlen(save_path_out) < 4 )
		insert_jscon_content(g_array,&g_obj,"picpath","");
	else
		insert_jscon_content(g_array,&g_obj,"picpath",save_path_out);
	free(save_path_out);
	LSD_INFO("\n");
	return 0;
}

//处理 JSON 数据
int deal_with_JSON()
{
	save_json_content(g_array);
	delete_json(g_array);
}
