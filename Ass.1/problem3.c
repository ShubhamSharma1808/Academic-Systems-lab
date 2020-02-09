#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<stdlib.h>
#include <time.h>

int main(int argc, char **argv)
{
	int nop,grids;
	char *filename;
	FILE *fptr;

	nop=atoi(argv[2]);			//No. of Players 
	grids=atoi(argv[1]);		//Grids 
	filename=(char*)argv[3];	//Name of the file
	fptr=fopen(filename,"r");
	if(fptr==NULL)
	{ 
        printf("Cannot open file \n"); 
        exit(0); 
    }	

    char SL;
    int start,end;
    int S[grids][2],L[grids][2];
    int count_S=0,count_L=0;
   	
   	/*
   	The below while loop stores the content of the file 
	in the two arrays S and L,S is for snakes and 
	L is for Ladders. In which even indices stores
	start and odd indices stores destination.
	after doing this the file is closed.
	*/

    while(fscanf(fptr,"%c",&SL)!= EOF){
    	if(SL=='S'){
    		fscanf(fptr,"%d",&start);
    		fscanf(fptr,"%d",&end);
    		if(start<end){
    			printf("Invalid entry of snakes in the file\n");
    			fclose(fptr);	
    			return 0;
    		}
    		else{
    			S[count_S][0]=start;
    			S[count_S][1]=end;
    			count_S++;
    		}
    	}
    	else if (SL=='L')
    	{
    		fscanf(fptr,"%d",&start);
    		fscanf(fptr,"%d",&end);    		
    		if(start>end){
    			printf("Invalid entry of Ladders in the file\n");
    			fclose(fptr);
    			return 0;
    		}
    		else{
    			L[count_L][0]=start;
    			L[count_L][1]=end;
    			count_L++;
    		}	
    	}    	
    }
    fclose(fptr);

    /*
    below two for loops are printing the contennts
    of the file whihc is stored in S and L array.
    To check if the contents read are correct or not.
    */ 

    for (int i = 0; i < count_L; ++i)
    {
    	printf("L %d %d\n",L[i][0],L[i][1] );
    }
    for (int i = 0; i < count_S; ++i)
    {
    	printf("S %d %d\n",S[i][0],S[i][1] );
    }
	
	/*
	Now comes the main part after all the inputs
	are taken care of.
	*/

    int count1,pid,val;    
    int pids[nop];		//for storing PID's of child.
	int dicevalue;  	//Name suggests meaning, dicevalue.
	int p2c_fd[2*nop];	//file descriptor for sending from parent to child
	int c2p_fd[2*nop];	//file descriptor for sending from child to parent
	time_t t;

	/*
	Below for loop initiates the pipe.
	*/

	for (int i = 0; i < nop; ++i)
	{
		pipe(&p2c_fd[2*i]);
		pipe(&c2p_fd[2*i]);
	}
	
	/*
	Below for loop creates the child processes.
	*/

	for (int i = 0; i < nop; ++i)
	{
		pid=fork();
		if(pid==0)
		{
			
			/*
			ab child process is loop se  bahar ja hi ni paegawo bss yha
			while loop me fas jaegi	and pipe ke through parent ko dicevalue
			bhejegi	then wait kregi pipe se read krne ka, jab tk pipe empty
			hoga,read 0 return krega and !(read) condn se while true hojaega and
			wo loop me run hote rhega.
			*/
						
			srand((int)time(&t) % (getpid()*245));
			while(1){
				close(c2p_fd[2*i]);
				dicevalue=(rand() %6) +1;					
				write(c2p_fd[2*i + 1],&dicevalue,sizeof(dicevalue) ); 
				while(!read(p2c_fd[2*i], &val, sizeof(int)));
			}
		}
		else
		{
			pids[i]=pid;
		}
	}
	
	/*
	Now this below code is executed by parent only 
	as the child processes are stuck in the above loop
	*/

	int new_posn,i,ping=1;
	int posn[nop];					//will store the position of each process.
	
	/*
	below for loop for initiating the process
	*/

	for (int j = 0; j < nop; ++j)
		posn[j]=0;

	/*
	below two lines to choose random player for first time.
	*/

	srand((int)time(&t) % (getpid()*245));
	i=rand()%nop;		
	printf("Initialised with process %d\n",i+1 );


	while(1){
		close(c2p_fd[2*i + 1]); 
		read(c2p_fd[2*i], &val, sizeof(int));		
		printf(" process %d: %d\n",i+1,val);
		new_posn=posn[i]+val;
		
		/*
		below two for loop checks whether snake or 
		ladder is present at the new position or not.
		*/ 

		for (int j = 0; j < count_L; ++j)
		{
			if(L[j][0]==new_posn){
				new_posn=L[j][1];
				break;
			}
		}
		for (int j = 0; j < count_S; ++j)
		{
			if(S[j][0]==new_posn){
				new_posn=S[j][1];
				break;
			}	
		}
		posn[i]=new_posn;	

		/*
		Below for loop prints the position of each process.
		*/

		for (int j = 0; j < nop; ++j)
			printf("process %d postition %d\n",j+1,posn[j]);
		
		/*
		Below if condition checks if the process exceeds
		the grid value,if yes then first the end of the pipes 
		is closed and then all the child process are killed 
		and then main program i.e parent process ends.
		*/

		if(posn[i]>grids)
		{
			printf("\n ~~~winner is process No. %d~~~\n",i+1);
			for (int j = 0; j < nop; ++j)
			{
				close(c2p_fd[j]);
				close(p2c_fd[j]);
			}
			for (int j = 0; j < nop; ++j)
			{			
				kill(pids[j], SIGKILL);
			}			
			return 0; 
		}

		/*
		this part is to ping the child that he can continue. 
		*/

		close(p2c_fd[2*i]);
		write(p2c_fd[2*i + 1],&ping,sizeof(int) ); 
		if(val!=6)				//it ensures that if 6 occurs then the same process gets the chance again.
			i=(i+1)%nop;			
	}
}

