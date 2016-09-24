#include <xinu.h>
#include <prodcons.h>
#include <ctype.h>

int n;                 //Definition for global variable 'n'

shellcmd xsh_prodcons(int nargs, char *args[])
{
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
	
	//Argument verifications and validations
	if (nargs == 2 && strncmp( args[1] , "--help", 7) == 0) 
	{
		printf("Usage: %s\n\n", args[0]);
		printf("Description:\n");
		printf("\tRuns the Producer & Consumer for n count\n");
		printf("\tn: int, Optional integer param to this command\n");
		printf("\t--help\tdisplay this help and exit\n");
		return 0;
	}

	if ( isNum(args[1]) == 1 )
	{
		printf("usage: prodcons [n] \n\tn: positive integer\n\t   defaults to 2000\n");
		return;
	}
	

  	int count = 2000;             //local varible to hold count
	if  ( nargs == 2 )
		count = atoi(args[1]);
   
	resume( create(producer, 1024, 20, "producer", 1, count));
	resume( create(consumer, 1024, 20, "consumer", 1, count));
  	return (0);
}