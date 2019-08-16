#ifndef _RESIGNCORE_H_
#define _RESIGNCORE_H_

enum ErrorCode{
	ERRORNULL,		//无错误
	NOIDENTITY, 	//暂无可用证书
	IDENTITYNULL,	//无效证书
	ERRORDIR,		//无权限
	ERRORCP,		//拷贝文件失败
	ERRORRENAME,	//重命名失败
	ERRORCREATE,	//创建文件失败
	ERRORRESIGN,	//重签名失败
	ERRORRCLEAN,	//清空缓存失败
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
int createPlist(char *empath,char * plistpath);
int resignFile(char * identity,char * appPath);
char * signCmd(char * appPath);
int resignApp(char * identity,char * plistPath,char * appPath);
void resignHelp();
int package(char * ipa,char * output);
int rmTempAppPath();
#endif
