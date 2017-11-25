#include <cstdio>
#include <unistd.h>
#include <vector>
#include <map>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

using namespace std;

void shareFile(int sockfd);
void searchFile(int sockfd);
void splitAndWrite(char *arr);

vector<string>v;

int main(int argc, char const *argv[])
{
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int port_no, socketfd,option;
	if(argc<3)
	{
		cout<<"Usage:"<<argv[0]<<" <serverIP> <Port No>"<<endl;
		return 0;
	}

	bzero((char *) &serv_addr, sizeof(serv_addr));
	port_no = atoi(argv[2]);

	server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
        cout<<argv[0]<<" No such host...\n";
        _exit(0);
    }

    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port_no);

	cout<<"***** Now at client machine *****"<<endl;

	socketfd = socket(AF_INET, SOCK_STREAM,0);
	if(socketfd<0)
	{
		perror("Error opening socket...\n");
		exit (1);
	}
	if(connect(socketfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
	{
		perror("Failed to connect to sever");
		exit(1);
	}

	while(1)
	{
		cout<<"\n\nEnter an option:"<<endl;
		cout<<"1.Search for a file"<<endl<<"2.Share a file"<<endl<<"3. Exit"<<endl;
		cout<<"--->>";
		cin>>option;

		switch(option)
		{
			case 1:
				searchFile(socketfd);
				break;
			case 2:
				shareFile(socketfd);
				break;
			case 3:
				close(socketfd);
				return (0);
		}
	}
	close(socketfd);

	return 0;
}

void searchFile(int sockfd)
{
	char buffer[256],buffer1[2048], filename[32];
	int sent,recieved;
	bzero(filename,32);
	bzero(buffer,256);
	bzero(buffer1,2048);
	strcpy(buffer,"1");
	cout<<"***Enter the file to search :";
	cin>>filename;
	strcat(buffer,"@");
	strcat(buffer,filename);
	sent = write(sockfd,buffer, strlen(buffer));
	if(sent <0){
		cout<<"Search Request to server failed.."<<strerror(errno)<<"\n";
		return;
	}
	if((recieved = recv(sockfd,buffer1,2048,0))>0)
	{
		//write(1,buffer1,sizeof(buffer1));
		splitAndWrite(buffer1);
	}
	return;	 
}
void splitAndWrite(char *arr)
{
	int i=1;
	char* pch = strtok (arr,"\t");
	while (pch != NULL)
  	{
  		cout<<i<<". "<<pch<<endl;
  		v.push_back(pch);
    	pch = strtok (NULL, "\t");
    	i++;
  	}
  	cout<<endl;
	return;
}

void shareFile(int sockfd)
{
	char buffer[256], filename[32];
	int sent,recieved;
	bzero(filename,32);
	bzero(buffer,256);
	strcpy(buffer,"2");
	cout<<"***Enter the file to share :";
	cin>>filename;
	char buff[PATH_MAX + 1];
	bzero(buff,256);
	char *cwd = getcwd( buff, PATH_MAX + 1 );
    if( cwd == NULL )
      	cout<<"Unable to get absolute path.."<<endl;
	strcat(buffer,"@");
	strcat(buffer, cwd);
	strcat(buffer, "/");
	strcat(buffer, filename);

	sent = write(sockfd,buffer, strlen(buffer));// send(socketfd, buffer, strlen(buffer),0);
	if(sent <0){
		cout<<"Share Request to server failed.."<<strerror(errno)<<"\n";
		return;
	}
	bzero(buffer,256);

	recieved = read(sockfd, buffer,255);// recv(socketfd, buffer, sizeof(buffer), 0))> 0)
	if(recieved<0)
		cout<<"Data Not recieved..\n";
	else
	{
		write(1,buffer,strlen(buffer));
		//cout<<buffer<<endl;
	}
	 
	return;
}