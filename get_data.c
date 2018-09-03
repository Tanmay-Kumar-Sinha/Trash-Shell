#include "get_data.h"

unsigned long int power(unsigned long int x,unsigned long int y)
{
	if(y==0) return 1;
	else
	{
		if(y%2==0)
		{
			unsigned long int b=power(x,y/2);
			return b*b;
		}
		else
		{
			return x*power(x,y-1);
		}
	}
}

char to_char(int a)
{
	if(a==0) return '0';
	else if(a==1) return '1';
	else if(a==2) return '2';
	else if(a==3) return '3';
	else if(a==4) return '4';
	else if(a==5) return '5';
	else if(a==6) return '6';
	else if(a==7) return '7';
	else if(a==8) return '8';
	else if(a==9) return '9';
}

unsigned long int char_to_int(char *a)
{
	//Takes a character array and turns it into an integer.
	int i=0;
	while(a[i]!='\0') i++;
	i--;
	int length=i;
	unsigned long int answer;
	while(i>=0)
	{
		unsigned long int digit=a[i];
		answer+=(unsigned long int)power(10,length-i);
		i--;
	}
}

int get_username(char username[])
{
	//Writes the username into the array username and returns the length of the username.
	char *name;
	//gethostname(name,100);
	name=getenv("USER");
	int len_username=0;
	while(name[len_username]!='\0')
	{
		username[len_username]=name[len_username];
		len_username++;
	}
	return len_username;
}


int get_osname(char os_name[])
{
	// Writes the osname into the array osname and returns the length of the os name.
	struct utsname system_info;
	uname(&system_info);
	int len_osname=0;
	while(system_info.sysname[len_osname]!='\0')
	{
		os_name[len_osname]=system_info.sysname[len_osname];
		len_osname++;
	}
	return len_osname;
}

int get_curr_dir(char current_dir[])
{
	//Writes the home directory path into home array and returns length of the path.
	for(int i=0;i<strlen(current_dir);i++)
	{
		current_dir[i]='\0';
	}
	int len_home=0;
	char name[1000];
	getcwd(name,1000);
	while(name[len_home]!='\0')
	{
		current_dir[len_home]=name[len_home];
		len_home++;
	}
	if(current_dir[len_home]=='/') current_dir[len_home]='\0'; 
	current_dir[len_home+1]='\0';
	return len_home;
}
