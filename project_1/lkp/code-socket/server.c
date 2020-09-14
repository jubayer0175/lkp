#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 5984
#define BUFF_SIZE 4096

int main(int argc, const char *argv[])
{
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFF_SIZE] = {0};
	char *hello = "Hello from server";

	/* [S1: point 1]
	 * Explain following in here.
     * This is essentially same as C1 in client.c execpt follwoings:
     * if returned file descriptor for this socket is NULL means 
     * the socket is not created and report "socket failed" using system error
     * perror() to stderr
     *
	 */
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	/* [S2: point 1]
	 * getsockopt() manipulates options for the socket referred to by the file descriptor
     * server_fd. argument SOL_SOCKET indicates we want to modify socket at API level.
     * SO_REUSEADDR flag allows server to bind to an address that is in TIME_WAIT state. It does
     * not allow more than one server to bind to the same address. SO_REUSEPOST allows multiple 
     * process to bind to the same address. &opt is the adress of the option value and sizeof(opt)
     * provides the length.
	 */
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		       &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}

	/* [S3: point 1]
	 * This question is similar to C2 with a difference server address filed of the address 
     * structure. s_adr filed is assigned to INADDR_ANY indiactes the server is requesting that
     * the socket be bound to all network interfaces on the host. By leaving address 
     * unspecified the server can recive all TCP connection requestis irrespective of the
     * network interface.
     * 
     * 
	 */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

	/* [S4: point 1]
	 * This block binds the server to its own address so that clients can 
     * talk to it using connect() on the client side (See C4 in client.c)
     * Failure to bind wouls result in exiting the server file execution with system error
     * printed as 'bind failure'
     *
	 */
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	/* [S5: point 1]
	 * listen for connection on the socket defined by server_fd file descriptor.
     * The second argument indiactes that who long the pending queue may grow(3 in this case).  
	 */
	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	/* [S6: point 1]
	 * accept() extracts new socket from the pending list of connection requests keeping
     * the original socket defined by server_fd unchanged. 
	 */
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
				 (socklen_t*)&addrlen)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	/* [S7: point 1]
	 * Wait for user to press a key before initiating message transaction. 
	 */
	printf("Press any key to continue...\n");
	getchar();

	/* [S8: point 1]
	 * Explain following in here.
     * Read data sent to the socket and retrive it from new_socket
	 */
	read( new_socket , buffer, 1024);
	printf("Message from a client: %s\n",buffer );

	/* [S9: point 1]
	 * Send message to the new_socket.
	 */
	send(new_socket , hello , strlen(hello) , 0 );
	printf("Hello message sent\n");
	return 0;
}
