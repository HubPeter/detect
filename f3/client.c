#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<error.h>
#include<arpa/inet.h>

#define PORT 9999
#define BUFSIZE 1024
#define KEYLEN 5
void PANIC(char* msg);
#define PANIC(msg) {perror(msg);exit(1);}

int main(int argc,char* argv[])
{
	struct sockaddr_in addr;
	int sockfd;
	int read_bytes;
	char * server="127.0.0.1";
	
	sockfd=socket(PF_INET,SOCK_STREAM,0);
	if(sockfd<0)
		PANIC("socket()");
	
	bzero(&addr,sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_port=htons(PORT);
	addr.sin_addr.s_addr=inet_addr(server);
	//connect to server
	if(connect(sockfd,(struct sockaddr*)&addr,sizeof(addr))==-1)
		PANIC("connect()");

	char buffer[BUFSIZE];
	bzero(buffer,BUFSIZE);
	

	//get the key from user
	char key[KEYLEN];
	printf("type your key:");
	scanf("%s",key);
	printf("key:%s\n",key);
	//give it to the destination
	memcpy(buffer,key,strlen(key));
	read_bytes=send(sockfd,key,strlen(key),0);
	if(read_bytes==0)
	{	
		printf("key send failed");
		return 0;
	}
	else
		printf("Key sended!\n");	
	read_bytes=recv(sockfd,buffer,BUFSIZE,0);
	if(read_bytes>0)
	{
		printf("%s\n",buffer);
	}
	//up is Ok
	/* file transfer */
	//get file name
	char *filename="data";//filename
	//open file
	int filefd=open(filename, O_RDONLY);
	if(filefd<=0)
		printf("Can't open %s\n",filename);
	//trans data
	printf("File transfering......");
	int i=0;//to count the dot
	int line=0;
	//send the filename
	read_bytes=send(sockfd,filename,strlen(filename)+1,0);
	if(read_bytes<=0)
		printf("Filename send failed!\n");
	bzero(buffer,BUFSIZE);
	while((read(filefd,buffer,BUFSIZE))!=0)
	{
		//printf("%s",databuf);
		read_bytes=send(sockfd,buffer,BUFSIZE,0);
		if(read_bytes<=0)//send fail
		{
			//remember the position
			printf("Stoped on line:%d\n",line);
		}
		
		printf("..");
		i++;
		line++;
		if(i%20==0)
			printf("\n");
		bzero(buffer,BUFSIZE);
	}
	printf("\n");
	//tell the reciver the end of the data
	bzero(buffer,BUFSIZE);
	memcpy(buffer,"EOF",4);
	read_bytes=send(sockfd,buffer,BUFSIZE,0);
	if(read_bytes<=0)
	{
		printf("EOF failed!\n");
		return 0;
	}
	printf("%s transfered!\n",filename);

	close(sockfd);
}
