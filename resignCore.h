#ifndef _RESIGNCORE_H_
#define _RESIGNCORE_H_

enum ErrorCode{
	ERRORNULL,		//无错误
	NOIDENTITY, 	//暂无可用证书
	IDENTITYNULL,	//无效证书
	ERRORDIR,		//无权限
	ERRORCP,		//拷贝文件失败
	ERRORRENAME,	//重命名失败
};

typedef struct resignError
{
	enum ErrorCode errorCode;
	char *errorMsg;
}ERROR;

void showAllIdentity();
ERROR * findIdentity(char *identity);
ERROR * startResigned(char * ipa,char *identity,char * embedded,char *output);
char * appendPayloadCmd(char * ipa);
char * getPayloadAppPath();
int copyFile(char * file1 , char * dir);


void resignHelp();
#endif
