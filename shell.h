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
#include "signal_handlers.h"
//Structs used

typedef struct 
{
  long int pid;
  char command[100];
  int finished;
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
pid_t curr_child_pid;
char *running_command;

//Functions defined.
void check_reminders();
void generate_prompt();
void getinput();
void check_bg_processes();
void execute(char *full_command);
void multiple_pipes(char commands[1000][1000],int num_commands);
void pre_process(char *full_command);
void parse_input(char *command);

