#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <thread>
#include <cstdio>
#include <vector>
#include <map>
#include <time.h>
#include <netdb.h>
#include <errno.h>
#include <limits.h>

using namespace std;

string commands[3]; 

void processRequest(int client_fd)
{
	char *data = "Hello Client";
	for(int i=0; i<100000000; i++)
		write(client_fd,data,strlen(data));
}

void server_function(string port)
{
/*	cout<<"Hi ";
	cout<<port<<endl;*/
	int socketfd, port_no, connectedfd;
	socklen_t client_len;
	struct sockaddr_in serv_addr, client_addr;

	socketfd = socket(AF_INET, SOCK_STREAM,0);
	if(socketfd<0)
	{
		perror("Error opening socket...\n");
		return;
	}	
	//cout<<"Socket created with socket fd "<<socketfd<<" succesfully\n";
	bzero((char *) &serv_addr, sizeof(serv_addr));
	port_no = atoi((port).c_str());

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_no);

	if(bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Binding socket to Server Address failed..\n");
		return;
	}
	//cout<<"Binding of socket to Server succesful\n";
	while(1)
	{
		listen(socketfd, SOMAXCONN);

	//	cout<<"Waiting for clients...\n";
		char buffer[256];
		client_len = sizeof(client_addr);
		bzero((struct sockaddr *) &client_addr, client_len);
		bzero(buffer,256);

		connectedfd = accept(socketfd, (struct sockaddr *) &client_addr, &client_len);
		string client_ip(inet_ntop(AF_INET,&client_addr.sin_addr, buffer, sizeof(buffer)));
		int client_port = ntohs(client_addr.sin_port);
		if(connectedfd < 0){
			cout<<"Server failed to accept connection\n";
			return;
		}
		thread clientFn(processRequest, connectedfd);
		clientFn.detach();
		
		//cout<<"Connection from "<<client_ip<<", port: "<<client_port<<" is accepted\n";
	}
}

void client_function(string ip_addr, string conn_port)
{
	struct sockaddr_in serv_addr;
	struct hostent *server;
	int port_no, socketfd;
	

	bzero((char *) &serv_addr, sizeof(serv_addr));
	port_no = atoi(conn_port.c_str());

	server = gethostbyname(ip_addr.c_str());
    if (server == NULL) 
    {
        cout<<ip_addr<<" No such host...\n";
        return;
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
	cout<<"Client Connected ...! \n";
	char buffer[256];
	int cnt = 1;
	while(1)
	{
		bzero(buffer, 255);
		int recvd = read(socketfd,buffer, 12);
		if(recvd<0)
			break;
		cout<<cnt++<<" "<<buffer<<"\n";
	}
}

void splitCommand(string command)
{
    string buf; 
    int no=0;
    stringstream ss(command);
    while (ss >> buf)
       // cout<<buf<<endl;
    	commands[no++] = buf;
}

int main(int argc, char const *argv[])
{
	string str;
	string port_no("");
	int createFlag = 0;
	while(1)
	{
		cout<<"\npastry >> ";
		getline(std::cin,str);

		splitCommand(str);
	    
	    if(commands[0] == "port")
	    {
	    	port_no = commands[1];
	    	if(port_no != "")
		    	cout<<"Port set to "<<commands[1]<<endl;
	    	else
	    		cout<<"Node ip and port not set, Give port No\n";
	    }
	    else if(commands[0] == "create")
	    {
	    	if(port_no == "")
	    		cout<<"Node ip and port not set, Give port No\n";
	    	else{
	    		createFlag = 1;
		    	thread serverFn(server_function,port_no);
		    	serverFn.detach();
		    	cout<<"Created Node with port \n";
		    	//cout<<"Successfully created threads..\n";
		    }
	    }
	    else if(commands[0] == "join")
	    {
	    	string ip_addr = commands[1];
	    	string conn_port = commands[2];
	    	if(port_no == "")
	    		cout<<"Node ip and port not set, Give port No\n";
	    	else if(createFlag != 1)
	    		cout<<"create Node first using command <create>\n";
	    	else 
	    	{
	    		if(ip_addr == "" || conn_port == "")
		    		cout<<"Enter IP address and port No\n";
		    	else
		    		client_function(ip_addr, conn_port);
		    }
	    }
	    else
	    	cout<<"command NOT found\n";
	    //cout<<endl;
	}

	return 0;
}