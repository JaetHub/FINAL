CC = gcc

server1: server1.o
	$(CC) -o iot_server server1.o -lpthread
server: server.o
	$(CC) -o iot_server server.o -lpthread
	
client1 : client1.o
	$(CC) -o doorcam client1.o -lpthread

client2 : client2.o
	$(CC) -o delibox client2.o -lpthread

client0 : client0.o
	$(CC) -o delibox client0.o -lpthread

test :
	$(CC) test.c -o test -I/usr/include/mysql -L/usr/lib -lmysqlclient

server.o : server.c

server1.o : server1.c

client0.o : client0.c

client1.o : client1.c

client2.o : client2.c


clean :
	rm -rf *.o iot_server doorcam delibox
