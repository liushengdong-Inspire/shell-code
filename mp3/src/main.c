#include <stdio.h>
#include "mp3.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "gbktoutf8.h"
#include "getMp3File.h"
#include <stdlib.h>
#include "LSD_LOG.h"

int main(int argc,char *arv[])
{
	p_MP3_FILE_NAME_LIST header = NULL;
	if(get_directory_mp3_file_list("./",&header) == -1)
		return -1;
	if(header == NULL )
	{
		LSD_ERROR("head is NULL!Please check!\n");
		return -1;
	}
	//show_mp3_file_name(&header);
	
	//拿出 MP3 文件名称
	p_MP3_FILE_NAME_LIST second = header->next;
	int i = 0;
	while( second != NULL )
	{
		int type = mp3_type(second->mp3_name);
		LSD_INFO("%s\n",second->mp3_name);
		if( type == -1 || type == 0 )
		{
			LSD_INFO("UNKNOWN!\n");
		}
		else if( type == 1 )
		{
			deal_ID3V1_info(second->mp3_name);
		}
		else if ( type == 2 )
		{
			deal_ID3V2_info(second->mp3_name);
		}
		second = second->next;
		i ++;
	}
	LSD_INFO("Toast deal with %d files\n",i);
	delete_directory_mp3_file_list(&header);
	return 0;
}
