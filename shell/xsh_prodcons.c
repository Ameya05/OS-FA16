#include <xinu.h>
#include <prodcons.h>
#include <ctype.h>

int n;                 //Definition for global variable 'n'

shellcmd xsh_prodcons(int nargs, char *args[])
{
	//function to check if the argument is numeric
	//Returns 1: False, 0: True
	int isNum(char num[])
	{
		int x = 0;
		
		for ( ; num[x] != 0; x++)
		{
		        if (!isdigit(num[x]))
				return 1;
		}
		return 0;
	} 
	
	void printUsage()
	{
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRuns the Producer & Consumer for n count\n");
		printf("\tn: int, Optional integer param to this command\n");
		printf("\t--help\tdisplay this help and exit\n");
	}

	//if --help is used, then print out help doc
	if (nargs == 2 && strncmp( args[1] , "--help", 7) == 0) 
	{
		printUsage();
		return;
	}
	
	//If More than 1 argument is supplied or argument is not numeric, then print usage
	if ( (nargs > 2) || ( ( nargs == 2) && ( isNum(args[1]) == 1 ) ) )
	{
		printUsage();
		return;
	}
	

  	int count = 2000;             //local varible to hold count
	
	if  ( nargs == 2 )
		count = atoi(args[1]);
   
	resume( create(producer, 1024, 20, "producer", 1, count));
	resume( create(consumer, 1024, 20, "consumer", 1, count));
  	return (0);
}