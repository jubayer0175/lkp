#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 5984
#define BUFF_SIZE 4096

int main(int argc, const char *argv[])
{
	int sock = 0;
	struct sockaddr_in serv_addr;
	char *hello = "Hello from client";
	char buffer[BUFF_SIZE] = {0};

	/* [C1: point 1]
	 * Explain following in here.
     * ....(sock = socket(AF_INET, SOCK_STREAM, 0)..
     *Socket() subroutine takes 0 to define deafult connection type (TCP) and as TCP is
     * connecttion based protocol SOCK_STREM is passed to indicated that. For UDP it would 
     * have been SOCK_DGRAM. SOCK_STREAM allows continues reading of the data stream.
     *
     * Failure to create a socket would return -1. The printf() here indiactes that.
     * AF_INET indiactes the adress domain of the socket. Internet domain for any two hosts 
     * belongs to AF_INET family.
	 */
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	/* [C2: point 1]
	 * Explain following in here.
     * This block sets up the address stucture.
     * memset() fills the memory pointer by serv_addr with zero.
     * serv_addr.sin_family = AF_INET; defines the family of server adress
     * serv_addr.sin_port = htons(PORT); defines the port number. instead 
     * copying the port number to "unsigned short sin_port" we need to convert port into network
     * byte order using htons() subroutine. 
     *
	 */
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	/* [C3: point 1]
	 * Explain following in here.
     *check the validity the validity of IPv4 adress 127.0.0.1 after converting  into binray form form text 
	 */
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

	/* [C4: point 1]
	 * Explain following in here.
     * Check the validity of socket afeter initiating the connection. -1 indiactes connection failure.
	 */
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("\nConnection Failed \n");
		return -1;
	}


	/* [C5: point 1]
	 * Explain following in here.
     *waits for input from the user
     *
	 */
	printf("Press any key to continue...\n");
	getchar();

	/* [C6: point 1]
	 * Explain following in here.
     * sends message (hello) to another socket. sock argument is the file descriptor of the sending
     * socket while message and message length pass the message and its length. The flas argument 
     * is ORed version of different flags that alow more control on differebt aspects of sending such as
     * informating that the sending socket has more data to send (MSG_DONTWAIT flag).
     *
	 */
	send(sock , hello , strlen(hello) , 0 );
	printf("Hello message sent\n");

	/* [C7: point 1]
	 * Explain following in here.
     * Listens to another socket and reads the data from that socket.
	 */
	read( sock , buffer, 1024);
	printf("Message from a server: %s\n",buffer );
	return 0;
}
