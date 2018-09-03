#include "pinfo.h"

void print_info(char *arg_)
{
	int i=0;
	char arg[1000];
	while(arg_[i]!='\0')
	{
		arg[i]=arg_[i];
		i++;
	} 
	arg[i]='\0';
	printf("pid -- %s\n", arg);
	char destination[1000];
	destination[0]='/';
	destination[1]='p';
	destination[2]='r';
	destination[3]='o';
	destination[4]='c';
	destination[5]='/';
	destination[6]='\0';
	char destination2[100];
	char destination3[100];
	strcpy(destination2,destination);
	strcpy(destination3,destination);
	strcat(destination,arg);
	strcat(destination2,arg);				
	strcat(destination3,arg);				
	int fd=open(strcat(destination,"/status"), O_RDONLY);
	char proc_info[1000];
	if(read(fd,proc_info,1000)==-1)
	{
		printf("An error occured\n");
		return;
	}
	for(i=0;i<strlen(proc_info)-1;i++)
	{
		if(i!=0 && proc_info[i]=='S' && proc_info[i-1]=='\n' && proc_info[i+1]=='t')
		{
			while(proc_info[i]!='\n')
			{
				printf("%c", proc_info[i]);
				i++;
			}
			printf("\n");
			break;
		}
	}
	int fd2=open(strcat(destination2,"/statm"),O_RDONLY);
	char proc_info2[1000];
	read(fd2,proc_info2,1000);
	i=0;
	printf("Virtual memory -- ");
	while(proc_info2[i]!=' ')
	{
		printf("%c", proc_info2[i]);
		i++;
	}
	printf("\n");
	char executable_path[100];
	strcat(destination3,"/exe");
	for(int j=0;j<100;j++) executable_path[j]='\0';
	readlink(destination3, executable_path,100);
	executable_path[strlen(executable_path)+1]='\0';
	printf("Executable path -- %s\n",executable_path);
	close(fd);
	close(fd2);
	return;
}