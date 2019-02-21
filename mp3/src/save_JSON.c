#include "cJSON.h"
#include "LSD_LOG.h"
#include <stdio.h>
#include <string.h>
#include "save_json.h"
#include <errno.h>
#include <stdlib.h>

//初始化JSON框架
int init_jscon_framework(cJSON **json,cJSON **array)
{
	//先创建空对象
	*json = cJSON_CreateObject();
	//在对象上添加键值对
	cJSON_AddItemToObject(*json,"mp3",*array=cJSON_CreateArray());
	return 0;
}

//添加JSON信息内容
int add_jscon_content(cJSON *array,cJSON *obj,char *mp3_name,char *title,char *singer,char *pic_path)
{
	if( array == NULL )
	{
		LSD_ERROR("array or obj is NULL!\n");
		return -1;
	}
	
	cJSON_AddItemToArray(array,obj=cJSON_CreateObject());
	cJSON_AddItemToObject(obj,"name",cJSON_CreateString(mp3_name));
	if(title == NULL || strlen(title) < 2) 
	{
		cJSON_AddStringToObject(obj,"title","");
	}
	else
	{
		cJSON_AddStringToObject(obj,"title",title);
	}
	
	if( singer == NULL || strlen(singer) < 2 )
	{
		cJSON_AddStringToObject(obj,"singer","");
	}
	else
	{
		cJSON_AddStringToObject(obj,"singer",singer);
	}
	
	if( pic_path == NULL )
	{
		cJSON_AddStringToObject(obj,"picpath","");
	}
	else
	{
		cJSON_AddStringToObject(obj,"picpath",pic_path);
	}
	return 0;
}

//保存 JSON 信息内容
int save_json_content(cJSON *json)
{
	if( json == NULL )
	{
		LSD_ERROR("param json is NULL!\n");
		return -1;
	}
	
	LSD_ERROR("%s\n",cJSON_Print(json));
	
	FILE *json_fp = fopen(JSON_SAVE_PATH,"w+");
	if( json_fp == NULL )
	{
		LSD_STDERROR("Open %s Error:%s",JSON_SAVE_PATH,strerror(errno));
		return -1;
	}
	
	char *buf = cJSON_Print(json);
	
	fwrite(buf,strlen(buf),1,json_fp);
	fclose(json_fp);
	return 0;
}

//插入JSON信息内容
int insert_jscon_content(cJSON* array,cJSON** obj,const char *item,char *content)
{
	if( array == NULL || item == NULL || obj == NULL )
	{
		LSD_ERROR("params is NULL!\n");
		return -1;
	}
	
	cJSON_AddStringToObject(*obj,item,content);
	return 0;
}

//删除 JSON 缓存
int delete_json(cJSON *json)
{
	if( json != NULL )
	{
		cJSON_Delete(json);
	}
}
