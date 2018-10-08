CC=gcc



main:
	${CC} -c clock.c list_contents.c pinfo.c get_data.c signal_handlers.c 
	${CC} shell.c clock.o list_contents.o pinfo.o get_data.o signal_handlers.o -o trash

clean:
	rm *.o
	rm trash
