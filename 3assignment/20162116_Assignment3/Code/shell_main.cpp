#include "include/includeFiles.h"

/*string command;
std::vector <string> parameters;*/
//fstream file;
std::vector <string> parameters;
std::map <int, string> history;
#include "commands.h"
using std::vector;

void implementRedirection(int ind,string command);
void computeBangOperator(string command);

void executeCommands(string command)
{		
	parameters.clear();
	splitCommand(command, &parameters);
//	cout<<command<<" -------- "<<endl;
	string cmd = parameters.at(0);
	//if(command.compare("!!")!=0)
	if(command.at(0)!='!')
		addHistory(command);
	int found;
	if((found = command.find('>')) != string::npos || (found=command.find('<'))!=string::npos)
		implementRedirection(found, command);
	else if ((found = command.find('|')) != string::npos)
		outputPipes(found, command);
	else if((found = command.find('=')) != string::npos)
		implementExport(found, command);	
	else if(cmd.compare("history")==0)
		printHistory(command,parameters);
	else if(cmd.compare("pwd")==0)
		cout<<printCWD()<<endl;
	else if(cmd.compare("echo")==0)
		parseEcho(command, parameters);
	else if(cmd.compare("clear")==0)
		cout<<"\x1B[2J\x1B[H";
	else if(cmd.compare("cd")==0)
		computeCD(parameters.at(1));
	//else if(command.compare("!!")==0)
	else if(command.at(0)=='!')
		computeBangOperator(command);
	else
		externalCommands(parameters);
//		cout<<"./MyShell:"<<cmd<<" command not found\n";
}
//Function to compute bang operator
void computeBangOperator(string command)
{
	char ch = command.at(1);
	//cout<<ch<<endl;
   	if(ch=='!')
    {
        string cmd = history[history.size()];
        cout<<cmd<<endl;
        executeCommands(cmd);
    }
    else if(ch=='-')
    {
    	string val = command.substr(2);
    	char *value = new char[val.length()+1];
            strcpy(value, val.c_str());
    	int no= atoi(value);
    	//cout<<no<<"   "<<history[history.size()=1-no]
    	cout<<history[history.size()+1-no]<<endl;
    	executeCommands(history[history.size()+1-no]);
    }
    else if('0'<=ch && ch<='9')
    {
    	//cout<<"This is number\n";
    	string val = command.substr(1);
    	char *value = new char[val.length()+1];
        strcpy(value, val.c_str());
    	int no= atoi(value);
    	cout<<history[no]<<endl;
    	executeCommands(history[no]);

    }
    else if(('a'<=ch && ch<='z') || ('a'<=ch && ch<='z' ))
    {
    	//cout<<"This is character\n";
    	string val = command.substr(1);
    	char *value = new char[val.length()+1];
        strcpy(value, val.c_str());
        int st_index = history.size();
        int i;
        for(i=st_index; i>0; i--)
        {
        	//cout<<history[i]<<endl;
        	char *hist = new char[history[i].length()+1];
        	strcpy(hist, history[i].c_str());
        	//cout<<hist<<value<<endl;
        	if(strstr(hist, value)!=NULL)
        		break;
        	
        }
        //cout<<i<<endl;
    	if(i!=0)
    	{
    	cout<<history[i]<<endl;
    	executeCommands(history[i]);
    	}
    	else
    		cout<<"./MyShell: "<<command<<" : event not found"<<endl;
    }
}

//Function to implement Redirection

//assumed there are atleast one single space between each redirection operator
//append not implemented
void implementRedirection(int ind,string command)
{
    bool append=false;
    char oper = command.at(ind);
    string commd = command.substr(0, ind);
    string file = command.substr(ind+1);
    string cmd;

    if(oper=='>' && command.at(ind+1)=='>')
    {
        append= true;
        file = command.substr(ind+2);
    }
 
    std::vector <string> redirection_filename;
    redirection_filename.clear();
    splitCommand(file, &redirection_filename);

    char *filename = new char [redirection_filename[0].length()+1];
    strcpy(filename, redirection_filename[0].c_str());

    int fin=0;	//to verify whether operator is '<'
    int sini;	//to reassign 1 ie output
    int infd;	//file descriptor
    if(oper == '<')
    {
    	fin=1;
 
        infd = open(filename, O_RDONLY);
        if(infd < 0)
        	cout<<"MyShell: "<<strerror(errno)<<endl;
        else
        {
	   		sini=dup(0);
	        close(0);
	        dup2(infd,0);
	    }
    }

    int fout=0;	//to verify whether operator is '>'
    int sout;	//to reassign 1 ie output
    int outfd;	//file descriptor
    if(oper == '>')
    {
    	fout=1;
    	
    	if(append)
    		outfd = open(filename, O_WRONLY | O_APPEND | O_CREAT, S_IRWXU);
    	else
        	outfd = open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRWXU);
        if(outfd < 0)
        	cout<<"MyShell: "<<strerror(errno)<<endl;
        else
        {
	        sout=dup(1);
	        close(1);
	        dup2(outfd,1);
	    }
    }

    executeCommands(commd);

	if(fout==1)
	{
		fout=0;
		close(outfd);
		dup2(sout,1);
	}

	if(fin==1)
	{
		fin=0;
		close(infd);
		dup2(sini, 0);
	}

}




//Child process executes the commands while the parent process waits for child to complete
void createChildProcess(string command)
{
	pid_t pid;
    int status;

    pid = fork();
    if(pid<0)
    {
    	cout<<"./MyShell:"<<strerror(errno)<<endl;
    	exit (1);
    }
    else if(pid==0)
    {
		/*int ret = */executeCommands(command);
    }
    else
    	//while (wait(&status) != pid);	//waits for child's completion
    	wait(NULL);
}

void sighandler(int sig)
{
//	string command;
	//cout<<"\nMyShell:"<<printCWD()<<"$ ";
//	command = "";
//	getline(std::cin,command);
	//addHistory(command);
	//createChildProcess(command);
	return;
}

int main()
{

	signal(SIGINT,sighandler);
	string command;
	/*if(chdir("/home/adithya")<0)
		cout<<"./MyShell:"<<strerror(errno)<<endl;*/
	cout<<"\x1B[2J\x1B[H";
	createMapHistory();
	while(1)
	{
	//	signal(SIGINT,sighandler);
		cout<<"MyShell:"<<printCWD()<<"$ ";
		command = "";
		getline(std::cin,command);
		
		while(command.compare("exit")==0)
		{
			cout<<"hi"<<endl;
			exit(0);
		}
		if(command.compare("") == 0)
			continue;
		createChildProcess(command);
	}
	//file.close();
	return 0;
}
