

//Function to print all map elements
void printAll()
{
    std::map<int, string>::iterator it;
    for (it=history.begin(); it!=history.end(); ++it)
    cout << setw(4)<<it->first << "  " << it->second << '\n';
}
//Function to print last n numbers of history
void printLast(int n)
{
    int size = history.size();
    int st_index = size-n+1;
    for(int i=st_index; i<=size; i++)
        cout << setw(4)<<i<< "  " << history[i] << '\n';
}

//Function to write to history.txt
void addHistory(string command)
{
  //  cout<<command<<"   hist\n";
    fstream file;
    if(history[history.size()] != command)
    {
        history[history.size()+1] = command;
        file.open("history.txt", ios::app);
        file<<setw(5)<<history.size()<<"#"<<command<<endl;
        file.close();
    }
}

//Function to print History
void printHistory(string command, vector <string> parameters)
{
   // addHistory(command);
    if(parameters.size()==1)
        printAll();
    else
    {
        char *nos = new char[parameters[1].length()+1];
        strcpy(nos, parameters[1].c_str());
        int no = atoi(nos);
        printLast(no);
    }
}

//Function to create map of history
void createMapHistory()
{
    fstream his_file;
    his_file.open("history.txt",ios::in);
    if(!his_file)
    {
        his_file.open("history.txt",ios::out);
        return;
    }
        //cout<<"yes\n";
    char cmd[300];
    int count=0;
    while(!his_file.eof())
    {
        his_file.getline(cmd, 300);       
        if(strcmp(cmd,"")!=0)
        {
            string cmmd(cmd);
            int found = cmmd.find('#');  //delimitter in history file
            string ind = cmmd.substr(0, found);
            string commd = cmmd.substr(found+1);
            //cout<<ind<<" "<<commd<<endl;
            char *index = new char[ind.length()+1];
            strcpy(index, ind.c_str());
            char *value = new char[commd.length()+1];
            strcpy(value, commd.c_str());
            count++;
            int in = atoi(index);

            history[in]=value;
        }
    }
    his_file.close();
}

//Function to implement splitting command and passing into vector parameters
void splitCommand(string command, vector <string> *parameters)
{
    string buf; 
    stringstream ss(command);
    while (ss >> buf)
        parameters->push_back(buf);
}

//Fucntion to remove quotations
string removeQuotations(string command)
{
   // cout<<command<<endl;
    char *cmd1 = new char [command.length()+1];
    strcpy(cmd1, command.c_str());
    //cout<<cmd1;
    int len = strlen(cmd1);
    int ind=0;
    for(int i=0; i<len; i++)
    {
        if(cmd1[i]=='\'' || cmd1[i]=='"')
            continue;
        else
            cmd1[ind++]= cmd1[i];
    }
    cmd1[ind]='\0';
    string cmd(cmd1);
  //  cout<<cmd<<endl;
    return cmd;
}

//Function to implement export function
//implmented all combinations of export with spaces before and after '='
void implementExport(int index, string command)
{
    string cmd = removeQuotations(command);
    string name = cmd.substr(0,index);
    string value = cmd.substr(index+1);
    char last = name.at(index-1);//<<endl;
    char first = value.at(0);//<<endl;
    if(last == ' ')
        cout<<name<<": command not found\n";
    else if(first == ' ')
        cout<<value.erase(0,1)<<": command not found\n";
    else{
        char *name1 = new char [name.length()+1];
        strcpy(name1, name.c_str());
        char *value1 = new char [value.length()+1];
        strcpy(value1, value.c_str());

        int status = setenv(name1, value1,1);   // 1 is to overwrite the already present name1 in env
        if(status == -1)
            cout<<"./MyShell:"<<strerror(errno)<<endl;
       // else
            //addHistory(command);
    }
}


//Function to implement pipes
void outputPipes(int first, string command)
{
 //  int first = command.find('|');
    string current = command.substr(0,first);
    string next = command.substr(first+1);

    int no = count(command.begin(), command.end(), '|');  //#include <algorithm>  

    no++;
    pid_t pid;
    int fd[2],j;

    for(int i=0; i<no-1; i++)
    {
        char *current1 = new char [current.length()+1];
        strcpy(current1, current.c_str());

      //  cout<<current1<<endl;        
        char *p = strtok (current1," ");
        char *cur_params[64];
        char **split = cur_params;

        while (p != NULL)
        {
            *split++ = p;
            p = strtok(NULL, " ");
        }
        *split = NULL;

        pipe(fd);
        pid = fork();
        if(pid==0)
        {
            dup2(fd[1],1); 
            execvp(cur_params[0],cur_params);
        }
        else{
            dup2(fd[0],0);
            close(fd[1]);      
        }
        first = next.find('|');
        current = next.substr(0,first);
        next = next.substr(first+1);
    }

    char *current1 = new char [current.length()+1];
    strcpy(current1, current.c_str());

   // cout<<current1<<endl;        
    char *p = strtok (current1," ");
    char *cur_params[64];
    char **split = cur_params;

    while (p != 0)
    {
        *split++ = p;
        p = strtok(NULL, " ");
    }
    *split = NULL;
    execvp(cur_params[0], cur_params);
}

//Function to implement external commands using execvp
void externalCommands(vector <string> parameters)
{
    string cmd = parameters.at(0);
    char *cstr = new char [cmd.length()+1];
    strcpy (cstr, cmd.c_str());

    int no_of_commands = parameters.size();
    char  *argv[64];
    int i;
    for(i=0; i<no_of_commands; i++)
    {
        argv[i] = new char [parameters.at(i).length()+1];
        strcpy(argv[i], parameters.at(i).c_str());
    }
    argv[i]=NULL;
    //addHistory(command);
    if (execvp(*argv, argv) == -1) {  
        cout<<"./MyShell:"<<strerror(errno)<<endl;
        exit(1);
    }
  }

//Function to get current working directory
char* printCWD()
{
	char *cwd;
	cwd = (char *)malloc(1024*sizeof(char));
   	if (getcwd(cwd, 1024) == NULL)
   	{
   		perror("Unable to get present working directory");
   		return NULL;
   	}
    //file<<command<<endl;
    return cwd;
    
}
//Function to compute CD Command
void computeCD(string str)
{
	char *dir = new char[str.length()+1];
	strcpy(dir, str.c_str());
	if(chdir(dir)<0)
		cout<<"./MyShell:"<<strerror(errno)<<endl;
}

//Function to parse echo command
void parseEcho(string command, vector <string> parameters)
{
    //addHistory(command);
    int dollar;
    if(parameters.size() == 1)	//empty parameter to echo
		  cout<<"\n";
    //for environmental variables
    else if ((dollar=command.find('$')) != string::npos)
    {
        int i=1;
        string env;
        for(i=1;i<parameters.size();i++)
        {
            if(parameters[i].at(0)=='"')
                env = parameters[i].substr(2, parameters[i].length()-3);
            else
                env = parameters[i].substr(1, parameters[i].length()-1);
            if(parameters[i].find('$') == string::npos)
            {
                cout<<parameters[i]<<" ";
                continue;
            }
            char *cstr = new char [env.length()+1];
            strcpy (cstr, env.c_str());
            if(env.compare("PWD") == 0)
            {
                cout<<printCWD()<<" ";
                continue;
            }
            char *st = getenv(cstr);
            if(st!=NULL)
              cout<<st<<" ";
        }
        cout<<endl;

    }
    else    /*****for normal string output *****/
    {
//echo    am     ashok    worki for    high school   "this is     "adithya    wo is good boy    sdfadsgjlksadjg
//echo "this    is adithya   'hi'   jklk"   hello    i am
//echo this    is aidhtya      sdf'dfgfsdgh'
        int d_quotes_start = command.find('"');
        if(d_quotes_start != string::npos)  //when there are double quotes
        {
            int d_quotes_end = command.find('"', d_quotes_start+1);
            int len = d_quotes_end - d_quotes_start+1;


            for(int i=1; parameters[i].at(0)!='"'; i++)
                    cout<<parameters[i]<<" ";
            cout<<command.substr(d_quotes_start+1, len-2);

            string after_d_quotes = command.substr(d_quotes_end+1);
            
            char *cstr = new char [after_d_quotes.length()];    // not lenght()+1 bcoz already removed " by 
            strcpy (cstr, after_d_quotes.c_str());              // using d_quotes_end+1 in prev stmt
            cout<<cstr[0];
            for(int i=1; cstr[i] != '\0'; i++)
            {
                    if(cstr[i] == ' ' && cstr[i-1]== ' ')
                        continue;
                    cout<<cstr[i];
            }   
            cout<<endl;
        }
        else    // when there are no double quotes
        {
            for(int i=1; i<parameters.size(); i++)
                cout<<parameters[i]<<" ";
            cout<<endl;
        }
    }
}