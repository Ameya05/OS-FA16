#include <xinu.h>
#include <stdlib.h>

int netserver()
{
	char	msg[] = "Message from Xinu !!!"; 
	char	inbuf[1500];
	int32	slot;			
	int32	length;
	uint16	echoport= 8888;	
	uint16	locport	= 52743;
	int32	retries	= 3;	
	int32	delay	= 2000;	
	uint32 remoteIp; 
	int val = -1;
	int	i;				
	int	retval;	


	dot2ip("192.168.1.100", &remoteIp);

	// Register the UDP port in the udp table
	slot = udp_register(remoteIp, echoport, locport);
	
	if (slot == SYSERR) 
	{
		return 1;
	}

	length = strnlen(msg, 1200);

	for (i=0; i<retries; i++) 
	{
		retval = udp_send(slot, msg, length);
		if (retval == SYSERR) 
		{
			return 1;
		}

		retval = udp_recv(slot, inbuf, sizeof(inbuf), delay);

		if (retval == TIMEOUT) 
		{	
			continue;
		} 
		else if (retval == SYSERR) 
		{
			udp_release(slot);
			return 1;
		}
		break;
	}

	udp_release(slot);
	if (retval == TIMEOUT) {	
		return 1;
	}
	
	if (retval != length) {	
		return 1;
	}
	
	printf("Data Received : %s \n",inbuf);
	val = atoi(inbuf);
	
	return val;
}