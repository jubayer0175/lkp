#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include<pthread.h>

#include <sys/types.h>
#include <linux/userfaultfd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <poll.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <poll.h>



#define BUFF_SIZE 4096

#define errExit(msg) do {perror(msg); exit (EXIT_FAILURE);\
    } while(0)



static char hello[6] = "12345"; /*Say this is my memroy request code*/

/* Grobal variables */

static volatile int int1 = 0;
static volatile int int2 = 0; /*Kind of way to track the which instance
is calling what*/
static volatile int c_connect = 0; /*connection tracking flags*/
static volatile int s_connect = 0; /*connection tracking flags*/

static long len;
static long page_size;
static volatile int done_mapping = 0; /*Map control flag*/
static char *addr = NULL;
char *tok;

extern int errno;

static void *
client(void *argv)
{
	int sock = 0;
    struct sockaddr_in serv_addr;
    /*get the port for the client*/
    long *ptr = (long*) argv;
    long client_PORT = *ptr;
	char buffer[BUFF_SIZE] = {0};
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		errExit("\n Socket creation error \n");
	}
	memset(&serv_addr, '0', sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(client_PORT);
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
		errExit("\nInvalid address/ Address not supported \n");
	}
/*Constant polling*/
    int cnt = 110000; /*Some arbitary large number*/
	while(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cnt -= 1;
        if (cnt == 0) {
            int1 =1; /*This one makes sure that only one instance execute
            memory mapping the in the main*/
            printf("Connection not found\n");
           cnt = 110000;
        }
	}
 
    printf("Info_c: Connected to the server\n");
   
    /*Lets us do some memory stuff here*/

    
  
    c_connect = 1;

    if (int1 == 0 && addr == NULL){ /*The memory is not created yet by the instance 1*/  /*Just in case the */
    /*Ask for memory mampping information*/
    printf("Requesting memory information\n");    
    int a ;
    a= send(sock , hello , strlen(hello) , 0 );
    

    /*read back the memory information*/
        if(read(sock ,buffer, 1024) == -1) { 
            printf("Info: memory request read failure\n");
    }
   //  printf("This is I have received %s\n", buffer);

     /*Process the received stuff*/
    unsigned long temp;
    tok = strtok(buffer, "&"); /*this should provide our address*/
    sscanf(tok ,"%ld",&temp );
    printf("Info_c: recived address %ld\n", temp );
	// Create memory with this;
    char* temp_address = (char *)malloc(sizeof(temp_address)); 
    temp_address =  (char*) temp;
            int i = 0;
            while(tok != NULL) {
                i++;
                tok = strtok(NULL, "&");
                if (i == 1) 
          // printf("length: %s\n", tok);
                sscanf(tok ,"%ld", &len);
   }

  /*I have everything I needed for the memory creationin this app*/
     addr = mmap(temp_address, len, PROT_READ | PROT_WRITE,
                                    MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
     if (addr == MAP_FAILED)
         errExit("mmap");
    printf("Int-2: %p\n", addr);



	
}

}

static void *
server(void *argv){


    /*Get the port for server*/
    long *ptr = (long *)argv;
    long server_PORT = *ptr;
   /* printf("Does the port look correct %ld\n", *ptr); */
	int server_fd, new_socket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	char buffer[BUFF_SIZE] = {0};
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
		       &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( server_PORT );

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	if (listen(server_fd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                    (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                        exit(EXIT_FAILURE);                       
    }
    /*Server has a connection*/
    printf("Info_s:Connection establised\n");
    s_connect = 1;
    int a;
    a = read( new_socket, buffer, BUFF_SIZE); 
    if(strcmp(buffer, hello ) == 0 ) { /** did you recv a memory*/
        printf("\nInfo_s : client just asked for memory information\n");
         static char buff2[BUFF_SIZE] = {0}; /*Holds sending data*/
     if(int1) {
        // printf("map flags: %d\n", done_mapping);
        while(done_mapping == 0);
       // wait
       if(addr != NULL) { /*Not really needed but just incase*/
            sprintf(buff2,"%ld &  %ld",(unsigned long) addr,len);
            a = send(new_socket ,buff2, sizeof(buff2) , 0 ); /*Hope to send the data*/
             }
    }

    }
    int b;
    b = close(server_fd);
    if(b != 0)
        errExit("Failed to close the port \n");
}

int main(int argc, char *argv[])
{

   if( argc != 3){
      errExit("wrong number of argument");
    }

    pthread_t client_thr;
    pthread_t server_thr;

    long c_port;
    long s_port;
    int e;
    long num_pages = 0;



    s_port = atol(argv[1]);
    c_port = atol(argv[2]);
    printf("Client port: %ld\n", c_port);
    printf("Server port: %ld\n", s_port);
    /*Server thread*/
    e = pthread_create(&server_thr, NULL, server, &s_port);
    if ( e != 0) {
        errExit("Failed to create server thread");
    }
    /*Client thread*/
    e = pthread_create(&client_thr, NULL, client, &c_port);
    if ( e != 0) {
        errExit("client thread creation failure");
    }
    
    while(s_connect ==0 || c_connect == 0); /*connection not done yet*/
    /*Means were have a connection and we want to create mm in inst1*/
    
   if(int1 == 1  ){
       printf("Inst-1: How many pages? : ");
       scanf("%ld",&num_pages );
    
      page_size = sysconf(_SC_PAGE_SIZE);
      len = num_pages * page_size;
      addr = mmap(NULL, len, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
       if (addr == MAP_FAILED)
           errExit("Failed to mapp");

       done_mapping = 1;
       printf("Inst-1: Mapped in %p\n", addr);

   }

    pthread_join(server_thr, NULL);
    pthread_join(client_thr, NULL);

exit(EXIT_SUCCESS);
}

