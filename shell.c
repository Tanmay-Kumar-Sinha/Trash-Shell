#include "shell.h"

void handle_sigstop(int signo)
{
	bg_processes[num_bg_processes].finished=0;
	bg_processes[num_bg_processes].pid=curr_child_pid;
	strcpy(bg_processes[num_bg_processes].command,running_command);
	strcpy(running_command,"\0");
	num_bg_processes++;
  if(curr_child_pid!=0) kill(curr_child_pid,SIGSTOP);
	curr_child_pid=0;
}

void check_reminders()
{
	if(num_reminders==0) return;
	int i;
	unsigned long int curr_time;
	time(&curr_time);
	for(i=0;i<num_reminders;i++)
	{
		if(curr_time>Reminders[i].time && Reminders[i].toprint==1)
		{
			printf("%s\n", Reminders[i].message);
			Reminders[i].toprint=0;
		}
	}
}

void list_jobs(bg_process processes[],int num)
{
  int num_printed=1;
  for(int i=0;i<num;i++)
  {
    if(processes[i].finished==0)
    {
      printf("[%d]", num_printed);
      num_printed++;
      char pid_string[100];
      int pid_int=processes[i].pid;
      int j=0;
      while(pid_int>0)
			{
				pid_string[j]=(to_char(pid_int%10));
				j++;
				pid_int=pid_int/10;
			}
			char pid_string2[100];
			for(int k=j-1;k>=0;k--) pid_string2[j-k-1]=pid_string[k];
			pid_string2[j]='\0';
			char dest[100];
			dest[0]='/';
			dest[1]='p';
			dest[2]='r';
			dest[3]='o';
			dest[4]='c';
			dest[5]='/';
			dest[6]='\0';
			strcat(dest,pid_string2);
			int fd=open(strcat(dest,"/status"),O_RDONLY);
			char proc_info[1000];
			read(fd,proc_info,1000);
			for(int k=0;k<strlen(proc_info)-1;k++)
			{
				if(k!=0 && proc_info[k]=='S' && proc_info[k-1]=='\n' && proc_info[k+1]=='t')
				{
					k+=6;
					while(proc_info[k]!='\n')
					{
						printf("%c", proc_info[k]);
						k++;
					}
    		}
  		}
  		printf("\t%s [%ld]\n",processes[i].command,processes[i].pid);
		}
	}
}


void generate_prompt()
{
	int i;
	for(i=0;i<strlen(prompt);i++) prompt[i]='\0';
	for(i=0;i<len_username;i++)
	{
		prompt[i]=username[i];
		printf("%c", username[i]);
	}
	prompt[i]='@';
	printf("@");
	i++;
	for(int j=0;j<len_osname;j++)
	{
		prompt[i]=os_name[j];
		printf("%c", os_name[j]);
		i++;
	}
	prompt[i]=':';
	printf("%c", prompt[i]);
	i++;
	char current[1000];
	getcwd(current,1000);
	int j=0;
	while(home[j]!='\0' && current[j]==home[j]) j++;
	if(home[j]=='\0')
	{
		printf("~");
		while(current[j]!='\0')
		{
			printf("%c", current[j]);
			j++;
		}
	}
	else
	{
		printf("%s", current);
	}
	printf("> ");
	return;
}

void getinput()
{
	long int read,len;
	read=getline(&command,&len,stdin);
}

void check_bg_processes()
{
	int status;
	pid_t wpid;
	wpid=waitpid(-1,&status,WNOHANG);
	while(wpid>0)
	{
		for(int i=0;i<num_bg_processes;i++)
		{
			if(bg_processes[i].pid==wpid)
			{
				printf("%s with pid %d exited normally\n",bg_processes[i].command,wpid);
				bg_processes[i].finished=1;
			}
		}
		wpid=waitpid(-1,&status,WNOHANG);
	}
}

void execute(char *full_command)
{
	char copy_command[1000],copy_2_command[1000];
	strcpy(copy_command,full_command);
	strcpy(copy_2_command,full_command);
	int is_Background=0;
	int input_redirect=0;
	int output_redirect=0;//Should be 1 for overwrite and 2 for append.
	char input_file[1000];
	char output_file[1000];
	char relevant_parts[1000];
	relevant_parts[0]='\0';
	int saved_stdout = dup(1);
	int saved_stdin = dup(0);

	char *text=strtok(copy_2_command,"\n\t ");

	while(text!=NULL)
	{
		if(strcmp(text,"<")==0)
		{
			input_redirect=1;
			int i;
			text=strtok(NULL,"\n\t ");
			for(i=0;text[i]!='\0';i++)
			{
				input_file[i]=text[i];
			}
			input_file[i]=text[i];
		}
		else if(strcmp(text,">")==0)
		{
			int i;
			output_redirect=1;
			text=strtok(NULL,"\n\t ");
			for(i=0;text[i]!='\0';i++)
			{
				output_file[i]=text[i];
			}
			output_file[i]=text[i];
		}
		else if(strcmp(text,">>")==0)
		{
			int i;
			output_redirect=2;
			text=strtok(NULL,"\n\t ");
			for(i=0;text[i]!='\0';i++)
			{
				output_file[i]=text[i];
			}
			output_file[i]=text[i];
		}
		else if(strcmp(text,"&")==0)
		{
			is_Background=1;
		}
		if(text!=NULL) text=strtok(NULL,"\n\t ");
	}

	if(output_redirect==1)
	{
		int fd1=open(output_file,O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		dup2(fd1,1);
		close(fd1);
	}
	else if(output_redirect==2)
	{
		int fd1=open(output_file,O_RDWR | O_APPEND | O_CREAT,S_IRUSR | S_IWUSR);
		dup2(fd1,1);
		close(fd1);
	}

	if(input_redirect==1)
	{
		int fd3=open(input_file,O_RDONLY,S_IRUSR | S_IWUSR);
		dup2(fd3,0);
		close(fd3);
	}

	int k=0;
	while(full_command[k]!='<' && full_command[k]!='>' && full_command[k]!='\0') k++;
	full_command[k]='\0';

	//char *command=strtok(full_command,"\n\t ");
	char *command=strtok(full_command,"\n\t ");
	if(strcmp(command,"exit")==0 || strcmp(command,"quit")==0)
	{
		exit_flag=1;
	}

	else if(strcmp(command,"setenv")==0)
	{
		char *variable_name=strtok(NULL,"\n\t ");
		char *variable_value=strtok(NULL,"\n\t ");
		if(variable_value!=NULL)
		{
			char *arg=strtok(NULL,"\n\t ");
			if(arg!=NULL)
			{
				perror("Error: setenv only takes two arguments.");
			}
			else
			{
				setenv(variable_name,variable_value,1);
			}
		}
		else
		{
			setenv(variable_name,"",1);
		}
	}

	else if(strcmp(command,"unsetenv")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		if(arg==NULL)
		{
			perror("Error: unsetenv requires atleast one argument.");
		}
		else
		{
			while(arg!=NULL)
			{
				unsetenv(arg);
				arg=strtok(NULL,"\n\t ");
			}
		}
	}

	else if(strcmp(command,"pwd")==0)
	{
		char *name;
		printf("%s\n", getcwd(curr_dir,100));
	}

	else if(strcmp(command,"jobs")==0)
	{
		list_jobs(bg_processes,num_bg_processes);
	}

	else if(strcmp(command,"kjob")==0)
	{
		char *jobid=strtok(NULL,"\n\t ");
		if(jobid==NULL)
		{
			perror("Usage: kjob <jobNumber> <signal>");
		}
		else
		{
			char *signal=strtok(NULL,"\n\t ");
			if(signal==NULL)
			{
				perror("Usage: kjob <jobNumber> <signal>");
			}
			else
			{
				int job_id=to_int(jobid);
				int sig=to_int(signal);
				int i=0;
				int j=0;
				while(job_id!=i && j<num_bg_processes)
				{
					if(bg_processes[j].finished!=1)
					{
						i++;
					}
					j++;
				}
				kill(bg_processes[j-1].pid,sig);
			}
		}
	}

	else if(strcmp(command,"overkill")==0)
	{
		for(int i=0;i<num_bg_processes;i++)
		{
			if(bg_processes[i].finished!=1)
			{
				kill(bg_processes[i].pid,9);
			}
		}
	}

	else if(strcmp(command,"fg")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		if(arg==NULL)
		{
			perror("Usage: fg <jobNumber>");
		}
		else
		{
			int job_id=to_int(arg);
			int i=0;
			int j=0;
			while(i!=job_id && j<num_bg_processes)
			{
				if(bg_processes[j].finished!=1)
				{
					i++;
				}
				j++;
			}
			j--;
			if(i!=job_id)
			{
				perror("Error");
			}
			else
			{
				pid_t pid=bg_processes[j].pid;
				int status;
				kill(pid,18);
				curr_child_pid=pid;
				waitpid(pid,&status,WUNTRACED);
				bg_processes[j].finished=1;
			}
		}
	}

	else if(strcmp(command,"bg")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		if(arg==NULL)
		{
			perror("Usage: bg <jobNumber>");
		}
		else
		{
			char new_command[100];
			strcpy(new_command,"kjob ");
			strcat(new_command,arg);
			strcat(new_command," 18");
			char *this_command=(char*)malloc(100*sizeof(char));
			strcpy(this_command,new_command);
			execute(this_command);
		}
	}

	else if(strcmp(command,"cd")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		if(arg==NULL) return;
		int correct;
		if(strcmp(arg,"~")==0)
		{
			correct=chdir(home);
		}
		else
		{
			correct=chdir(arg);
		}
		
		if(correct!=0)
		{	
			printf("%s: no such directory.\n", arg);
		}
		if(strcmp(home,curr_dir)==0 && strcmp("..",arg)==0)
		{
			in_home = 0;
		}
		getcwd(curr_dir,1000);
		if(strcmp(home,curr_dir)==0)
		{
			in_home=1;
		}
	}

	else if(strcmp(command,"echo")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		while(arg!=NULL)
		{
			printf("%s ", arg);
			arg=strtok(NULL,"\n\t ");
		}	
		printf("\n");
	}

	else if(strcmp(command,"ls")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		char location[1000];
		int show_hidden=0;
		int show_lengthy=0;
		while((arg!=NULL && arg[0]=='-'))
		{
			for(int i=1;i<strlen(arg);i++)
			{
				if(arg[i]=='l') show_lengthy=1;
				if(arg[i]=='a') show_hidden=1;
			}
			arg=strtok(NULL,"\n\t ");
		}
		if(arg==NULL)
		{
			strcpy(location,curr_dir);
		}
		else strcpy(location,arg);
		list_contents(location,show_hidden,show_lengthy);
		arg=strtok(NULL,"\n\t ");
		while(arg!=NULL)
		{
			strcpy(location,arg);
			list_contents(location,show_hidden,show_lengthy);
			arg=strtok(NULL,"\n\t ");
		}
	}

	else if(strcmp(command,"pinfo")==0)
	{

		char *arg=strtok(NULL,"\n\t ");
		if(arg==NULL)
		{
			pid_t pid = getpid();
			int i=0;
			char* arg_=(char*)(malloc(1000));
			while(pid>0)
			{
				arg_[i]=(to_char(pid%10));
				i++;
				pid=pid/10;
			}
			arg_[i]='\0';
			arg=(char*)malloc(1000);
			for(int j=i-1;j>=0;j--) arg[i-j-1]=arg_[j];
			arg[i]='\0';
		}
		print_info(arg);
	}

	else if(strcmp(command,"clock")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		char *time_interval;
		unsigned long int current_time;
		time(&current_time);
		if((arg!=NULL && arg[0]=='-' && arg[1]=='t'))
		{
			time_interval=strtok(NULL,"\n\t ");
		}
		else
		{
			perror("Error in format: please specify time interval using the -t flag\n");
			return;
		}
		unsigned long int final_time;
		unsigned long int dt;
		dt=char_to_int(time_interval);
		arg=strtok(NULL,"\n\t ");
		if(arg==NULL)
		{
			final_time=current_time+10*dt;
		}
		else
		{
			arg=strtok(NULL,"\n\t ");
			if(arg==NULL)
			{
				printf("Incorrect usage.\n");
				return;
			}
			final_time=char_to_int(arg);
			final_time+=current_time;
		}
		while(final_time>current_time)
		{
			print_time();
			sleep(dt);
			time(&current_time);
		}
	}

	else if(strcmp(command,"remindme")==0)
	{
		char *arg=strtok(NULL,"\n\t ");
		if(arg==NULL)
		{
			perror("Error. See usage of command\n");
			return;
		}
		unsigned long int dt=char_to_int(arg);
		arg=strtok(NULL,"\n\t ");
		if(arg==NULL)
		{
			perror("No message given\n");
			return;
		}
		char message[1000];
		int i=0;
		int j=0;
		int num_spaces=0;
		while(num_spaces<2)
		{
			if(copy_command[i]==' ') num_spaces++;
			i++;
		}
		while(copy_command[i]!='\0')
		{
			message[j]=copy_command[i];
			i++;
			j++;
		}
		message[j]='\0';
		strcpy(Reminders[num_reminders].message,message);
		unsigned long int time_;
		time(&time_);
		Reminders[num_reminders].time=time_+dt;
		Reminders[num_reminders].toprint=1;
		num_reminders++;
	}

	else
	{
		strcpy(running_command,command);
		curr_child_pid=fork();
		int status;
		if(curr_child_pid==0)
		{
			char *arg=strtok(NULL,"\n\t ");
			char *argv[100];
			argv[0]=command;
			int i=1;
			while(arg!=NULL)
			{
				arg[strlen(arg)]='\0';
				if(strcmp(arg,"&")!=0 && strcmp(arg,">")!=0) argv[i]=arg;
				i++;
				arg=strtok(NULL,"\n\t ");
			}
			argv[i]=arg;
			int possible=execvp(command,argv);
			if(possible==-1)
			{
				perror("Invalid option.");
				exit(0);
			}
		}
		else
		{
			if(is_Background!=1) 
			{
        int status;
				waitpid(curr_child_pid,&status,WUNTRACED);
				curr_child_pid=0;
			}
			else
			{
				bg_processes[num_bg_processes].pid=curr_child_pid;
				//strcpy(bg_processes[num_bg_processes].command,copy_command);
				int j=0;
				while(copy_command[j]!='\t' && copy_command[j]!=' ' && copy_command[j]!='\0')
				{
					bg_processes[num_bg_processes].command[j]=copy_command[j];
					j++;
				}
				bg_processes[num_bg_processes].command[j]='\0';
				bg_processes[num_bg_processes].finished=0;
				num_bg_processes++;
			}
		}
	}
	dup2(saved_stdout, 1);
	dup2(saved_stdin,0);
	close(saved_stdout);
	close(saved_stdin);
}

void multiple_pipes(char commands[1000][1000],int num_commands)
{
	int p[2];
	int fd_in=0;
	int i=0;
	while(i!=num_commands)
	{
		pipe(p);
		if(fork()==0)
		{
			dup2(fd_in,0);
			if(i+1!=num_commands)
			{
				dup2(p[1],1);
			}
			close(p[0]);
			execute(commands[i]);
			exit(0);
		}
		else
		{
			wait(NULL);
			close(p[1]);
			fd_in=p[0];
			i++;
		}
	}
}

void pre_process(char *full_command)
{
	char copy[1000];
  strcpy(copy,full_command);
  int num_commands=0;
  char commands[1000][1000];//Stores the series of commands to be used in pipe.
  char *command=strtok(full_command,"|");
  int saved_stdin=dup(0);
  int saved_stdout=dup(1);
  while(command!=NULL)
  {
  	strcpy(commands[num_commands],command);
  	command=strtok(NULL,"|");
  	num_commands++;
  }
  if(num_commands==1)
  {
  	execute(commands[0]);
  }
  else
  {
  	multiple_pipes(commands,num_commands);
  }
  dup2(saved_stdout,1);
  dup2(saved_stdin,0);
}

void parse_input(char *command)
{
	char copy[1000];
	strcpy(copy,command);
	char *token=strtok(command,";\n");
	int times=0;
	while(token!=NULL)
	{
		pre_process(token);
		times++;
		strcpy(command,copy);
		token=strtok(command,";\n");
		for(int i=0;i<times;i++)
		{
			token=strtok(NULL,";\n");
		}
	}
	check_bg_processes();
	check_reminders();
}

int main()
{
	running_command=(char*)malloc(100*sizeof(char));
	curr_child_pid=0;
	strcpy(running_command,"\0");
	signal(SIGINT,handle_sigint);
	signal(SIGTSTP,handle_sigstop);
	signal(SIGSTOP,handle_sigstop);
	//Getting the username.
	len_username = get_username(username);
	//Getting os name.
	len_osname = get_osname(os_name);
	// Obtaining the home directory.
	len_home=get_curr_dir(home);
	get_curr_dir(curr_dir);
	while(exit_flag==0)
	{
		generate_prompt();
		getinput();
		parse_input(command);
	}
	free(command);
}