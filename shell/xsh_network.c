 #include <newnet.h>

shellcmd xsh_network(int nargs, char *args[])
{	
	resume( create(netserver, 1024, 20, "netserver", 0));
	return 0;
}