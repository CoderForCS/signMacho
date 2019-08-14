#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "resignCore.h"

#define _LINE_LENGTH 300


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
            "  As:\n"
            "       resign -i \"/Desktop/myapp/sign.ipa\" -c \"XXXXCo., Ltd.\" -e \"/Desktop/myapp/embedded.mobileprovision\" -o \"/Desktop\""
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
    
    

}
