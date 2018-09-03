#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <dirent.h>


char* formatdate(char* str, time_t val);
int list_contents(char *location,int show_hidden,int show_lengthy);