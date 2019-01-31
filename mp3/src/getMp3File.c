#include <stdio.h>
#include "getMp3File.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
//初始化 MP3 文件名称列表
void init_mp3_file_name_list()
{
	if( G_MP3_LIST_HEADER == NULL )
	{
		G_MP3_LIST_HEADER = (p_MP3_FILE_NAME_LIST)malloc(sizeof(MP3_FILE_NAME_LIST));
		if( G_MP3_LIST_HEADER == NULL )
		{
			fprintf(stderr,"Malloc G_MP3_LIST_HEADER Error:%s\n",strerror(errno));
			return;
		}
		G_MP3_LIST_HEADER->next = NULL;
	}
}

//添加文件名
int add_mp3_file_name(char *mp3_file_name)
{
	if( mp3_file_name == NULL )
	{
		printf("add_mp3_file_name mp3_file_name = Null\n");
		return -1;
	}
	
	if( G_MP3_LIST_HEADER == NULL )
	{
		printf("G_MP3_LIST_HEADER is Null,Please init first!");
		return -1;
	}
	
	p_MP3_FILE_NAME_LIST second = G_MP3_LIST_HEADER;
	while( second->next != NULL )
	{
		second = second->next;
	}
	
	p_MP3_FILE_NAME_LIST third = (p_MP3_FILE_NAME_LIST)malloc(sizeof(MP3_FILE_NAME_LIST));
	if( third == NULL )
	{
		fprintf(stderr,"Malloc to add file name Error:%s\n",strerror(errno));
		return -1;
	}
	
	second->next = third;
	third->next = NULL;
	strncpy(third->mp3_name,mp3_file_name,MP3_FILE_NAME_LEN);
	return 0;
}

//展示 MP3 文件名
int show_mp3_file_name()
{
	if( G_MP3_LIST_HEADER == NULL || G_MP3_LIST_HEADER->next == NULL )
	{
		printf("G_MP3_LIST_HEADER is Null in show_mp3_file_name,Please init and add file name first\n");
		return -1;
	}
	
	p_MP3_FILE_NAME_LIST second = G_MP3_LIST_HEADER->next;
	int i = 0;
	while( second != NULL )
	{
		printf("%s\n",second->mp3_name);
		second = second->next;
		i ++;
	}
	printf("Toatal has %d file\n",i);
	return 0;
}

//删除 MP3 文件名
int delete_mp3_file_name()
{
	if( G_MP3_LIST_HEADER == NULL || G_MP3_LIST_HEADER->next == NULL )
	{
		printf("G_MP3_LIST_HEADER is Null in show_mp3_file_name,Please init and add file name first\n");
		return -1;
	}
	
	p_MP3_FILE_NAME_LIST second = G_MP3_LIST_HEADER->next;
	int i = 0;
	while( second != NULL )
	{
		p_MP3_FILE_NAME_LIST third = second->next;
		memset(second->mp3_name,0,MP3_FILE_NAME_LEN);
		second->next = NULL;
		free(second);
		second = third;
		i ++;
	}
	printf("success to delete %d file name!\n",i);
	free(G_MP3_LIST_HEADER);
	G_MP3_LIST_HEADER = NULL;
	return 0;
}


//打开 dir_path 目录
int open_current_dir(char *dir_path)
{
	if( dir_path == NULL )
	{
		printf("Input DIR path Null!\n");
		return -1;
	}
	
	DIR *mp3_DIR = opendir(dir_path);
	if( mp3_DIR == NULL )
	{
		fprintf(stderr,"Open DIR %s Error:%s\n",dir_path,strerror(errno));
		return -1;
	}
	
	struct dirent * current_dir = NULL;
	while((current_dir = readdir(mp3_DIR)) != NULL )
	{
		//去掉当前目录下的 . 和 ..
		if(!strncmp(current_dir->d_name,".",strlen(".")) || !strncmp(current_dir->d_name,"..",strlen("..")))
			continue;
		//判断是否是 MP3 文件
		if(is_MP3_FILE(current_dir->d_name) != -1)
		{
			save_mp3_file_name(current_dir->d_name);
		}
		
	}
	closedir(mp3_DIR);
	return 0;
}

//判断是否是 MP3 文件
int is_MP3_FILE(char *file_name)
{
	if( file_name == NULL )
	{
		printf("is_MP3_FILE file_name = Null\n");
		return -1;
	}
	int i = 0;
	while(file_name[i] != '\0' && file_name[i+1] != '\0' && file_name[i+2] != '\0' && file_name[i+3] != '\0' )
	{
		//确保后缀为 .mp3
		if( file_name[i] == '.' && file_name[i+1] == 'm' && file_name[i+2] == 'p' && file_name[i+3] == '3' && file_name[i+4] == '\0' )
		{
			return 0;
		}
		i ++;
	}
	return -1;
}

//保存 MP3 文件名
int save_mp3_file_name(char *file_name)
{
	if( file_name == NULL )
	{
		printf("save_mp3_file_name file_name = Null\n");
		return -1;
	}
	if(G_MP3_LIST_HEADER == NULL )
		init_mp3_file_name_list();
	add_mp3_file_name(file_name);
}

