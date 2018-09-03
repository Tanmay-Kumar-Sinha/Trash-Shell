#include<sys/utsname.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
unsigned long int power(unsigned long int x,unsigned long int y);
char to_char(int a);
unsigned long int char_to_int(char *a);
int get_username(char username[]);
int get_osname(char os_name[]);
int get_curr_dir(char current_dir[]);