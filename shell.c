#include "shell.h"

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
	}
	else if(output_redirect==2)
	{
		int fd1=open(output_file,O_APPEND | O_CREAT,S_IRUSR | S_IWUSR);
		dup2(fd1,1);
	}

	if(input_redirect==1)
	{
		int fd3=open(input_file,O_RDONLY,S_IRUSR | S_IWUSR);
		dup2(fd3,0);
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

	else if(strcmp(command,"pwd")==0)
	{
		char *name;
		printf("%s\n", getcwd(curr_dir,100));
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
			printf("Error in format: please specify time interval using the -t flag\n");
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
		int pid=fork();
		int status;
		/*for(int i=0;i<strlen(copy_command);i++)
		{
			if(copy_command[i]=='&')
			{
				is_Background=1;
			}
		}
		*/if(pid==0)
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
				printf("%s is not a valid option.\n", command);
				exit(0);
			}
		}
		else
		{
			if(is_Background!=1) 
			{
				wait(NULL);
			}
			else
			{
				bg_processes[num_bg_processes].pid=pid;
				//strcpy(bg_processes[num_bg_processes].command,copy_command);
				int j=0;
				while(copy_command[j]!='\t' && copy_command[j]!=' ' && copy_command[j]!='\0')
				{
					bg_processes[num_bg_processes].command[j]=copy_command[j];
					j++;
				}
				bg_processes[num_bg_processes].command[j]='\0';
				num_bg_processes++;
			}
		}
	}
	dup2(saved_stdout, 1);
	dup2(saved_stdin,0);
	close(saved_stdout);
	close(saved_stdin);
}

void parse_input(char *command)
{
	char copy[1000];
	strcpy(copy,command);
	char *token=strtok(command,";\n");
	int times=0;
	while(token!=NULL)
	{
		execute(token);
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
//		printf("%s> ", prompt);
		getinput();
		parse_input(command);
	}
	free(command);
}