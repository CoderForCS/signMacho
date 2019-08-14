#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "resignCore.h"

#define _LINE_LENGTH 300
char * payloadPath = "~/resign_temp_app/";

void showAllIdentity(){
	char *identityCmd = "security find-identity -p codesigning -v";
	int status = system(identityCmd);
	if (status != 0)
	{
		puts("cmd fiale! Please check current environment.\n");
	}
}

void resignHelp(){
      fprintf(stderr,
            "resign %s\n"
            "Usage: resign -i <ipa path> -c <identity name> -e <profile path> [options]\n"
            "\n"
            "  where options are:\n"
            "        -s             show current can sign Identity.\n"
            "        -i             need sign ipa path.\n"
            "        -c             sign app identity name.\n"
            "        -e             mobileprovisio path.\n"
            "        -o             output resign ipa path.\n"
            "\n"
            "  Example:\n"
            "       resign -i /Desktop/myapp/sign.ipa -c \"XXXXCo., Ltd.\" -e /Desktop/myapp/embedded.mobileprovision -o /Desktop"
            "\n"
            ,
            "version 1.0 Help(" __DATE__ " " __TIME__ ")"
       );
}

ERROR * findIdentity(char *identity){
	char *identityCmd = "security find-identity -p codesigning -v";
	FILE* fp = popen(identityCmd,"r");
	char line[_LINE_LENGTH];
	ERROR * terror = (ERROR *)malloc(sizeof(ERROR));
	if (fp == NULL)
	{
		terror->errorCode = NOIDENTITY;
		terror->errorMsg = "Not Find Identity!\n----暂无可用证书---\n";
        goto close;
	}else{
        while (fgets(line, _LINE_LENGTH, fp) != NULL)
        {
            char * status = strstr(line,identity);
            if (status != NULL)
            {
            	terror->errorCode = ERRORNULL;
            	goto close;
            }
        }
    }
    terror->errorCode = IDENTITYNULL;
    terror->errorMsg = "This P12 can't signed!\n----无效的证书---\n";
close:    
	pclose(fp);
	free(terror);
    return terror;
}


ERROR * startResigned(char * ipa,char *identity,char * embedded,char *output){
    char * unzipCmd = appendPayloadCmd(ipa);

   int status = system(unzipCmd);
   if (status == 0)
   {
       printf("%s\n","Unzip Payload...");
   }else{
       perror("system()");
   }

    //1先将描述文件cpoy到app中
    ERROR * terror = (ERROR *)malloc(sizeof(ERROR));
    char * appPath = getPayloadAppPath();
    char * name = "/embedded.mobileprovision";
    char * embeddedAppPath =  (char *) malloc(strlen(appPath) + strlen(name));
    sprintf(embeddedAppPath,"%s%s",appPath,name);
    int cpStatus = copyFile(embedded,embeddedAppPath);
    if (cpStatus != 0)
    {
       terror->errorCode = ERRORCP;
       terror->errorMsg = "--拷贝描述文件失败--\n";
       goto close;
    }else{
        printf("%s\n","copy embedded...");
    }
    //2生成plist文件
    // security cms -D -i 








    terror->errorCode = ERRORNULL;
close:
    free(terror);
    return terror;
}


char * appendPayloadCmd(char * ipa){
    char * mkCmd = "mkdir -pv ";
    char * unzipCmd1  = " && unzip -o -q ";
    char * unzipCmd2  = " -d ";
    char * payloadCmd = (char *) malloc(strlen(mkCmd) + strlen(unzipCmd1) + strlen(unzipCmd2) + strlen(payloadPath)*2 + strlen(ipa));
    sprintf(payloadCmd, "%s%s%s%s%s%s", mkCmd, payloadPath,unzipCmd1,ipa,unzipCmd2,payloadPath);
    free(payloadCmd);
    return payloadCmd;
}

char * getPayloadAppPath(){
    char * lsCmd = "ls ~/resign_temp_app/Payload/ | grep .app";
    FILE* fp = popen(lsCmd,"r");
    if (fp == NULL)
    {
      return NULL;
    }
     char * appName;
     char line[_LINE_LENGTH];
     while (fgets(line, _LINE_LENGTH, fp) != NULL){
        appName = strtok(line,"\n");
     }
     char * payload = "Payload/";
     char * appPath =  (char *) malloc(strlen(payloadPath) + strlen(payload) + strlen(appName));
     sprintf(appPath,"%s%s%s",payloadPath,payload,appName);
     pclose(fp);
     return appPath;
 }

int copyFile(char * file1 , char * dir){
    char * cp = "cp";
    char * cpCmd =  (char *) malloc(strlen(cp)*2 + strlen(file1) + strlen(dir));
    sprintf(cpCmd,"%s %s %s",cp,file1,dir);
    int status = system(cpCmd);
    if (status != 0)
    {
      perror("system()");
      return -1;
    }
    free(cpCmd);
    return 0;
}
