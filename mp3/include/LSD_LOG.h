#ifndef __LSD_LOG_H_
#define __LSD_LOG_H_

//全日志开启
#define DEBUG 0
//错误日志
#define ERROR_DEBUG 0
//info 日志
#define INFO_DEBUG 0
//WARN 日志
#define WARN_DEBUG 0
//STD 日志
#define STD_DEBUG 0
//DEBUG 日志
#define DEBUG_LOG 0

#define ESC_START     "\033["
#define ESC_END       "\033[0m"
#define COLOR_FATAL   "31;40;5m"
#define COLOR_ALERT   "31;40;1m"
#define COLOR_CRIT    "31;40;1m"
#define COLOR_ERROR   "31;40;1m"
#define COLOR_WARN    "33;40;1m"
#define COLOR_NOTICE  "34;40;1m"
#define COLOR_INFO    "32;40;1m"
#define COLOR_DEBUG   "36;40;1m"
#define COLOR_TRACE   "37;40;1m"

//全开启
#if DEBUG
#define LSD_INFO(format, args...) (printf( ESC_START COLOR_INFO "[INFO]-[%s]-[%d]:" format ESC_END, __FILE__ , __LINE__, ##args))
#define LSD_DEBUG(format, args...) (printf( ESC_START COLOR_DEBUG "[DEBG]-[%s]-[%d]:" format ESC_END, __FILE__ , __LINE__, ##args))
#define LSD_WARN(format, args...) (printf( ESC_START COLOR_WARN "[WARN]-[%s]-[%d]:" format ESC_END, __FILE__ , __LINE__, ##args))
#define LSD_ERROR(format, args...) (printf( ESC_START COLOR_ERROR "[ERRO]-[%s]-[%d]:" format ESC_END, __FILE__ , __LINE__, ##args))
#define LSD_STDERROR(format,args...) (fprintf(stderr,ESC_START COLOR_ERROR "[ERRO]-[%s]-[%d]:" format ESC_END,__FILE__,__LINE__,##args))
#else
//错误信息日志
#if ERROR_DEBUG
#define LSD_ERROR(format, args...) (printf( ESC_START COLOR_ERROR "[ERRO]-[%s]-[%d]:" format ESC_END, __FILE__, __LINE__, ##args))
#else
#define LSD_ERROR(format, args...)
#endif

//INFO 日志
#if INFO_DEBUG
#define LSD_INFO(format, args...) (printf( ESC_START COLOR_INFO "[INFO]-[%s]-[%d]:" format ESC_END, __FILE__, __LINE__, ##args))
#else
#define LSD_INFO(format, args...)
#endif

//WARN 日志
#if WARN_DEBUG
#define LSD_WARN(format, args...) (printf( ESC_START COLOR_WARN "[WARN]-[%s]-[%d]:" format ESC_END, __FILE__,__LINE__, ##args))
#else
#define LSD_WARN(format, args...)
#endif

//标准出错日志
#if STD_DEBUG
#define LSD_STDERROR(format,args...) (fprintf(stderr,ESC_START COLOR_ERROR "[ERRO]-[%s]-[%d]:" format ESC_END,__FILE__,__LINE__,##args))
#else
#define LSD_STDERROR(format,args...)
#endif

//DEBUG_LOG 日志
#if DEBUG_LOG
#define LSD_DEBUG(format, args...) (printf( ESC_START COLOR_DEBUG "[DEBG]-[%s]-[%d]:" format ESC_END, __FILE__, __LINE__, ##args))
#else
#define LSD_DEBUG(format, args...)
#endif

#endif

#endif//__LSD_LOG_H_