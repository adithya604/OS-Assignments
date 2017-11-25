#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>	// for inet_ntop function
#include <map>
#include <utility>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>

using namespace std;

vector<pair<string, pair<string, string> > >v;
void sendSearchResults(int clientfd, string filename);
void serveConnectedClient(string client_ip, int clientfd);
void addToRepo(string client_ip,string data, int clientfd);
void createVectorDataStructure();

int main(int argc, char const *argv[])
{
	int socketfd, port_no, connectedfd;
	socklen_t client_len;
	struct sockaddr_in serv_addr, client_addr;
	char buffer[256];
	if(argc<2)
	{
		cout<<"Syntax : "<<argv[0]<<" <server port number>\n";
		exit(1);
	}
	v.clear();

	createVectorDataStructure();

	socketfd = socket(AF_INET, SOCK_STREAM,0);
	if(socketfd<0)
	{
		perror("Error opening socket...\n");
		exit (1);
	}
	
	cout<<"Socket created with socket fd "<<socketfd<<" succesfully\n";
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	port_no = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port_no);

	if(bind(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("Binding socket to Server Address failed..\n");
		exit(1);
	}
	cout<<"Binding of socket to Server succesful\n";

	while(1)
	{
		listen(socketfd, SOMAXCONN);
		cout<<"Waiting for clients...\n";

		client_len = sizeof(client_addr);
		bzero((struct sockaddr *) &client_addr, client_len);
		bzero(buffer,256);

		connectedfd = accept(socketfd, (struct sockaddr *) &client_addr, &client_len);
		string client_ip(inet_ntop(AF_INET,&client_addr.sin_addr, buffer, sizeof(buffer)));
		int client_port = ntohs(client_addr.sin_port);
		if(connectedfd < 0){
			cout<<"Server failed to accept connection\n";
			continue;
		}
		cout<<"Connection from "<<client_ip<<", port: "<<client_port<<" is accepted\n";
		
		serveConnectedClient(client_ip,connectedfd);
		close(connectedfd);
	}
	return 0;
}

void serveConnectedClient(string client_ip, int clientfd)
{
	int recieved, sent;
	char buffer[256];
	string buf;
	do
	{
		bzero(buffer,255);
		recieved = read(clientfd,buffer, 255);
		if(recieved<0)
		{
			cout<<"unable to read data from client"<<endl;
			return;
		}
		switch(buffer[0])
		{
			case '1':
				cout<<"search reqested\n";
				buf.assign(buffer);
				buf = buf.substr(2);
				sendSearchResults(clientfd, buf);
				break;
			case '2':
				cout<<"share requested\n";
				buf.assign(buffer);
				addToRepo(client_ip,buf, clientfd);
				break;
		}

	}while(recieved>0);
	return;
}

void sendSearchResults(int clientfd, string filename)
{
	char str[256], dt[2048];
	string records[3],str1;
	char *fname = new char [filename.length()+1];
	strcpy (fname, filename.c_str());
	ifstream infile("repo.txt");
	int i,cnt=0, sent;
	bzero(dt,2048);
	while(!infile.eof())
	{
		bzero(str,256);
		infile.getline(str, 256);
		i=0;
		char* pch = strtok (str,"@");
		while (pch != NULL)
  		{
  			records[i] = pch;
    		pch = strtok (NULL, "@");
    		i++;
  		}
  		if(records[0] == fname)
  		{
  			str1 = records[1]+"@"+records[2]+"\t";
  			strcat(dt,str1.c_str());
			cnt++;
  		}  		
	}	
	if(cnt==0)
	{
		strcpy(str,"No records found\n");
		sent = write(clientfd,str,strlen(str));
	}
	else
		sent = write(clientfd,dt,strlen(dt));
	return;
}

void addToRepo(string client_ip, string data, int clientfd)
{
    string path = data.substr(2);
    string filename = data.substr(data.rfind("/")+1);
    pair<string,string> p1;
    p1 = make_pair(client_ip, path);
    pair<string, pair<string,string> >p2;
    p2 = make_pair(filename, p1);
   	v.push_back(p2);

   	string full = filename+"@"+client_ip+"@"+path+"\n";
   	ofstream file("repo.txt",ios::out | ios::app);
	if(file.is_open())
	{
		int sent = write(clientfd,"Share request processed\n",25);
		if(sent <0)
			cout<<"could not send reply\n";
		file<<full;
	}
	else
		cout<<"file is not created\n";

	file.close();
	return;
}

void createVectorDataStructure()
{
	char str[256];
	string records[3];
	ifstream infile("repo.txt");
	int i;
	while(!infile.eof())
	{
		infile.getline(str, 256);
		//cout<<str<<endl;
		i=0;
		char* pch = strtok (str,"@");
		while (pch != NULL)
  		{
  			records[i] = pch;
    		pch = strtok (NULL, "@");
    		i++;
  		}
  		pair<string,string> p1;
 	   	p1 = make_pair(records[1], records[2]);
    	pair<string, pair<string,string> >p2;
    	p2 = make_pair(records[0], p1);
   		v.push_back(p2);
	}
	return;
}