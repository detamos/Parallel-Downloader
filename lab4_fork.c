#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

long long toNum(char *str)
{
	int n = strlen(str);
	int i=0;
	long long ret = 0;
	while(i < n)
	{
		ret = ret*10 + str[i]-'0';
		i++;
	}
	return ret;
}	

void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 	if(num == 0)
 	{
 		strcpy(str,"0");
 		return ;
 	}
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

int main(int argc,char *argv[])
{
	if(argc != 5)
	{
		printf("Wrong number of arguments\n");
		exit(0);
	}
	char url[1000];
	strcpy(url,argv[1]);
	long long size = toNum(argv[2]);
	long long numProcesses = toNum(argv[3]);
	char outFile[1000];
	strcpy(outFile,argv[4]);

	long long bytes = size/numProcesses;
	long long total;
	char range[1000],part[1000];
	long long start = 0,end = bytes;
	int i;
	for(i=0;i<numProcesses;i++)
	{
		if(i == numProcesses-1)
			end = size;
		char numtochar[1000];
		tostring(numtochar,i+1);
		strcpy(part,"part");
		strcat(part,numtochar);
		char srange[1000],erange[1000];
		tostring(srange,start);
		tostring(erange,end);
		strcpy(range,srange);
		strcat(range,"-");
		strcat(range,erange);
	//	printf("%s %s\n",range,part);
		pid_t pid = fork();
		if(pid == 0)
		{
			char *args[] = {"curl","--range",range,"-o",part,url,NULL};
			execvp(args[0],args);
		}
		start = end+1;
		end = end+bytes;
	}
	long long n = numProcesses;
	while(n--)
	{
		int status;
		pid_t pid = wait(&status);
	}
	char fileName[1000];
	freopen(outFile,"w",stdout);
	for(i=0;i<numProcesses;i++)
	{
		char fileNum[100000];
    	tostring(fileNum,i+1);
    	strcpy(fileName,"part");
    	strcat(fileName,fileNum);
  	 	pid_t pid = fork();
    	int status;
    	char *args[] = {"cat",fileName,NULL};
    	if(pid == 0)
    	{
    		execvp(args[0],args);
    	}
    	else if(pid > 0)
    	{
    		pid = wait(&status);
    	}
    	else
    	{
    		fprintf(stderr,"Can't fork a new process\n");
    		abort();
    	}
	}
	return 0;
}