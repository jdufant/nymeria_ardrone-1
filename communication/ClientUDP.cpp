#include "udp_wrapper.hpp"
#include <stdlib.h>

int main()
{
	char message[10];
	int nb_char;

	// UDP port (7777) is synchronized with server's one
	UDPClient client("127.0.0.1", 7777); 
	
	// first client request
	while(client.send("go", 2) <= 0);

	while (1)
	{
		nb_char = client.recv(message, 10);
		if(message[0] >120)
		{
			message[0] = 128;
		}		
		printf("received : %d-\n", message[0]);
	}
}
