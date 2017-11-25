
/***** Program to implement wc command of Unix  **********/

/*****	Name   : A Adithya	*******/
/*****	Roll No: 20162116	*******/

#include <stdio.h>
#include <stdlib.h>
#include  <string.h>
#include "syscall.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define IN 1	/* inside a word */
#define OUT 0	/* outside a word */

char* readInput(char*);
void decodeOptions(char *opts, char* inp);
int lineCount(char *inp,int longest);	//longest =0 implies only line count, =1 implies longest line length
int wordCount(char *inp);
long characterCount(char *inp);
long characterCountFile(int ipt_fd);
int lineCountFile(int ipt_fd, int longest);	//longest =0 implies only line count, =1 implies longest line length);
int wordCountFile(int ipt_fd);
long byteCountFile(int ipt);
void decodeOptionsFile(char *opts, int ipt_fd);
char* getCombinedOptionsAndFileIndex(int arc, char **arv, int *f_index);

int main(int argc, char *argv[])
{
	char *ipt;
	int i;
	int l,w,ll;
	long m;
	if(argc == 1)		/* with no parameters inc options */
	{
		ipt = (char*)malloc(sizeof(char));
		ipt = readInput(ipt);
	
		l = lineCount(ipt,0);	// 0 for getting just line count, 1 for getting longest line length
		w = wordCount(ipt);
		m = (int)strlen(ipt); 

		printf("\t%d\t%d\t%ld\n",l,w,m);
	}
	if(argc == 2)		
	{
		char *second = argv[1];
		if(second[0] == '-')	/* with options but not input file */
		{
			char c;
			i=1;
			while((c = second[i]) != '\0')	/* Checks for any invalid option given after -  */
			{								/* If found, prints error message and exits		*/
				if(!(c == 'l' || c == 'w' || c == 'L' || c == 'm' || c == 'c'))
				{
					printf("%s: Invalid Option -- '%c'\n",argv[0],c);
					return 1;
				}
				i++;
			}

			ipt = (char*)malloc(sizeof(char));
			ipt = readInput(ipt);	//read input from std input
							
			if(i>1)		/* decode given options and prints the desired o/p */
			{
				decodeOptions(second,ipt);
				printf("\n");
			}
			else		/* second argument has only '-' */
			{			/* takes std i/p and print line, word, character counts */
				l = lineCount(ipt,0);
				w = wordCount(ipt);
				m = (int)strlen(ipt); 
				printf("\t%d\t%d\t%ld -\n",l,w,m);
			}
		}
		else	//case when no option is given but filename is given
		{		//reads file, prints linecount, word count, character count
			char *filename = argv[1];
			int input_fd;

			/* Create input file descriptor */
   			input_fd = open (filename, O_RDONLY);
    		if(input_fd == -1) {
            	printf("%s: %s: %s\n",argv[0],filename,strerror(errno));
            	return 1;
    		}
    		l = lineCountFile(input_fd,0);
    		lseek(input_fd,0L,0);
			w = wordCountFile(input_fd);
			lseek(input_fd,0L,0);
			m = characterCountFile(input_fd);

			printf("  %d  %d  %ld  %s\n",l,w, m,filename);
			
		}
	}
	if(argc > 2)
	{
		int fileIndex = -1;	
		char* combinedOptions;
		combinedOptions = (char *)malloc(20);
		
		combinedOptions = getCombinedOptionsAndFileIndex(argc, argv, &fileIndex);

		i=0;
		char c;
		while((c=combinedOptions[i]) != '\0')
		{/* returns if one of the option is invalid option given */
			if(!(c == 'l' || c == 'w' || c == 'L' || c == 'm' || c == 'c' || c=='-'))
				{
					printf("%s: invalid option -- '%c'\n",argv[0],c);
					return 1;
				}
				i++;
		}

		if(fileIndex == -1) //no file name give in parameters, all start with '-'
		{					//read std i/p and print desired o/p of options
			ipt = (char*)malloc(sizeof(char));
			ipt = readInput(ipt);

			decodeOptions(combinedOptions,ipt);
			printf("\n");
		}
		else	//file name and options are given
		{
			char *filename = argv[fileIndex];
			int input_fd;

					/* Create input file descriptor */
 	 		input_fd = open (filename, O_RDONLY);
    		if(input_fd == -1) {
        	      	printf("%s: %s: %s\n",argv[0],filename,strerror(errno));
            	return 1;
            }
			decodeOptionsFile(combinedOptions,input_fd);
			printf("  %s\n", filename);
		}
	}
	return 0;
}
/* Function to decode all the given options, prints the required options reading contents from STDIN */
void decodeOptions(char* opts, char* inp)
{
	int i=0;
	char flags[6]={'\0'};
	while(opts[i] != '\0')
	{
		if(opts[i] == 'l') flags[0] = 'l';
		if(opts[i] == 'w') flags[1] = 'w';
		if(opts[i] == 'c') flags[2] = 'c';
		if(opts[i] == 'm') flags[3] = 'm';
		if(opts[i] == 'L') flags[4] = 'L';
		i++;
	}
	flags[i]='\0';
	i=0;
	while(flags[i] != '\0')
	{
		switch(flags[i])
		{
			case 'l':
				printf("  %d",lineCount(inp,0));
				break;
			case 'm':
					printf("  %ld",(long)strlen(inp));
		/*		printf("%ld\n",characterCount(inp)); */
				break;
			case 'w':
					printf("  %d",wordCount(inp));
				break;
			case 'c':
					printf("  %ld",(long)strlen(inp));
		/*		printf("%ld\n",characterCount(inp)); */
				break;	
			case 'L':
					printf("  %d",lineCount(inp,1));	//same code of line count with parameter 1, it returns longest line
				break;
		}
		i++;
	}
}
/* Function to decode all the given options, prints the required options reading contents from file descriptor */
void decodeOptionsFile(char *opts, int inp)
{
	int i=0;
	char flags[6]={'\0'};
	while(opts[i] != '\0')
	{
		if(opts[i] == 'l') flags[0] = 'l';
		if(opts[i] == 'w') flags[1] = 'w';
		if(opts[i] == 'c') flags[2] = 'c';
		if(opts[i] == 'm') flags[3] = 'm';
		if(opts[i] == 'L') flags[4] = 'L';
		i++;
	}
	flags[i]='\0';
	i=0;
	int checkRepeatedOptions[5] = {0,0,0,0,0};	// index 0 for lines, 1 for words, 2 for characters, 
	while(flags[i] != '\0')						// 3 for bytes, 4 for LongestLine
	{
		lseek(inp,0L,0);
		switch(flags[i])
		{
			case 'l':	//line count
					printf("  %d",lineCountFile(inp,0));
				break;
			case 'm':	//character count
					printf("  %ld",characterCountFile(inp));
				break;
			case 'w':	//word count
					printf("  %d",wordCountFile(inp));
				break;
			case 'c':	//byte count
					printf("  %ld",byteCountFile(inp));
				break;
			case 'L':	//longest Line length
				printf("  %d",lineCountFile(inp,1));
				break;
		}
		i++;
	}
}
/* Gets byte count of the file */
long byteCountFile(int ipt)
{
	struct stat fileStat;
    if(fstat(ipt,&fileStat) < 0)    
        return 1;
	return fileStat.st_size;
}
/* Combines scattered options and give index of the argv where file is passed */
char* getCombinedOptionsAndFileIndex(int arc, char **arv, int *f_index)
{
	char* combinedOptions;
	combinedOptions = (char *)malloc(20);
	int toFind[arc];
	
	for(int i=0; i<arc;i++)
			toFind[i] = 0;	//to check whether it is an option or filename 

	for(int i=1; i<arc;i++)
	{
		if(arv[i][0] == '-')
			toFind[i]++;	//toFind is set if it is an option
		else
			*f_index = i;	//fileIndex is set when argv[k][0] != '-'
	}
	for(int i=1; i<arc;i++)
	{
		if(toFind[i]>0)
			strcat(combinedOptions,arv[i]);
	}
	return combinedOptions;
}

/* Gets character count of the file */
long characterCountFile(int ipt_fd)
{
	long cnt =0;
	char buffer;
	ssize_t ret_in;
    while((ret_in = read (ipt_fd, &buffer, 1)) > 0){
    		cnt++;
    }
	return cnt;
}
/* Gets line count and longest line length of the file, based on the parameter longest */
int lineCountFile(int ipt_fd, int longest)
{
	int max=0, chars=0;
	int cnt =0;
	char buffer;
	ssize_t ret_in;
    while((ret_in = read (ipt_fd, &buffer, 1)) > 0){
 	  	chars++;
    	if(buffer == '\n')
    	{
 	  		if(max<chars)
    			max=chars-1;
    		cnt++;
    		chars=0;    	
    	}
    }
    if(longest == 1) return max;
	return cnt;
}
/* Gets line count and longest line length of input STDIN, based on the parameter longest */
int lineCount(char *inp, int longest)
{
	int nl=0, nc=0;
	int max=0,chars=0;
	while(inp[nc] != '\0')
	{
		chars++;
		if(inp[nc] == '\n')
		{
			if(max<chars)
				max = chars-1;	//since \n is taken as a character and line doesnot have \n as character
			chars=0;
			nl++;
		}
		nc++;
	}
	if(longest == 1) return max;
	return nl;
}
/* Gets word count  of the file */
int wordCountFile(int ipt_fd)
{
	int cnt =0;
	int c,nc=0,len, state = OUT;
	char buffer;
	ssize_t ret_in;
    while((ret_in = read (ipt_fd, &buffer, 1)) > 0){
    	if(buffer == ' ' || buffer == '\n' || buffer == '\t')
			state= OUT;
		else if (state== OUT)
		{
			state = IN;
			cnt++;
		}
    }
	return cnt;
}
/* Returns characte count of STDIN, not used bcoz we are storing int in string and strlen is used */
/*long characterCount(char *inp)
{
	printf("this is characterCount\n");
	long nc = 0;
	while(inp[nc]!='\0')
		nc++;
	return nc;
}*/
/* Gets word count of STDIN */
int wordCount(char *inp)
{
	int c,nw=0, nc=0,len, state = OUT;
	len = (int)strlen(inp);
	/*while(nc < len)*/
	while(inp[nc] != '\0')
	{
		c = inp[nc];
		if(c == ' ' || c == '\n' || c == '\t')
			state= OUT;
		else if (state== OUT)
		{
			state = IN;
			nw++;
		}
		nc++;
	}
	return nw;
}

/* Reads input from Std i/p each character at a time and returns that input as a string */
char *readInput(char *inp)
{
	int i, size;
	char c;
	i=0;
	size=1;
  	while ((c= getchar()) != EOF) 
	//while(read(0,&c,1) == 1)
  	{
 	 	inp = (char*)realloc(inp, size * sizeof(char));	/*increases in size as each character is read */
 	 	inp[i] = c;
 	 	
    	i++;
    	size++;
 	}
 	inp[i] = '\0';

 	return inp;
}
