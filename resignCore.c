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


    int status =  rmTempAppPath();
    ERROR * terror = (ERROR *)malloc(sizeof(ERROR));
    if (status != 0)
    {
        terror->errorCode = ERRORRCLEAN;
        terror->errorMsg = "--清空缓存失败--\n";
       goto close;
    }

    char * unzipCmd = appendPayloadCmd(ipa);
    status = system(unzipCmd);
    if (status == 0)
    {
       printf("%s\n","Unzip Payload...");
    }else{
       perror("system()");
    }

    //1先将描述文件cpoy到app中
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
    char * environmentPlistPath = "~/resign_temp_app/Payload/entitlements.plist";
    status = createPlist(embeddedAppPath,environmentPlistPath);
    if (status != 0)
    {
       terror->errorCode = ERRORCREATE;
       terror->errorMsg = "--创建描述文件失败--\n";
       goto close;
    }else{
        printf("%s\n","Create Plist...");
    }
    
    printf("%s\n","---------- 开始重签名 ---------");
    //开始进行二进制文件，库文件签名
    char * appMachO = getPayloadAppPath(1);
    char * machPath =  (char *)malloc(strlen(appPath) + strlen(appMachO)+1);
    sprintf(machPath,"%s/%s",appPath,appMachO);
    status = resignFile(identity,machPath);
    if (status != 0)
    {
       terror->errorCode = ERRORRESIGN;
       terror->errorMsg = "--文件重签名失败--\n";
       goto close;
    }

    char * findCmd = signCmd(appPath);
    FILE * fp = popen(findCmd,"r");
    char line[_LINE_LENGTH];
    while (fgets(line, _LINE_LENGTH, fp) != NULL){
         status =  resignFile(identity,line);
         if (status != 0)
         {
            terror->errorCode = ERRORRESIGN;
            terror->errorMsg = "--文件重签名失败--\n";
            goto close;
        }
    }
    pclose(fp);
    status = resignApp(identity,environmentPlistPath,appPath);
    if (status != 0)
    {
        terror->errorCode = ERRORRESIGN;
        terror->errorMsg = "--文件重签名失败--\n";
       goto close;
    }else{
        printf("%s\n","---------- 开始打包 ---------");
    }

    //打包
    status = package(ipa,output);
    if (status != 0)
    {
        terror->errorCode = ERRORRESIGN;
        terror->errorMsg = "--文件打包失败--\n";
       goto close;
    }
       
    status =  rmTempAppPath();
    if (status != 0)
    {
        terror->errorCode = ERRORRCLEAN;
        terror->errorMsg = "--清空缓存失败--\n";
       goto close;
    }


    terror->errorCode = ERRORNULL;
close:
    free(terror);
    return terror;
}

int rmTempAppPath(){
   int status = system("rm -rf ~/resign_temp_app");
   if (status != 0)
   {
       return -1;
   }
   return 0;
}


int package(char * ipa,char * output){
    char * cd = "cd ~/resign_temp_app/ && ";
    char * zip = "zip -qr ";
    char * zipPath = "Payload";
    char * ipaFix = "_resigned.ipa\0";
    char oldIpa[200];
    stpcpy(oldIpa,ipa);
    char * out = NULL;
    char * lastName = NULL;
    out = strtok(ipa,"/");
        while (out != NULL){
              out = strtok(NULL,"/");
              if (out != NULL)
              {
                  lastName = out;
              }
        }
    char currentPath[200];
    int n = strlen(oldIpa)-strlen(lastName);
    memcpy(currentPath,oldIpa,n);
    currentPath[n] = '\0';
    lastName = strtok(lastName,".");

    if (output != NULL){
        char * outlastChar = &(output[strlen(output)-1]);
        int outStatus = memcmp("/",outlastChar,1);
        if (outStatus == 0 )
        {
            char * outIpa = (char *) malloc(strlen(output)+strlen(lastName)+strlen(ipaFix));
            sprintf(outIpa,"%s%s%s",output,lastName,ipaFix);
            out = outIpa;
        }else{
            char * outIpa = (char *) malloc(strlen(output)+strlen(lastName)+strlen(ipaFix)+1);
            sprintf(outIpa,"%s/%s%s",output,lastName,ipaFix);
            out = outIpa;
        }
    }else{
        char * outIpa = (char *) malloc(strlen(currentPath)+strlen(lastName)+strlen(ipaFix));
        sprintf(outIpa,"%s%s%s",currentPath,lastName,ipaFix);
        out = outIpa;
    }
    char * packageCmd = malloc(strlen(cd)+strlen(zip)+strlen(out)+strlen(zipPath)+10);

    sprintf(packageCmd,"%s %s %s %s",cd,zip,out,zipPath);
    int status = system(packageCmd);
    if (status != 0)
    {
        return -1;
    }else{
        printf("%s%s\n","新包路径：",out);
    }

    return 0;
}

char * signCmd(char * appPath){
    char * find = "find ";
    char * dir = " -name \"*\" | grep -e .framework -e .dylib";
    char * signCmd =  (char *)malloc(strlen(find) + strlen(appPath) + strlen(dir));
    sprintf(signCmd,"%s%s%s",find,appPath,dir);
    return signCmd;
}

int resignApp(char * identity,char * plistPath,char * appPath){
    char * code = "/usr/bin/codesign --continue -f -s";
    char * entitlements = "--entitlements";
    char * signCmd =  (char *)malloc(strlen(code) + strlen(identity) + strlen(entitlements) + strlen(plistPath) + strlen(appPath) + 10);
    sprintf(signCmd,"%s \"%s\" %s %s %s",code,identity,entitlements,plistPath,appPath);
    int status = system(signCmd);
     if (status != 0)
     {
        free(signCmd);
        perror("system()");
        return -1;
     }
     // free(signCmd);

    return 0;
}

int resignFile(char * identity,char * path){
     // puts(path);
     char * cmd = "/usr/bin/codesign -fs"; //codesign -fs "iPhone Developer: xxx " xxx.framework
     char * codesignCmd =  (char *) malloc(strlen(cmd) + strlen(identity) + strlen(path)+10);
     sprintf(codesignCmd,"%s \"%s\" %s",cmd,identity,path);
     int status = system(codesignCmd);
     if (status != 0)
     {
        free(codesignCmd);
        perror("system()");
        return -1;
     }
     free(codesignCmd);
    return 0;
}

int createPlist(char *empath,char * plistpath){
    char * fullPlisyCmd = "security cms -D -i ";
    char * fullPlist = " ~/resign_temp_app/Payload/entitlements_full.plist";
    char * cmd =  (char *) malloc(strlen(fullPlisyCmd) + strlen(empath) + strlen(fullPlist)+10);
    sprintf(cmd,"%s%s>%s",fullPlisyCmd,empath,fullPlist);
    int status = system(cmd);
    if (status != 0)
    {
        free(cmd);
        perror("system()");
        return -1;
    }

    char * plistCmd = "/usr/libexec/PlistBuddy -x -c \"Print:Entitlements\"";
    char * createPlistCmd =  (char *) malloc(strlen(plistCmd) + strlen(fullPlist) + strlen(plistpath)+10);
    sprintf(createPlistCmd,"%s%s>%s",plistCmd,fullPlist,plistpath);
    status = system(createPlistCmd);
    if (status != 0)
    {
        free(createPlistCmd);
        perror("system()");
        return -1;
    }
    return 0;
}


char * appendPayloadCmd(char * ipa){
    char * mkCmd = "mkdir -pv ";
    char * unzipCmd1  = " && unzip -o -q ";
    char * unzipCmd2  = " -d ";
    char * payloadCmd = (char *) malloc(strlen(mkCmd) + strlen(unzipCmd1) + strlen(unzipCmd2) + strlen(payloadPath)*2 + strlen(ipa) + 10);
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
