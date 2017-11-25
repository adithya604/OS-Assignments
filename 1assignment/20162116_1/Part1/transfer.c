
/** Program to implement tr command of Unix **/

/***	Name		: A Adithya	***/
/***	Roll NO		: 20162116	***/

#include <stdio.h>
#include <stdlib.h>
#include  <string.h>
#include "syscall.h"
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define BUF_SIZE 105

void makeEqualCharacters(char *s1,char *s2);
char *transfer(char *s1, char *s2,char *input);
char* readInput(char *inp,int *n);
char *deleteCharacters(char *s1, char *input);
char *squeezeCharacters(char *s1, char *input);
char *complementSet(char *s1);
char *decodeSets(char *s1);
int pathCreation(char* file_path, mode_t mode);
void readAndPrint(char *s1, char *ipt_address, char *opt_address, char flags[], int len);
void transferCharachters(char *s1, char *s2, char *input_file, char *output_file);
char *getrange(char a, char b);

char *ascii_values, ascii_map[127]={'\0'};

int main(int argc, char *argv[])
{
	char *set1, *set2;

	if(argc == 2){
		printf("%s : Missing operand after '%s'\n",argv[0], argv[1]);
		exit(1);
	}
	if(argc == 1){
		printf("%s : Missing operand\n",argv[0]);
		exit(1);
	}
	/* Index of ascii values */
	ascii_values = (char *)malloc(127);
	for(int i=0; i<127;i++)
		ascii_values[i] = (char)i;

	set1 = (char *) malloc(127);
	set2 = (char *) malloc(127);

	char flags[3] = {'\0'};	//flag[0] for 'c' flag[1] for 'd' flag[2] for 's'
	int i, input_file_index=-1,output_file_index=-1;
	int invalid = 0;
	for(i=1; i<argc;i++)
	{
		if(argv[i][0] != '-')
			break;
		for(int k=1; argv[i][k]!='\0';k++)
		{
			if(argv[i][k] =='c') flags[0] = 'c';
			else if(argv[i][k] =='d') flags[1] = 'd';
			else if(argv[i][k] =='s') flags[2] = 's';
			else
			{
				printf("%s: invalid option '%s'\n",argv[0], argv[i]);
				exit(1);
			}
		}
	}
	int set1_index = i;
	set1 = argv[set1_index];
	set1 = decodeSets(set1);

	int search_from_index = set1_index;

	if(flags[0] == 'c')
		set1 = complementSet(set1);
	if(flags[1] == '\0' && flags[2] == '\0')
	{
		if(set1_index+1 >= argc)
		{
			printf("%s: missing operand after '%s'\n",argv[0],argv[set1_index]);
			return 1;
		}
		set2 = argv[set1_index+1];
		if(flags[0] == 'c')
		{
			int n=0;
		//	char c = 'F';
			char c = set2[strlen(set2)-1];
			//set2=c;			
			set2[1] = '\0';
			set2[0] = c;
		}
		search_from_index = set1_index+1;
	}

	set2 = decodeSets(set2);

	if((search_from_index+1 == argc-1) || (search_from_index+3 == argc-1))
	{
		printf("%s: invalid parameter or option '%s' after set1/set2\n",argv[0], argv[argc-1]);
		exit(1);
	}

	for(i=search_from_index;i<argc; i++)
		if(strcmp(argv[i],"-I")==0 )
			break;
	if((i+1) == argc){
		printf("%s: missing operand after %s\n",argv[0],argv[i]);
		exit(1);
	}
	if(i != argc)
		input_file_index = i+1;

	for(i=search_from_index;i<argc; i++)
		if(strcmp(argv[i],"-O")==0 )
			break;
	if((i+1) == argc){
		printf("%s: missing operand after %s\n",argv[0],argv[i]);
		exit(1);
	}
	if(i != argc)
		output_file_index = i+1;

	if(flags[1] == '\0' && flags[1] == '\0')		// case for transfer and -c
	{
		if((input_file_index==-1) && (output_file_index==-1))
			transferCharachters(set1,set2, NULL,NULL);
		else if(!(input_file_index==-1) && (output_file_index==-1))
			transferCharachters(set1,set2, argv[input_file_index],NULL);
		else if((input_file_index==-1) && !(output_file_index==-1))
			transferCharachters(set1, set2, NULL,argv[output_file_index]);
		else if(!(input_file_index==-1) && !(output_file_index==-1))
			transferCharachters(set1,set2,argv[input_file_index],argv[output_file_index]);
	}
	else	// cases for -s and -d
	{
		if((input_file_index==-1) && (output_file_index==-1))
			readAndPrint(set1, NULL,NULL, flags,3);
		else if(!(input_file_index==-1) && (output_file_index==-1))
			readAndPrint(set1, argv[input_file_index],NULL, flags,3);
		else if((input_file_index==-1) && !(output_file_index==-1))
			readAndPrint(set1, NULL,argv[output_file_index], flags,3);
		else if(!(input_file_index==-1) && !(output_file_index==-1))
			readAndPrint(set1, argv[input_file_index],argv[output_file_index],flags,3);
	}
	printf("\n");
	return 0;
}

void transferCharachters(char *s1, char *s2, char *input_file, char *output_file)
{
	char *input,*output;
	ssize_t ret_in;
	int input_fd=0, output_fd=0, i;
	if(input_file != NULL)	//opens input file to read
	{
 		if((input_fd = open (input_file, O_RDONLY))<0)
 		{
 			printf("./a.out: %s",strerror(errno));
 			exit(1);
 		}
	}
	if(output_file != NULL)	//generates o/p file if doesn't exist
	{
 		if((output_fd = open (output_file, O_RDWR,S_IRWXU))<=0)
 		{
 			int success;
    		success = pathCreation(output_file, S_IRWXU);
    		if(success ==0)
				output_fd = creat(output_file, S_IRWXU);
    		else
    		{
    			printf("%s\n",strerror(errno));
    			exit(1);
    		}
 		}
	}
	if(input_file == NULL)	//both cases when input file is not given
	{
		int n=0;
		input = (char *)malloc(sizeof(char));
		input = readInput(input,&n);
		//printf("%s --- %d\n", input, n);
		output = (char *)malloc(n*sizeof(char));
		output = transfer(s1,s2,input);
		if(output_file == NULL)	//output file not given
			printf("%s",output);
		else
		{				//output file given
			for(i=0; output[i]!='\0';i++);	//gets strlen of output		
			write(output_fd,output,i);
		}
	}
	if(input_file != NULL)	//both cases when input file is given
	{
		int cnt=1;
		input = (char *)malloc(BUF_SIZE*sizeof(char));
		while((ret_in = read(input_fd, input, BUF_SIZE)) > 0)
		{
			//printf()
			output = (char *)malloc(ret_in*sizeof(char));
			output = transfer(s1,s2,input);
			//printf("%d -- %s\n",cnt++,output);
			for(i=0; output[i]!='\0';i++);	
			if(output_file == NULL)	//output file is not given
				write(1,output,i);	
			else	//output file is given
				write(output_fd,output,i);
		}
	}
}
void readAndPrint(char *s1, char *input_file, char *output_file, char flags[], int leng)
{
	char *input,*output;
	ssize_t ret_in;
	int input_fd, output_fd, i;

	if(input_file != NULL)	//opens input file to read
	{
 		if((input_fd = open (input_file, O_RDONLY))<0)
 		{
 			printf("./a.out: %s",strerror(errno));
 			exit(1);
 		}
	}
	if(output_file != NULL)	//generates o/p file if doesn't exist
	{
 		if((output_fd = open (output_file, O_RDWR,S_IRWXU))<=0)
 		{
 			int success;
    			success = pathCreation(output_file, S_IRWXU);
    			if(success ==0)
				output_fd = creat(output_file, S_IRWXU);
    			else
    			{
    				printf("%s\n",strerror(errno));
    				exit(1);
    			}
 		}
	}
	if(input_file == NULL)	//both cases when input file is not given
	{
		int n=0;
		input = (char *)malloc(sizeof(char));
		input = readInput(input,&n);
		//printf("%s --- %d\n", input, n);
		output = (char *)malloc(n*sizeof(char)+1);
		if(flags[1] == '\0' && flags[2] == 's')
			output = squeezeCharacters(s1,input);
		if(flags[1] == 'd' && flags[2] == '\0')
			output = deleteCharacters(s1,input);
		if(output_file == NULL)	//output file not given
			printf("%s",output);
		else
		{				//output file given
			for(i=0; output[i]!='\0';i++);	//gets strlen of output		
			write(output_fd,output,i);
		}
	}
	if(input_file != NULL)	//both cases when input file is given
	{

		input = (char *)malloc(BUF_SIZE*sizeof(char));
		while((ret_in = read(input_fd, input, BUF_SIZE)) > 0)
		{
			output = (char *)malloc(ret_in*sizeof(char));
			if(flags[1] == '\0' && flags[2] == 's')
				output = squeezeCharacters(s1,input);
			if(flags[1] == 'd' && flags[2] == '\0')
				output = deleteCharacters(s1,input);
			for(i=0;output[i]!='\0';i++);
		//	printf("%s",output);
			if(output_file == NULL)	//output file is not given
				//write(1,output,i);
				printf("%s\n",output);	
			else	//output file is given
				write(output_fd,output,i);
		}
	}
//	free(input);
//	free(output);
}
char *complementSet(char *s1)
{
	char *temp;
	temp = (char *)malloc(127);
	int i,j=0;
	for(i=0;i<127; i++)		//resetting ascii map array to '\0'
		ascii_map[i] = '\0';
	for(i=0;s1[i]!='\0';i++)
		ascii_map[(int)s1[i]] = 'Y';
	for(i=32; i<127; i++)
	{
		if(ascii_map[i]!='Y')
			temp[j++] = ascii_values[i];
	}
	temp[j] = '\0';
	return temp;

}
char *decodeSets(char *s1)
{
	char *temp;
/*	if(strcmp(s1,"[:space:]") == 0)
	{
		s1="\\t\\n ";
		return s1;
	}
*/	if(strcmp(s1, "[:punct:]") == 0)
	{
		int i,k=0;
		temp = (char *)malloc(43);	//42 punctuation character and last one for '\0'
		for(i=33; i<127; i++)
		{
			temp[k] = ascii_values[i];
			if(i == 64)		//skipping non punctuation characters
				i=90;
			if(i == 96)
				i=122;
			if(i == 47)
				i = 57;
			k++;
		}
		temp[k] = '\0';
		return temp;
	}
	if(strcmp(s1,"[:digit:]") == 0)
	{
		int i;
		s1 = '\0';
			
		temp = (char *)malloc(10);
		temp = "0123456789";
		
		return temp;
	}
	else if(strcmp(s1,"[:lower:]") == 0)
	{
		int i;
		s1='\0';
		
		temp = (char *)malloc(26);
		for(i=0; i<26; i++)
			temp[i] = ascii_values[i+97];
		temp[i]='\0';
		
		return temp;
	}
	else if(strcmp(s1,"[:upper:]") == 0)
	{
		int i;
		s1='\0';
			
		temp = (char *)malloc(26);
		for(i=0; i<26; i++)
			temp[i] = ascii_values[i+65];
		temp[i]='\0';
		
		return temp;		
	}
	else if(strcmp(s1,"[:alnum:]") == 0)
	{
		temp = (char *)malloc(127);
		temp = strcpy(temp,decodeSets("[:lower:]"));
		temp = strcat(temp,decodeSets("[:upper:]"));
		temp = strcat(temp,decodeSets("[:digit:]"));

		return temp;
	}
	else
	{
		int len = strlen(s1);
		if(len%3 !=0)
			return s1;
		int is_hiphen = 0;
		char *temp1;
		temp = (char *)malloc(127);
		temp[0] = '\0';
		for(int i=0;i<len;i++)
		{
			if(i%3 == 1)
			{
				if(s1[i]!='-')
					return s1;
				if(((int)s1[i-1])> ((int)s1[i+1]))
				{
					printf("./a.out: range-endpoints of '%c-%c' are in reverse collating sequence order\n",s1[i-1],s1[i+1]);
					exit(1);
				}
				temp = strcat(temp, getrange(s1[i-1], s1[i+1]));
			}
		}	
		return temp;
	}
	return s1;
}

char *getrange(char a, char b)
{
	char *temp;
	int k,cnt=0;
	int i = (int)a;
	int j = (int)b;
	temp = (char *)malloc((j-i+1+1)*sizeof(char));
	for(k=i; k<=j; k++)
		temp[cnt++]=ascii_values[k];
	temp[cnt]='\0';
	return temp;
}

char *squeezeCharacters(char *s1, char *input)
{
	char *temp;
	temp = (char *)malloc(strlen(input)*sizeof(char));
	int i,asc_num,k=0;
	for(i=0;i<127; i++)		//resetting ascii map array to '\0'
		ascii_map[i] = '\0';
	for(i=0;s1[i] != '\0'; i++)
		ascii_map[(int)s1[i]] = 'Y';
	for(i=0; input[i] != '\0'; i++)
	{
		asc_num = (int)input[i];
		if(i == 0){
			temp[k] = input[i];
			k++;
			continue;
		}
		if(ascii_map[asc_num] == 'Y'){
			if(input[i] != input[i-1])
				temp[k] = input[i];
			else
				continue;
		}
		else
			temp[k] = input[i];
		k++;
	}
	temp[k] = '\0';
	return temp;
}
char *deleteCharacters(char *s1, char *input)
{
	int i,asc_num,k=0;
	char *temp;
	temp = (char *)malloc(sizeof(input));
	for(i=0;i<127; i++)		//resetting ascii map array to '\0'
		ascii_map[i] = '\0';
	for(i=0;s1[i] != '\0'; i++)
		ascii_map[(int)s1[i]] = 'Y';	//maping ascii numbers of chars of set1 to 'Y' to delete only those elements
	for(i=0; input[i] != '\0'; i++){
		asc_num = (int)input[i];
		if(ascii_map[asc_num] == '\0')
		{
			temp[k++] = input[i];
		}
	}
	temp[k] = '\0';
//	printf("%s-- in delete function\n",temp);
	return temp;
}
char *transfer(char *s1, char *s2, char *input)
{
	int i, asc_num;
	makeEqualCharacters(s1,s2);

	char* temp;
	temp = (char *)malloc(sizeof(input));
	
	for(i=0;i<127; i++)		//resetting ascii map array to '\0'
		ascii_map[i] = '\0';
	for(i=0;s1[i] != '\0'; i++)
		ascii_map[(int)s1[i]] = s2[i];	//maping ascii numbers of chars of set1 to corresponding set2 elements

	for(i=0; input[i] != '\0'; i++)
	{
		asc_num = (int)input[i];
		if(ascii_map[asc_num] == '\0')
		{
	//		printf("%c", input[i]);
			temp[i] = input[i];
		}
		else
		{
	//		printf("%c",ascii_map[asc_num]);
			temp[i] = ascii_map[asc_num];
		}
	}
	temp[i]='\0';
	//printf("%s\n",temp);
	return temp;
}

void makeEqualCharacters(char *s1,char *s2)
{
	int i=0, l1, l2;
	l1 = strlen(s1);
	l2 = strlen(s2);
	
	if(l2>l1)	/* handling when set1 has lesser character than set2 */
		s2[l1] = '\0';
	if(l1>l2){
		i=l2;
		while(i<l1){	/* handling when set2 has lesser character than set1 */
			s2[i]=s2[l2-1];
			i++;
		}
		s2[i]='\0';
	}
}

char *readInput(char *inp,int *n)
{
	int i, size;
	char c;
	i=0;
	size=1;
  	while ((c= getchar()) != EOF) {
 	 	inp = (char*)realloc(inp, size * sizeof(char));
 	 	inp[i] = c; 	 	
    	i++;
    	size++;
 	}
 	inp[i] = '\0';
 	*n=i-1;
 	return inp;
}

int pathCreation(char* file_path, mode_t mode) {	
  char* p;
  p=strchr(file_path+1, '/');//strchr returns next occurance of '/' in filepath
  while(p) {
    *p='\0';
    if (mkdir(file_path, mode)==-1) {
    	if (errno!=EEXIST) { 
      		*p='/';
      		return -1; 
  		}
    }
    *p='/';
     p=strchr(p+1, '/');
  }
  return 0;
}
