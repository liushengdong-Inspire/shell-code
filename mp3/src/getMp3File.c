#include <stdio.h>
#include "getMp3File.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
//初始化 MP3 文件名称列表
void init_mp3_file_name_list(p_MP3_FILE_NAME_LIST *header)
{
	if( *header == NULL || header == NULL )
	{
		*header = (p_MP3_FILE_NAME_LIST)malloc(sizeof(MP3_FILE_NAME_LIST));
		if( *header == NULL )
		{
			fprintf(stderr,"Malloc header Error:%s\n",strerror(errno));
			return;
		}
		(*header)->next = NULL;
	}
}

//添加文件名
int add_mp3_file_name(char *mp3_file_name,p_MP3_FILE_NAME_LIST *header)
{
	if( mp3_file_name == NULL )
	{
		printf("add_mp3_file_name mp3_file_name = Null\n");
		return -1;
	}
	
	if( *header == NULL )
	{
		printf("header is Null,Please init first!\n");
		return -1;
	}
	
	p_MP3_FILE_NAME_LIST second = *header;
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
int show_mp3_file_name(p_MP3_FILE_NAME_LIST *header)
{
	if( *header == NULL || (*header)->next == NULL )
	{
		printf("header is Null in show_mp3_file_name,Please init and add file name first\n");
		return -1;
	}
	
	p_MP3_FILE_NAME_LIST second = (*header)->next;
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
int delete_mp3_file_name(p_MP3_FILE_NAME_LIST *header)
{
	if( *header == NULL || (*header)->next == NULL )
	{
		printf("header is Null in show_mp3_file_name,Please init and add file name first\n");
		return -1;
	}
	
	p_MP3_FILE_NAME_LIST second = (*header)->next;
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
	free(*header);
	*header = NULL;
	return 0;
}


//打开 dir_path 目录
int open_current_dir(char *dir_path,p_MP3_FILE_NAME_LIST *header)
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
			if(save_mp3_file_name(current_dir->d_name,header) == -1)
				return -1;
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
int save_mp3_file_name(char *file_name,p_MP3_FILE_NAME_LIST *header)
{
	if( file_name == NULL )
	{
		printf("save_mp3_file_name file_name = Null\n");
		return -1;
	}
	if(*header == NULL || header == NULL )
		init_mp3_file_name_list(header);
	add_mp3_file_name(file_name,header);
}

//对外接口，获取 MP3 文件列表
int get_directory_mp3_file_list(char *dir_path,p_MP3_FILE_NAME_LIST *header)
{
	int ret = -1;
	ret = open_current_dir(dir_path,header);
	if( ret == -1 )
	{
		return -1;
	}
	return 0;
}

//对外接口，释放 MP3 文件列表
int delete_directory_mp3_file_list(p_MP3_FILE_NAME_LIST *header)
{
	delete_mp3_file_name(header);
	return 0;
}

