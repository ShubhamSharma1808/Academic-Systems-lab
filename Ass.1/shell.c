#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<unistd.h>  
#include<fcntl.h>
#include<readline/readline.h>
#include<readline/history.h>

#define MAXCHAR 1000
#define TOK_BUFSIZE 40
#define TOK_DELIM "\t\r\n\a " " "

int fun_cd(char **args);
int fun_help(char **args);
int fun_exit(char **args);
int fun_cat(char **args);
int fun_ls(char **args);
int fun_sort(char **args);
int fun_grep(char **args);
int fun_mkdir(char **args);
int fun_cp(char **args);
char s[100];
int x;

int a_read_line(char *str)					//for taking input and taking it to history           
{
char *buf;
buf=readline("");
if(strlen(buf)!=0)
{
    add_history(buf);
    strcpy(str,buf);
    return 0;
}
else
{
    return 1;
}

}

int fun_sort(char **args)
{
struct dirent *de;  								// pointer to entry of directory 
  char che[40];
    										// it returns a pointer of dir.  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  								//if couldn't open directory it return NULL 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    if(args[0]!=NULL)
    {
      	while ((de = readdir(dr)) != NULL) 
        printf("%s\t", de->d_name); 
    }
    closedir(dr);     
    return 0;
}

int fun_grep(char **args)                                                        //implementation of grep function
{
    char fn[10], pat[10], temp[200];
    FILE *fp;
    
    *fn=&args[2];
    *pat=&args[1];
    fp = fopen(fn, "r");
    while (!feof(fp))
    {
        fgets(temp, sizeof(fp), fp);
        if (strcmp(temp, pat))
            printf("%s", temp);
    }
    fclose(fp);
    return 1;

}
int fun_mkdir(char **args)
{
struct dirent *de;  								// implementation of mkdir 
  char che[40];
    										  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  								
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    if(args[0]!=NULL)
    {
	
      	while ((de = readdir(dr)) != NULL) 
        printf("%s\t", de->d_name); 
    }
    closedir(dr);     
    return 0;

}
int fun_cp(char **args)
{
struct dirent *de;  								 
  char che[40];
    										
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  								
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    if(args[0]!=NULL)
    {
      	while ((de = readdir(dr)) != NULL) 
        printf("%s\t", de->d_name); 
    }
    closedir(dr);     
    return 0;
}

int fun_ls(char **args)
{
if(args[1]==NULL)
{
	struct dirent **nl;
	int n;
	n= scandir(".",&nl,NULL,alphasort);    			//scandir function call to scan directory i.e. 

	if(n<0)							//files present in the directory
	{
	perror("scandir");
	}
	else
	{
	while(n--)
	{
	if((strcmp(nl[n]->d_name,".")!=0)&&(strcmp(nl[n]->d_name,"..")!=0))
	printf("%s\t",nl[n]->d_name);
	free(nl[n]);
	}
	
	free(nl);
	}
}else
	{

  struct dirent *de;  								// Pointer for directory entry 
  char che[40];
    								// opendir() returns a pointer of DIR type.  
    DIR *dr = opendir("."); 
  
    if (dr == NULL)  								// opendir returns NULL if couldn't open directory 
    { 
        printf("Could not open current directory" ); 
        return 0; 
    } 
    if(args[0]!=NULL)
    {
      	while ((de = readdir(dr)) != NULL) 
        printf("%s\t", de->d_name); 
    }
    closedir(dr);     
    return 0;
	}
}

int fun_cd(char **args)
{
    
    
	chdir(args[1]);  
    	//printf("%s\n", getcwd(s, 100)); 
    
    return 1;
}

char *builtin_str[] = {
  "cd",
  "help",
  "exit",
  "cat",
  "ls",
  "sort",
  "grep",
  "mkdir",
  "cp"
};

int fun_help(char **args)
{
  int i;  
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < fun_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int fun_exit(char **args)
{
	x=1;
	printf("%d",x);
   	return 0;
}

int fun_cat(char **args)
{
  FILE *fp;
  int n=1;
  char str[MAXCHAR];
  char* filename = "c:\\temp\\test.txt";
	
  if(args[2]!=NULL)
  {
  fp = fopen(args[2], "r");
  if (fp == NULL){
        printf("Could not open file %s",args[2]);
        return 1;
  }    
  while (fgets(str, MAXCHAR, fp) != NULL)
  {
	printf("%d ",n);
	printf("%s", str);
	n++;
  }
  fclose(fp);
  }else
{
	fp = fopen(args[1], "r");
	if (fp == NULL){
        printf("Could not open file %s",args[1]);
        return 1;
    }
	while (fgets(str, MAXCHAR, fp) != NULL)
	printf("%s", str);
	fclose(fp);
}
    return 0;

}

int (*builtin_func[]) (char **) = {
  &fun_cd,
  &fun_help,
  &fun_exit,
  &fun_cat,
  &fun_ls,
  &fun_sort,
  &fun_grep,
  &fun_mkdir,
  &fun_cp
};

int fun_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int fun_launch(char **args)
{
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
  										
    if (execvp(args[0], args) == -1) {
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) {
    
    perror("lsh");
  } else {
    
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}


int fun_execute(char **args)
{
  int i;
  
  if (args[0] == NULL) {
    printf("An empty command was entered.");
    return 1;
  }

  for (i = 0; i <5; i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

    return fun_launch(args);
}


int main()
{
	int buff_size= TOK_BUFSIZE;
	
	
	x=50;
	while(x)
	{
		char line[100];
		char **tokens=malloc(buff_size *sizeof(char*));
		
		printf("\033[1;33m");
		printf("\naayush-shubham@SHELL:");		
		
		printf("\033[1;31m");
		printf("%s", getcwd(s, 100)); 
		printf("\033[0m");
		printf("$");		
		size_t size = sizeof(char);
		a_read_line(line);
		

		char *token;
		
		int i=0;
		token=strtok(line,TOK_DELIM);
		
		while(token!=NULL)
		{
			tokens[i]=token;
			token=strtok(NULL,TOK_DELIM);
			i++;		
		}
		
		for(i=0;i<9;i++)
		{
			if(strcmp(tokens[0],builtin_str[i])==0)
			goto label;
		}
	label:  if(i<9)
		fun_execute(tokens);
		else
		printf("command not found");		
		free(tokens);
		x--;
	};

}
