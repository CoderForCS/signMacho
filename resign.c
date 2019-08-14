#include <stdio.h>
#include <string.h>
#include "resignCore.h"

int main(int argc, char *argv[]){

//resign -i "/Desktop/myapp/sign.ipa" -c "XXXXCo., Ltd." -e "/Desktop/myapp/embedded.mobileprovision" -o "/Desktop"
	int j = 0;
	char * ipaPtah = NULL;
	char * identity= NULL;
	char * embedded= NULL;
	char * output  = NULL;
	while(j<argc){

		if (strcmp(argv[j],"-h") == 0 || argc == 1)
		{
			resignHelp();
			return -1;
		}

		if (strcmp(argv[j],"-s") == 0)
		{
			showAllIdentity();
			return -1;
		}

		//后跟ipa包路径
		if (strcmp(argv[j],"-i") == 0)
		{
			FILE *fp = fopen(argv[j+1],"r");
			if (fp == NULL)
			{
				printf("%s\n","Error file,Not find ipa.");
				return -1;
			}else{
				fclose(fp);
				printf("%s\n","Find this ipa..\nStart find P12...");
				ipaPtah = argv[j+1];
			}
		}

		//后跟证书名字
		if (strcmp(argv[j],"-c") == 0)
		{
			ERROR * error = findIdentity(argv[j+1]);
			if (error->errorCode == ERRORNULL)
			{
				printf("P12 Ready: %s\n", argv[j+1]);
				identity = argv[j+1];
			}else{
				printf("%s\n",error->errorMsg);
				return -1;
			}
		}

		//后跟描述文件路径
		if (strcmp(argv[j],"-e") == 0)
		{
			FILE *fp = fopen(argv[j+1],"r");
			if (fp == NULL)
			{
				printf("%s\n","Error file,Not find embedded.");
				return -1;
			}else{
				fclose(fp);
				printf("%s\n","Find this embedded...");
				embedded = argv[j+1];
			}
		}

		//后跟输出路径
		if (strcmp(argv[j],"-o") == 0)
		{
			output = argv[j+1];
		}
		j++;
	}
	printf("%s\n","------🔥. 开始签名 ------");
	ERROR * error = startResigned(ipaPtah,identity,embedded,output);
	if (error->errorCode != ERRORNULL)
	{
		printf("%s\n",error->errorMsg);
	}


	return 0;
}