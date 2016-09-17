#include <xinu.h>

/*
  *	Program to take input as hello <string>
  *	output: Hello <string>, Welcome to the world of Xinu!!
  */

shellcmd xsh_hello(int nargs, char *args[])
{
	if(nargs != 2)
	{
		printf("Wrong number of arguments entered...");
	}
	else
	{
		printf("Hello %s, Welcome to the world of Xinu!!",args[1]);
	}
	printf("\n");
	return 0;

}