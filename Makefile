CC=gcc



main:
	${CC} -c clock.c list_contents.c pinfo.c get_data.c
	${CC} shell.c clock.o list_contents.o pinfo.o get_data.o -o trash

clean:
	rm *.o
	rm trash
