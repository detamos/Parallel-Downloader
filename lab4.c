#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>

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

struct args
{
	char range[1000],part[1000],url[1000];
};

void *run(void *arg)
{
	struct args *arr = (struct args *)arg;
	char comm[1000];
	strcpy(comm,"curl --range ");
	strcat(comm,arr->range);
	strcat(comm," -o ");
	strcat(comm,arr->part);
	strcat(comm," ");
	strcat(comm,arr->url);
	system(comm);
	pthread_exit(0);
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
	long long start = 0,end = bytes;
	int i;
	pthread_t threads[numProcesses];
	struct args arr[numProcesses];
	for(i=0;i<numProcesses;i++)
	{
		if(i == numProcesses-1)
			end = size;
		char numtochar[1000];
		tostring(numtochar,i+1);
		strcpy(arr[i].part,"part");
		strcat(arr[i].part,numtochar);
		char srange[1000],erange[1000];
		tostring(srange,start);
		tostring(erange,end);
		strcpy(arr[i].range,srange);
		strcat(arr[i].range,"-");
		strcat(arr[i].range,erange);
		strcpy(arr[i].url,url);
		pthread_create(&threads[i],NULL,run,(void*)(arr+i));
		start = end+1;
		end = end+bytes;
	}
	for(i=0;i<numProcesses;i++)
	{
		pthread_join(threads[i],NULL);
	}
	char fileName[1000];
	for(i=0;i<numProcesses;i++)
	{
		char fileNum[100000];
    	tostring(fileNum,i+1);
    	strcpy(fileName,"part");
    	strcat(fileName,fileNum);
    	char comm[1000];
    	strcpy(comm,"cat ");
    	strcat(comm,fileName);
    	strcat(comm," >> ");
    	strcat(comm,outFile);
  	 	pid_t pid = fork();
    	int status;
    	if(pid == 0)
    	{
    		system(comm);
    		exit(0);
    	}
    	else if(pid > 0)
    	{
    		pid = wait(&status);
    	}
    	else
    	{
    		printf("Can't fork new child\n");
    		exit(0);
    	}
	}
	return 0;
}