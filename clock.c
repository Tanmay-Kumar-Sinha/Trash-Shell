#include<time.h>
#include<stdio.h>
#include "clock.h"

void print_time()
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo=localtime(&rawtime);
	printf("%s\n", asctime(timeinfo));
}