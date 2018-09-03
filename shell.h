#include<unistd.h>
#include<sys/utsname.h>
#include<sys/types.h>
#include <dirent.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "get_data.h"
#include "list_contents.h"
#include <sys/wait.h>
#include<fcntl.h>
#include "pinfo.h"
#include<time.h>
#include "clock.h"

//Structs used

typedef struct 
{
	long int pid;
	char command[100];
} bg_process;

typedef struct 
{
	char message[100];
	int toprint;
	unsigned long int time;//Gives time after which command is to be printed.
} reminder;

//Global variables accessible to all.

char username[100];
int len_username;
char os_name[50];
int len_osname;
char home[1000];
int len_home;
char prompt[1000];
int in_home=1;
char curr_dir[1000];
char *command;
int exit_flag=0;
int num_bg_processes=0;
bg_process bg_processes[1000];
reminder Reminders[100];
int num_reminders=0;

