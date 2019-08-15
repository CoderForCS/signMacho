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
    char * appPath = getPayloadAppPath(0);
    char * name = "/embedded.mobileprovision";
    char * embeddedAppPath =  (char *) malloc(strlen(appPath) + strlen(name));
    sprintf(embeddedAppPath,"%s%s",appPath,name);
    status = copyFile(embedded,embeddedAppPath);
    if (status != 0)
    {
       terror->errorCode = ERRORCP;
       terror->errorMsg = "--拷贝描述文件失败--\n";
       goto close;
    }else{
        printf("%s\n","copy embedded...");
    }
    //2生成plist文件
    status = createPlist(embeddedAppPath,"~/resign_temp_app/Payload/entitlements.plist");
    if (status != 0)
    {
       terror->errorCode = ERRORCREATE;
       terror->errorMsg = "--创建描述文件失败--\n";
       goto close;
    }else{
        printf("%s\n","Create Plist...");
    }

    resignFile(identity,appPath);

    terror->errorCode = ERRORNULL;
close:
    free(terror);
    return terror;
}

int resignFile(char * identity,char * appPath){
    puts(identity);
    puts(appPath);
    char * appMachO = getPayloadAppPath(1);
    printf("%s\n",appMachO);
    // char * cmd = "codesign -fs";

    return 0;
}

int createPlist(char *empath,char * plistpath){
    char * fullPlisyCmd = "security cms -D -i ";
    char * fullPlist = " ~/resign_temp_app/Payload/entitlements_full.plist";
    char * cmd =  (char *) malloc(strlen(fullPlisyCmd) + strlen(empath) + strlen(fullPlist)+1);
    sprintf(cmd,"%s%s>%s",fullPlisyCmd,empath,fullPlist);
    int status = system(cmd);
    if (status != 0)
    {
        free(cmd);
        perror("system()");
        return -1;
    }
    free(cmd);

    char * plistCmd = "/usr/libexec/PlistBuddy -x -c \"Print:Entitlements\"";
    char * createPlistCmd =  (char *) malloc(strlen(plistCmd) + strlen(fullPlist) + strlen(plistpath)+1);
    sprintf(createPlistCmd,"%s%s>%s",plistCmd,fullPlist,plistpath);
    status = system(createPlistCmd);
    if (status != 0)
    {
        free(createPlistCmd);
        perror("system()");
        return -1;
    }
    free(createPlistCmd);
    return 0;
}


char * appendPayloadCmd(char * ipa){
    char * mkCmd = "mkdir -pv ";
    char * unzipCmd1  = " && unzip -o -q ";
    char * unzipCmd2  = " -d ";
    char * payloadCmd = (char *) malloc(strlen(mkCmd) + strlen(unzipCmd1) + strlen(unzipCmd2) + strlen(payloadPath)*2 + strlen(ipa));
    sprintf(payloadCmd, "%s%s%s%s%s%s", mkCmd, payloadPath,unzipCmd1,ipa,unzipCmd2,payloadPath);
    // free(payloadCmd);
    return payloadCmd;
}

char * getPayloadAppPath(int status){
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

     if (status == 1)
     {
        appName = strtok(appName,".");
        char * appNames = (char *) malloc(strlen(appName));
        sprintf(appNames,"%s",appName);
        return appNames;
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
      free(cpCmd);
      perror("system()");
      return -1;
    }
    free(cpCmd);
    return 0;
}
