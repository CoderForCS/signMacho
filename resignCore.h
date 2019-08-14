#ifndef _RESIGNCORE_H_
#define _RESIGNCORE_H_

enum ErrorCode{
	ERRORNULL,		//无错误
	NOIDENTITY, 	//暂无可用证书
	IDENTITYNULL,	//无效证书

};

typedef struct resignError
{
	enum ErrorCode errorCode;
	char *errorMsg;
}ERROR;

void showAllIdentity();
ERROR * findIdentity(char *identity);
ERROR * startResigned(char * ipa,char *identity,char * embedded,char *output);
void resignHelp();
#endif
