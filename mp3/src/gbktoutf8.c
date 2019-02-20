#include <stdio.h>
#include "gbktoutf8.h"
#include <iconv.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include "LSD_LOG.h"

int code_convert(char *from_charset,char *to_charset,char *inbuf, size_t inlen,char *outbuf, size_t outlen)
{
	if( from_charset == NULL || to_charset == NULL || inbuf == NULL || outbuf == NULL )
	{
		LSD_ERROR("Input Error in code_convert!\n");
		return -1;
	}
	iconv_t cd;
	char **pin = &inbuf;
	char **pout = &outbuf;
	
	cd = iconv_open(to_charset, from_charset);
	if (cd == 0)
	{
		LSD_STDERROR("Open iconv_open Error:%s\n",strerror(errno));
		return -1;
	}
	memset(outbuf, 0, outlen);
	if (iconv(cd, pin, &inlen, pout, &outlen) == -1)
		return -1;
	iconv_close(cd);
	*pout = '\0';
	return 0;
}