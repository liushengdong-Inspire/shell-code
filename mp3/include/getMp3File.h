#ifndef __GET_MP3_FILE_H__
#define __GET_MP3_FILE_H__

#define MP3_FILE_NAME_LEN 2048

typedef struct STRUCT_MP3_FILE_NAME_LIST
{
	char mp3_name[MP3_FILE_NAME_LEN];//文件名
	struct STRUCT_MP3_FILE_NAME_LIST *next;
}MP3_FILE_NAME_LIST,*p_MP3_FILE_NAME_LIST;

//初始化文件名称列表
void init_mp3_file_name_list(p_MP3_FILE_NAME_LIST *header);

//添加文件名
int add_mp3_file_name(char *mp3_file_name,p_MP3_FILE_NAME_LIST *header);

//展示 MP3 文件名
int show_mp3_file_name(p_MP3_FILE_NAME_LIST *header);

//删除 MP3 文件名
int delete_mp3_file_name(p_MP3_FILE_NAME_LIST *header);

//获取当前目录下的所有 MP3 文件
int open_current_dir(char *dir_path,p_MP3_FILE_NAME_LIST *header);

//对外接口，获取 MP3 文件列表
int get_directory_mp3_file_list(char *dir_path,p_MP3_FILE_NAME_LIST *header);

//对外接口，释放 MP3 文件列表
int delete_directory_mp3_file_list(p_MP3_FILE_NAME_LIST header);

#endif //__GET_MP3_FILE_H__