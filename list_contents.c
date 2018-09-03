#include "list_contents.h"

char* formatdate(char* str, time_t val)
{
        strftime(str, 36, "%d.%m.%Y %H:%M:%S", localtime(&val));
        return str;
}

int list_contents(char *location,int show_hidden,int show_lengthy)
{
	//printf("%s\n", location);
	DIR *d;
	struct dirent *dir;
	d=opendir(location);
	if(d) dir=readdir(d);
	if(show_hidden==0)
	{
		while(dir!=NULL)
		{
			if(show_lengthy==0)
			{
				if(dir->d_name[0]!='.') printf("%s ",dir->d_name);
			}
			else
			{
				if(dir->d_name[0]!='.')
				{
					struct stat info;
					char copy_location[100];
					strcpy(copy_location,location);
					copy_location[strlen(location)]='/';
					copy_location[strlen(location)+1]='\0';
					stat(strcat(copy_location,dir->d_name),&info);
					if(S_ISDIR(info.st_mode)) printf("d");
					else printf("-");
					
					if(info.st_mode & S_IRUSR) printf("r");
					else printf("-");
					if(info.st_mode & S_IWUSR) printf("w");
					else printf("-");
					if(info.st_mode & S_IXUSR) printf("x");
					else printf("-");
				
					if(info.st_mode & S_IRGRP) printf("r");
					else printf("-");
					if(info.st_mode & S_IWGRP) printf("w");
					else printf("-");
					if(info.st_mode & S_IXGRP) printf("x");
					else printf("-");
				
				
					if(info.st_mode & S_IROTH) printf("r");
					else printf("-");
					if(info.st_mode & S_IWOTH) printf("w");
					else printf("-");
					if(info.st_mode & S_IXOTH) printf("x ");
					else printf("- ");
					
					printf("%ld ",info.st_nlink);

					struct passwd *pw = getpwuid(info.st_uid);
					struct group  *gr = getgrgid(info.st_gid);
					char date[36];
					printf("%s %s %ld %s %s\n", pw->pw_name,gr->gr_name,info.st_size,formatdate(date,info.st_mtime),dir->d_name);
				}
			}
			dir=readdir(d);
		}
	}
	else
	{
		while(dir!=NULL)
		{
			if(show_lengthy==0)
			{
				printf("%s ",dir->d_name);
			}
			else
			{
				struct stat info;
				char copy_location[100];
				strcpy(copy_location,location);
				copy_location[strlen(location)]='/';
				copy_location[strlen(location)+1]='\0';
				stat(strcat(copy_location,dir->d_name),&info);
				if(S_ISDIR(info.st_mode)) printf("d");
				else printf("-");
				
				if(info.st_mode & S_IRUSR) printf("r");
				else printf("-");
				if(info.st_mode & S_IWUSR) printf("w");
				else printf("-");
				if(info.st_mode & S_IXUSR) printf("x");
				else printf("-");
			
				if(info.st_mode & S_IRGRP) printf("r");
				else printf("-");
				if(info.st_mode & S_IWGRP) printf("w");
				else printf("-");
				if(info.st_mode & S_IXGRP) printf("x");
				else printf("-");
			
				if(info.st_mode & S_IROTH) printf("r");
				else printf("-");
				if(info.st_mode & S_IWOTH) printf("w");
				else printf("-");
				if(info.st_mode & S_IXOTH) printf("x ");
				else printf("- ");
				
				printf("%ld ",info.st_nlink);
				struct passwd *pw = getpwuid(info.st_uid);
				struct group  *gr = getgrgid(info.st_gid);
				char date[36];
				printf("%s %s %ld %s %s\n", pw->pw_name,gr->gr_name,info.st_size,formatdate(date,info.st_mtime),dir->d_name);
			}
			dir=readdir(d);			
		}
	}
	closedir(d);
	printf("\n");
}
