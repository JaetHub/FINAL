#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUF_SIZE 1000
#define MAX_CLT 256

void* ThreadMain(void* args);
void* HandleTCPClient(int clt_sock);
void error_handling(char* message);
void Recv_IMG(int sock);
void update_db(char* msg,int clt_sock);

int clt_cnt =0;
int clt_socks[MAX_CLT];
pthread_mutex_t mutx;

int main(int argc, char *argv[])
{
	struct sockaddr_in ser_addr,clt_addr;
	int clt_addr_len;
	pthread_t t_id,f_id;
	
	if(argc!=2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	
	//socket create
	
	pthread_mutex_init(&mutx, NULL);
	int ser_sock = socket(PF_INET, SOCK_STREAM, 0);
	
	if(ser_sock == -1)
		error_handling("socket() error");

	memset(&ser_addr, 0, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	ser_addr.sin_port = htons(atoi(argv[1]));
	
	//IP, PORT bind
	if(bind(ser_sock, (struct sockaddr*) &ser_addr, sizeof(ser_addr))==-1)
		error_handling("bind() error");
	
	//listen
	if(listen(ser_sock,5) ==-1)
		error_handling("listen() error");
	
	//accept
	printf("\nServer Waiting.........\n\n");
	while(1)
	{
		int clt_addr_len = sizeof(clt_addr);
		int clt_sock= accept(ser_sock, (struct sockaddr*)&clt_addr, &clt_addr_len);
		
		if(clt_sock == -1)
			error_handling("accept() error");
		else
		{
			printf("--------------\n");
			puts("Connect!");
		}

		pthread_mutex_lock(&mutx);
		clt_socks[clt_cnt++] = clt_sock;
		pthread_mutex_unlock(&mutx);
	        
		pthread_create(&t_id, NULL, ThreadMain, (void*)&clt_sock);
		pthread_detach(t_id);
	
		printf("How Many Client?  %d\n",clt_cnt);
		printf("--------------\n");
	}
	close(ser_sock);
	return 0;
}

void* ThreadMain(void* args)
{
	int clt_sock = *((int*)args);
	int i;
	
	HandleTCPClient(clt_sock);

	pthread_mutex_lock(&mutx);
	for(i=0; i<clt_cnt;i++)
	{
		if(clt_sock == clt_socks[i])
		{
			while(i++<clt_cnt-1)
				clt_socks[i] = clt_socks[i+1];
			break;
		}
	}
	clt_cnt--;
	pthread_mutex_unlock(&mutx);

	printf("--------------\n");
	puts("Disonnect!");
	printf("How Many Client?  %d\n",clt_cnt);
	printf("--------------\n\n");

	close(clt_sock);
	return NULL;
}

void* HandleTCPClient(int clt_sock)
{
	int i;
	int str_len=0;
	char msg[BUF_SIZE];

	while((str_len = read(clt_sock,msg,sizeof(msg)))!=0)
	{
		pthread_mutex_lock(&mutx);
		for(i=0; i<clt_cnt; i++)
			write(clt_socks[i],msg,str_len);
		update_db(msg,clt_sock);
		pthread_mutex_unlock(&mutx);
	}
}

void update_db(char* msg,int clt_sock)
{
	char* data = msg;
	int sock =clt_sock;
	switch(data[0])
	{
		case 69 :
			printf("\n-------------------------------------\n");
			printf("Send Message to Android : CCTV ON              DB Update : status\n");
			printf("-------------------------------------\n\n\n");
			break;
		case 70 : 
			printf("\n-------------------------------------\n");
			printf("Send Message to Android : CCTV OFF              DB Update : status\n");
			printf("-------------------------------------\n\n\n");
			break;
		case 71 : 
			printf("\n-------------------------------------\n");
			printf("Recieved a Picture                              DB Update : picture\n");
			Recv_IMG(sock);
			printf("-------------------------------------\n\n\n");
			break;
		case 72 : 
			printf("\n-------------------------------------\n");
			printf("Send Message to Android : MESSAGE PLAY            DB Update : status\n");
			printf("-------------------------------------\n\n\n");
			break;
		default :
		       	break;
	}
}	

void Recv_IMG(int socket)
{
	char filename[20];
	int filesize =0;
	int total=0,recv_len;
	int fp;
	char buf[BUF_SIZE];
	int sock = socket;
	int temp;

	bzero(filename,20);
	recv(sock, filename,sizeof(filename),0);
	printf("filename : %s  ",filename);
	read(sock, &filesize,sizeof(filesize));
	printf("filesize : %d  \n",filesize);
	
	fp = open(filename,O_RDWR|O_CREAT|O_TRUNC);
	while(1)
	{
		memset(buf,0x00,BUF_SIZE);
		recv_len= recv(sock, buf, BUF_SIZE,0);
		write(fp,buf,recv_len);
		temp =total;
		total+=recv_len;
		if(total >=filesize)
		{
			printf("finish file\n");
			break;
		}
	}

	printf("file translating is completed\n");
	close(fp);
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

