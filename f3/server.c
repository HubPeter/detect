#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <errno.h>

#define MY_PORT		9999
#define MAXBUF		1024
#define KEYLEN 15

int main(int Count, char *Strings[])
{   int sockfd;
	struct sockaddr_in self;
	char buffer[MAXBUF];

	/*---Create streaming socket---*/
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
	{
		perror("Socket");
		exit(errno);
	}

	/*---Initialize address/port structure---*/
	bzero(&self, sizeof(self));
	self.sin_family = AF_INET;
	self.sin_port = htons(MY_PORT);
	self.sin_addr.s_addr = INADDR_ANY;

	/*---Assign a port number to the socket---*/
    if ( bind(sockfd, (struct sockaddr*)&self, sizeof(self)) != 0 )
	{
		perror("socket--bind");
		exit(errno);
	}

	/*---Make it a "listening socket"---*/
	if ( listen(sockfd, 20) != 0 )
	{
		perror("socket--listen");
		exit(errno);
	}

	/*---Forever... ---*/
	while (1)
	{	int clientfd;
		struct sockaddr_in client_addr;
		int addrlen=sizeof(client_addr);
		int read_bytes=0;
		/*---accept a connection (creating a data pipe)---*/
		clientfd = accept(sockfd, (struct sockaddr*)&client_addr, &addrlen);
		//receive the key
		char buffer[MAXBUF];
		char key[KEYLEN];
		bzero(key,sizeof(key));
		read_bytes=recv(clientfd,key,MAXBUF,0);
		if(read_bytes==0)
			printf("receive key failed");
		else
		{
			printf("%s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			//memcpy(key,buffer,KEYLEN-1);
			printf("key:%s\n",key);
			
		}
		memcpy(buffer,"I received the key",30);
		// up is ok
		
		/*---Echo back anything sent---*/
		send(clientfd, buffer, 30, 0);
		// up is ok
		/* receive the file */
		//get file name
		char filename[KEYLEN];
		bzero(filename,KEYLEN);
		read_bytes=recv(clientfd,filename,KEYLEN+1,0);
		if(read_bytes<=0)
		{
			printf("File name receive failed!\n");
			return 0;
		}
		printf("File name:%s\n",filename);
		//up is ok
		/* begin receiving the file contents */
		//create file by filename
		FILE *recvfile;
		recvfile=fopen("data2","a+");
		char *eof="EOF";
		bzero(buffer,MAXBUF);
		while(read_bytes=recv(clientfd,buffer,MAXBUF,0)>0)
		{
			if(memcmp(eof,buffer,3)==0)
				break;
			fprintf(recvfile,"%s",buffer);
				//printf("write failed:%s\n",buffer);
			bzero(buffer,MAXBUF);
		}
		fclose(recvfile);
		printf("Received 1 file:%s\n",filename);
		/*---Close data connection---*/
		close(clientfd);
	}

	/*---Clean up (should never get here!)---*/
	close(sockfd);
	return 0;
}

