#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define BUF_SIZE 100

void error_handling(char *message);
void* send_msg(void* arg);
void* recv_msg(void* arg);
void* delieverybox(char* msg);

char msg[BUF_SIZE];

int main(int argc, char *argv[])
{
	int sock;
	struct sockaddr_in ser_addr;
	pthread_t s_t, r_t;
	void* thread_return;
	if(argc!=3)
	{
		printf("Usage : %s <IP> <port> \n", argv[0]);
		exit(1);
	}
	
	//socket create
	sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if(sock == -1)
		error_handling("socket() error");

	memset(&ser_addr, 0, sizeof(ser_addr));
	
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = inet_addr(argv[1]);
	ser_addr.sin_port = htons(atoi(argv[2]));

	while(1)
	{	//demand connecting
	if(connect(sock, (struct sockaddr *)&ser_addr, sizeof(ser_addr))==-1)
		error_handling("connect() error");
	else
		puts("Connected.............");
	
	while(1)
	{	
		pthread_create(&s_t,NULL, send_msg, (void*)&sock);
		pthread_create(&r_t,NULL, recv_msg, (void*)&sock);
		pthread_join(s_t, &thread_return);
		pthread_join(r_t, &thread_return);
	}

	close(sock);
	}
	return 0;
}

void* send_msg(void* arg)
{
	int sock = *((int*)arg);
	while(1)
	{
		fgets(msg, BUF_SIZE,stdin);
		if(!strcmp(msg, "q\n")||!strcmp(msg,"Q\n"))
		{
			close(sock);
			exit(0);
		}
		write(sock, msg,strlen(msg));
	}
	return NULL;
}


void* recv_msg(void* arg)
{
	int sock = *((int*)arg);
	int read_len;
	while(1)
	{
		read_len = read(sock,msg, BUF_SIZE-1);
		if(read_len == -1)
			return (void*)-1;

		msg[read_len] = 0;
		//delieverybox(msg);
	}
	return NULL;
}

void* delieverybox(char* msg)
{
	switch(msg[0])
	{
		case 65 :
			printf("door_on\n"); break;
		case 66 :
			printf("door_off\n"); break;
		case 67 :
			printf("A_ON\n");break;
		case 68 :
			printf("A_OFF\n");break;
		default :
			break;

	}
	return NULL;
}

void error_handling(char* message)
{
	fputs(message,stderr);
	fputc('\n', stderr);
	exit(1);
}
