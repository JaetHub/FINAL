CC = gcc

app : app.o
	$(CC) -o app app.o -lpthread

app.o : app.c

clean :
	rm -rf *.o app
