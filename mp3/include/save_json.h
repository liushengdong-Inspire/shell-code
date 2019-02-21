#ifndef __SAVE_JSON_H__
#define __SAVE_JSON_H__
#include "cJSON.h"
#include "cJSON_Utils.h"

#define JSON_SAVE_PATH "./mp3_json.xml"

static cJSON *g_json;
static cJSON *g_array;
static cJSON *g_obj;

//初始化JSON框架
int init_jscon_framework(cJSON **json,cJSON **array);

//添加JSON信息内容
int add_jscon_content(cJSON *array,cJSON *obj,char *mp3_name,char *title,char *singer,char *pic_path);

//插入JSON信息内容
int insert_jscon_content(cJSON* array,cJSON** obj,char *item,char *content);

//保存 JSON 信息内容
int save_json_content(cJSON *json);

//删除 JSON 缓存
int delete_json(cJSON *json);

#endif //__SAVE_JSON_H__