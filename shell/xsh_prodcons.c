#include <xinu.h>
#include <prodcons.h>
#include <ctype.h>
#include <future.h>

int32 n;                 //Definition for global variable 'n'

sid32 produced, consumed;

shellcmd xsh_prodcons(int32 nargs, char *args[])
{
	//function to check if the argument is numeric
	//Returns 1: False, 0: True
	int32 isNum(char num[])
	{
		int32 x = 0;
		
		for ( ; num[x] != 0; x++)
		{
		        if (!isdigit(num[x]))
				return 1;
		}
		return 0;
	} 
	
	void printUsage()
	{
		printf("Usage: %s [n] [-f] \n\n", args[0]);
		printf("Description:\n");
		printf("\tRuns the Producer & Consumer for n count\n");
		printf("\t-f\n\t\tRuns the Producer & Consumer using Futures\n");
		printf("\tn: int, Optional integer param to this command\n");
		printf("\t--help\tdisplay this help and exit\n");
	}

	//if --help is used, then print out help doc
	if (nargs == 2 && strncmp( args[1] , "--help", 7) == 0) 
	{
		printUsage();
		return (0);
	}
	
	//If More than 1 argument is supplied or argument is not numeric, then print usage
	if ( (nargs > 2) || ( ( nargs == 2) && ( isNum(args[1]) == 1 ) && strncmp( args[1] , "-f", 3) != 0 ) )
	{
		printUsage();
		return (0);
	}
	
        //If script is invoked with -f, then call the futures implementation
    if( nargs == 2 && strncmp( args[1] , "-f", 3) == 0)
    {
	future *f1, *f2, *f3;
	
	f1 = future_alloc(FUTURE_EXCLUSIVE);
	f2 = future_alloc(FUTURE_EXCLUSIVE);
	f3 = future_alloc(FUTURE_EXCLUSIVE);
	
	resume( create(future_cons, 1024, 20, "fcons1", 1, f1) );
	resume( create(future_prod, 1024, 20, "fprod1", 1, f1) );
	resume( create(future_cons, 1024, 20, "fcons2", 1, f2) );
	resume( create(future_prod, 1024, 20, "fprod2", 1, f2) );
	resume( create(future_cons, 1024, 20, "fcons3", 1, f3) );
	resume( create(future_prod, 1024, 20, "fprod3", 1, f3) );
	return (0);
    }
    else
    {
    	produced = semcreate(0);
	consumed = semcreate(1);

  	int32 count = 2000;             //local varible to hold count
	
	if  ( nargs == 2 )
		count = atoi(args[1]);
   
	resume( create(producer, 1024, 20, "producer", 1, count));
	resume( create(consumer, 1024, 20, "consumer", 1, count));
	return (0);
    }
}