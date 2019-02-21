#ifndef __MP3_H__
#define __MP3_H__

#define MP3_TYPE_SIZE 3
#define MP3_ID3V1_INFO_LENTH 128
#define BUF_SIZE 2048
#define PIC_SAVE_PATH "./picture/"

#include <stdio.h>

//ID3V2 帧的ID 信息
static char* FrameID_array[72] = {"AENC","APIC","COMM","COMR","ENCR","ETC0","GEOB","GRID","IPLS",
								"MCDI","MLLT","OWNE","PRIV","PCNT","POPM","POSS","RBUF","RVAD","RVRB",
								"SYLT","SYTC","TALB","TBPM","TCOM","TCON","TCOP","TDAT","TDLY","TENC",
								"TEXT","TELT","TIME","TIT1","TIT2","TIT3","TKEY","TLAN","TLEN","TMED",
								"TOAL","TOFN","TOLY","TOPE","TPRY","TOWM","TPE1","TPE2","TPE3","TPE4",
								"TPOS","TPUB","TRCK","TRDA","TRSN","TRSO","TSIZ","TSRC","TSSE","TYER",
								"TXXX","UFID","USER","USLT","WCOM","WCOP","WOAF","WOAR","WOAS","WORS",
								"WPAY","WPUB","WXXX"};
//帧 ID 信息对应表示的内容
static char *FrameID_array_info[72] = {"音频加密技术","附加描述","注释","广告","加密方法注册","事件时间编码",
								"常规压缩对象","组识别注册","复杂类别列表","音乐CD标识符","MPEG位置查找表格",
								"所有权","私有","播放计数","普通仪表","位置同步","推荐缓冲区大小","音量调节器",
								"混响","同步歌词或文本","同步节拍编码","专辑","每分钟节拍数","作曲家","流派",
								"版权","日期","播放列表返录","编码","歌词作者","文件类型","时间","内容组描述",
								"标题","副标题","最初关键字","语言","长度","媒体类型","原唱片集","原文件名","原歌词作者",
								"原艺术家","最初发行年份","文件所有者","艺术家","乐队","指挥者","翻译","作品集部分","发行人",
								"音轨","录制日期","电台名","电台所有者","大小","国际的标准记录代码","硬件设置","年代","年度",
								"唯一的文件标识符","使用条款","歌词","广告信息","版权信息","官方音频文件网页","官方艺术家网页",
								"官方音频原始资料网页","官方互联网无线配置首页","付款","出版商官方网页","用户定义的URL链接"};


//ID3V1 以字符串 “TAG” 为标识，固定长度是在 MP3 文件末尾 128 字节处
typedef struct STRUCT_MP3_ID3V1
{
	char TAG[3];		//文件头标识，TAG
	char Title[30]; 	//歌曲名
	char Artist[30];	//歌手名
	char Album[30];		//专辑名
	char Year[4];		//日期信息
	char Comment[28];	//注释信息，有时会是 30 字节
	char Reserved;		//音轨，0 表示有音轨，否则表明注释有 30 个字节
	char Track;			//音轨
	unsigned char Genre;		//歌曲风格
}MP3_ID3V1,*p_MP3_ID3V1;

/* ID3X 由一个标签头和如干个标签帧组成
 * 文件前 10 个字节为整个文件的
*/
//ID3X 标签头
typedef struct STRUCT_MP3_ID3X_HEADER
{
	char Header[3];				//文件标识
	char Ver;					//主版本号
	char Revision;				//副版本号
	char Flags;
	unsigned char Size[4];		//标签大小，表示标签的大小
}MP3_ID3X_HEADER,*p_MP3_ID3X_HEADER;

//ID3X 标签帧
typedef struct STRUCT_MP3_ID3X_INFO
{
	char FrameID[4];			//说明此帧的内容，是标题还是其他内容
	unsigned char Size[4];		//不包括帧的大小的帧内容的大小，即 FrameID 表示的内容的详细信息大小
	char Flags[2];				//存放标志
}MP3_ID3X_INFO;

//判断 MP3 文件是 ID3V1 还是 ID3V2
int mp3_type(char *mp3_path);

// 解析 MP3 ID3V1 相关内容
int read_mp3_ID3V1_info(p_MP3_ID3V1 id3v1_info,char *mp3_path);

// 获取 MP3 ID3V2 头部信息
int read_mp3_ID3VX_HEAD_info(FILE *fp,p_MP3_ID3X_HEADER mp3_id3x_info);

// 返回 MP3 ID3V2 标签大小
int read_back_ID3V2_note_size(char *mp3_path);

// 解析帧内容信息
int read_mp3_ID3VX_info_size(char *mp3_path,int current_pos,int *now_pos);

//处理 ID3V1 版本相关信息
int deal_ID3V1_info( char *mp3_file_name );

//处理 ID3V2 版本文件相关信息
int deal_ID3V2_info( char * mp3_file_name );

//获取帧内容大小
int get_frame_content_size(char *mp3_path,int current_pos);

//处理 JSON 数据
int deal_with_JSON();
#endif