#ifndef __GBK_TO_UTF8_H__
#define __GBK_TO_UTF8_H__
int code_convert(char *from_charset,char *to_charset,char *inbuf, size_t inlen,char *outbuf, size_t outlen);
#endif