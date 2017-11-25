
/***** Program to implement split+tac command of Unix **********/

/*****	Name   : A Adithya	*******/
/*****	Roll No: 20162116	*******/

#include <stdio.h>
#include <stdlib.h>
#include  <string.h>
#include "syscall.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include "stack.c"	/* Program uses stack */

int pathCreation(char* file_path, mode_t mode);
int writeToFiles(char* iptFile, char* out, char* f_name, int lines, char *input);

int main(int argc, char *argv[])
{
	if(argc != 4)
	{
        printf ("Usage: %s <inputfile1> <outputfile2> <no_of_lines>",argv[0]);	//file1 is input file, file2 is output file, no_of_lines to be split
        return 1;
    }
	int input_fd, output_fd;    /* Input and output file descriptors */
	struct stat st_buf;
    DIR *optDir = NULL;
    DIR *iptDir = NULL;
    struct dirent *iptDptr = NULL;
    struct dirent *optDptr = NULL;
    int createOptFile = 0;
    char *optFile;
    char *iptFile;	
    
    optFile = (char *)malloc(128);
    strcpy(optFile,argv[2]);
    iptFile = (char *)malloc(128);
    strcpy(iptFile,argv[1]);

    if(optFile[strlen(optFile)-1] != '/')
        strncpy(optFile+strlen(optFile),"/",1);

    if(iptFile[strlen(iptFile)-1] != '/')
        strncpy(iptFile+strlen(iptFile),"/",1);
	
    // Open the input directory
    if((iptDir = opendir(iptFile)) == NULL)
    {
        printf("\n%s: cannot open '%s' for reading: %s\n",argv[0],argv[1],strerror(errno));
        return 1;
    }
    if((optDir = opendir(optFile)) == NULL)
        createOptFile = 1;	//output directory is not present
    
    if(createOptFile == 1)	//opt dir is not present and is creating opt directory
    {
    	int success;
    	success = pathCreation(optFile, S_IRWXU|S_IRWXG|S_IRWXO);
    	if(success ==0)
    		optDir = opendir(optFile);
    	else
    	{
    		printf("%s: %s\n",argv[0],strerror(errno));
    		return 1;
    	}
    }
 	/* Reading through files of input directory */
    while((iptDptr = readdir(iptDir)) != NULL)
    {	/* skip current and parent directories */
    	if(strcmp(iptDptr->d_name, ".") == 0 || strcmp(iptDptr->d_name, "..") == 0)
    		continue;
 		int status = stat (iptDptr->d_name, &st_buf);
 		if (!(S_ISDIR (st_buf.st_mode)))	/* sends only files skipping other directories, if any */
 			writeToFiles(iptFile,optFile,iptDptr->d_name,atoi(argv[3]),argv[1]);
 		else
 			printf("%s: %s is not a regular file",argv[0], iptDptr->d_name);
    }

	printf("%s:Succesfully created required files\n", argv[0]);
	return 0; 
}

int writeToFiles(char* iptFile, char* out, char* f_name, int lines, char* input)
{
	if(input[strlen(input)-1] != '/')
        strncpy(input+strlen(input),"/",1);
	char *p,*q;
	strncpy(iptFile+strlen(iptFile),f_name,strlen(f_name));
	int input_fd=0;

		/* Create input file descriptor */
 	input_fd = open (iptFile, O_RDONLY);

	memset(iptFile,0,128);
	strcpy(iptFile,input);	//resetting iptFile to argv[1]
	int cnt =1;
	char buffer;
	ssize_t ret_in;
	char file_name[128];	
	char integer_string[4];
	int i;
	char c;

	p=strchr(f_name, '.');
  	  	*p = '\0';		//filename has filename without extension
  	
  	q=p+1;	//q has extension
  	
  	memset(file_name,0,sizeof(file_name));
    strcpy(file_name,out);  	

    strncat(file_name,f_name,strlen(f_name));
    strncat(file_name,"_",1);

    int len=strlen(file_name);	//used in while loop - don't change its position
 								//this contains stringlength upto _ ie filename_ to add cnt later
    sprintf(integer_string, "%d", cnt);
    strncat(file_name,integer_string,strlen(integer_string));
    strncat(file_name,".",1);
    strncat(file_name,q,strlen(q));
	int output_fd;
	output_fd = creat(file_name, S_IRWXU);

	i=0;
	int no_of_lines = 0;
	int no_of_characters = 0;
	int cur_pos = 0, offset = 0, prev_cur_pos =0;
	struct Position pos;
	while ((ret_in = read (input_fd, &c, 1)) > 0) /* reading the characters from the file */
  	{
  		++no_of_characters;
  		cur_pos = no_of_characters;

	 	i++;
  		if(c == '\n')	/* checking for new line */
  		{
  			no_of_lines++;
  			offset = i;
  			/* pushing current pos and offset to print in reverse order after all lines are completed */
  			pos.current_pos = prev_cur_pos;
  			pos.cur_offset = offset;
  			push(pos);
  			i=0;
  			prev_cur_pos = cur_pos;

			if((no_of_lines%lines) == 0)	/* 'lines' is passed through function which is no of lines to be written */
  			{
  				while(!isEmptyStack())	/*poping the current position and offset to print in reverse order */
  				{
  					pos=pop();
  					char buf[pos.cur_offset];
  					if(lseek(input_fd,pos.current_pos,0) >=0 )
  					{
  						char ch;
  						int k,a=0;
  						while(a<pos.cur_offset)
  						{
  							if((k=read(input_fd,&ch,1))>0)
  								buf[a] = ch;
  							a++;
  						}
  						buf[a]='\0';
  					}
  					write(output_fd,buf,sizeof(buf));
				}
				lseek(input_fd,prev_cur_pos,0);	/* setting the position of file descriptor to point before popping */
  				close(output_fd);
  				/* changing file name ie adding suffixes _i  and creating it to write */
  				cnt++;
				sprintf(integer_string, "%d", cnt);
				file_name[len]='\0';
   				strncat(file_name,integer_string,strlen(integer_string));
    			strncat(file_name,".",1);
    			strncat(file_name,q,strlen(q));
    			output_fd = creat(file_name, S_IRWXU);
  			}
  		}
 	}
 	/*  writing to the last file where no of lines is less than prescribed to print */
 	while(!isEmptyStack())
  	{
  		pos=pop();
  	//	printf("%d--%d\n",pos.current_pos, pos.cur_offset);
  		char buf[pos.cur_offset];
  		if(lseek(input_fd,pos.current_pos,0) >=0 )
  		{
  			char ch;
  			int k,a=0;
  			while(a<pos.cur_offset)
  			{
  				if((k=read(input_fd,&ch,1))>0)
  					buf[a] = ch;
  				a++;
  			}
  			buf[a]='\0';
  		}
  		write(output_fd,buf,sizeof(buf));
	}
	close(output_fd);
	close(input_fd);
	return 0;
}

int pathCreation(char* file_path, mode_t mode) {
	
  char* p;
  p=strchr(file_path+1, '/');//strchrreturns next occurance of '/' in filepath
  while(p) 
  {
    *p='\0';
    if (mkdir(file_path, mode)==-1) 
    {
      if (errno!=EEXIST) 
      	{ 
      		*p='/';
      		return -1; 
  		}
    }
    *p='/';
     p=strchr(p+1, '/');
  }
  return 0;
}
