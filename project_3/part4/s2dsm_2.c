/*Author: Jubayer
 * Written for project 3 part 2 (Linux kernel)*/


#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
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
#include <stdio_ext.h>



#define BUFF_SIZE 4096
#define errExit(msg) do {perror(msg); exit (EXIT_FAILURE);\
    } while(0)






/* MSI stuffs*/

enum MSI {M,S,I};
static volatile enum MSI *state ; /*This willl conatain number of MSI states for each
                                    
page localy
*/


static char hello[6] = "12345"; /*Say this is my memroy request code*/
static char invalid[2]="2";
static char read_req[2]="1";
/* Grobal variables */
static volatile int read_for_app_2; 
static volatile int cl=0;
static volatile int int1 = 0;
/* Kind of way to track the which instance
 * is calling what*/
static volatile int c_connect = 0; /*connection tracking flags*/
static volatile int s_connect = 0; /*connection tracking flags*/
static long len;
static long page_size;
static volatile int done_mapping = 0; /*Map control flag*/
static char *addr = NULL;
char *tok;
extern int errno;

static volatile int page_track;


/*Client function*/
static volatile int sock =0;

static char *page = NULL;




static void *
client(void *argv)
{

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
            int1 =1; /* This one makes sure that only one instance execute
            memory mapping the in the main */
            printf("Connection not found\n");
           cnt = 110000;
        }
	}
    printf("Info_c: Connected to the server\n");
    c_connect = 1;  /* Client is connected */
    if (int1 == 0){ /* The memory is not created yet by the instance 1*/  
       /* Ask for memory mampping information*/
        if(addr == NULL) {
    printf("**Requesting memory information from instance (1)**\n");    
    send(sock , hello , strlen(hello) , 0 );
    /* read back the memory information */
        if(read(sock ,buffer, 1024) == -1) { 
            printf("Info_c: memory request read failure\n");
    }
     /*Process the received stuff*/
    unsigned long temp;
    tok = strtok(buffer, "&"); /*this should provide our address*/
    sscanf(tok ,"%ld",&temp );
   // printf("Info_c: recived address %ld\n", temp );
	// Create memory with this;
    char* temp_address = (char *)malloc(sizeof(temp_address)); 
    temp_address =  (char*) temp;
            int i = 0; /*Find the segment I sent from the other instance*/
            while(tok != NULL) {
                i++;
                tok = strtok(NULL, "&");
                if (i == 1) /* the first delimiter is the length info*/
                sscanf(tok ,"%ld", &len); /* Format the string to long*/

   }
            
          printf("length: %ld byte\n", len);
  /*I have everything I needed for the memory creationin this app*/
     addr = mmap(temp_address, len, PROT_READ | PROT_WRITE, \
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
     if (addr == MAP_FAILED)
         errExit("Info_c: mmap failure");
         printf("Int-2: shared memory address:  %p\n", addr);
    cl =1;
           }
    }   

    while(1);
}

static void *
server(void *argv){
    /*Get the port for server*/
    long *ptr = (long *)argv;
    long server_PORT = *ptr;
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
    if(int1 == 1){
            read( new_socket, buffer, BUFF_SIZE); 
            if(strcmp(buffer, hello ) == 0 ) { /** did you recv a memory*/
       // printf("\nInfo_s : client just asked for memory information\n");
            static char buff2[BUFF_SIZE] = {0}; /*Holds sending data*/
     
        // printf("map flags: %d\n", done_mapping);
            while(done_mapping == 0);
       // wait
       if(addr != NULL) { /*Not really needed but just incase*/
            sprintf(buff2,"%ld &  %ld",(unsigned long) addr,len);
            send(new_socket ,buff2, sizeof(buff2) , 0 ); /*Hope to send the data*/
             }
            }

    }

/********Above this is the init protocol***********/
        
         char str[BUFF_SIZE] = {0};
         char str_pages_number[BUFF_SIZE] = {0};
        while(1){
            // wait to see if client send any request
            read(new_socket, str, BUFF_SIZE);
            printf("request from the client 2:%s\n", str);

            read(new_socket, str_pages_number, BUFF_SIZE);
            int number_of_page;
            sscanf(str_pages_number, "%d", &number_of_page);

        //    printf("Request %d\n", page_or_msi);
            if(strcmp(str, invalid) ==0){
                if(madvise(addr + (number_of_page*page_size),page_size,\
                            MADV_DONTNEED ) )
                    errExit("failed to madvise");
                state[number_of_page] = I;
                printf("info_s: invalidation req\n");
            }
            
            else if(strcmp(str, read_req) == 0){

                read_for_app_2 = 1; /*set the flag*/
                strncpy(str, addr + number_of_page*page_size, page_size);
                send(new_socket, str, page_size, 0); //Share with the app2
                state[number_of_page]=S;
                read_for_app_2 = 0; /*unset the flag*/
                printf("info_s: read request\n" );

                            }
            }
}

/********************************************/


static void *
fault_handler_thread(void *arg)
{
	static struct uffd_msg msg;   /* Data read from userfaultfd */
	static int fault_cnt = 0;     /* Number of faults so far handled */
	long uffd;                    /* userfaultfd file descriptor */
	struct uffdio_copy uffdio_copy;
	ssize_t nread;
    char rcv[BUFF_SIZE] = "";
	uffd = (long) arg;
	if (page == NULL) {
		page = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
			    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
		if (page == MAP_FAILED)
			errExit("mmap");
	}

	for (;;) {

		/* See what poll() tells us about the userfaultfd */

		struct pollfd pollfd;
		int nready;

		pollfd.fd = uffd;
		pollfd.events = POLLIN;
		nready = poll(&pollfd, 1, -1);
            if (nready == -1)
                errExit("poll");
/*		printf("\nfault_handler_thread():\n");
		printf("    poll() returns: nready = %d; "
                       "POLLIN = %d; POLLERR = %d\n", nready,
                       (pollfd.revents & POLLIN) != 0,
                       (pollfd.revents & POLLERR) != 0);
		*/
        nread = read(uffd, &msg, sizeof(msg));
		if (nread == 0) {
			printf("EOF on userfaultfd!\n");
			exit(EXIT_FAILURE);
		}

		if (nread == -1)
			errExit("read");
		if (msg.event != UFFD_EVENT_PAGEFAULT) {
			fprintf(stderr, "Unexpected event on userfaultfd\n");
			exit(EXIT_FAILURE);
		}

    
        // page fault can come from when app 2 tries to read a page
        // or app 1 tries to read it
        if (read_for_app_2 == 1) {
            memset(page, '\0', page_size); // fill up the page with null
            printf("check for app_2 read flag \n");
                }
            
        else{

        // call hapens from server 1. (ints 1)
            printf("\n[x] PAGEFAULT\n");
    //    memset(page, 'A' + fault_cnt % 20, page_size);

        char t1[BUFF_SIZE] = "0";
        int err;
        err = send(sock,read_req, strlen(read_req), 0 );
        sprintf(t1, "%d", page_track);
  //      printf("converted t1 , %s\n", t1);
        send(sock, t1, BUFF_SIZE, 0); // This one goes to the server ()2)
        printf("err: %d\n",err );
        
        err = read(sock, rcv, page_size);
        printf("read error: %d,\n", err);
        memcpy(page, rcv, page_size);
        state[page_track] = S;
     }
    
  if(msg.arg.pagefault.flags== (unsigned long long int) 1) {
      
      send(sock,invalid, strlen(invalid), 0);
      char t[BUFF_SIZE] = "0";
      sprintf(t, "%d", page_track);
     send(sock,t,strlen(t),0);
  }   


		fault_cnt++;
		uffdio_copy.src = (unsigned long) page;
		uffdio_copy.dst = (unsigned long) msg.arg.pagefault.address &
			~(page_size - 1);
		uffdio_copy.len = page_size;
		uffdio_copy.mode = 0;
		uffdio_copy.copy = 0;
		if (ioctl(uffd, UFFDIO_COPY, &uffdio_copy) == -1)
			errExit("ioctl-UFFDIO_COPY");
	}
}






int main(int argc, char *argv[])
{

   if( argc != 3){
      printf("Example run: ./s2dm 5001 5000 \t  then run in another terminal \n ./s2dm 5000 5001 \n");
      printf("This is a cross connection one server must talk to the other client port \n");
      errExit("Wrong number of argument\n");
      exit(EXIT_FAILURE);
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
       printf("Enter number of pages,  for example: 2\n");
       printf("Inst-1: How many pages? : ");
       scanf("%ld",&num_pages );
      page_size = sysconf(_SC_PAGE_SIZE);
      len = num_pages * page_size;
      addr = mmap(NULL, len, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
       if (addr == MAP_FAILED)
           errExit("Failed to map");
       done_mapping = 1;
       printf("Inst-1: Mapped in %p\n", addr);
       printf("Inst -1: Length %ld \n  ", len);
       /*create MSI in invalid state*/
    
      }
   else{ // ia am responsible for memory mapping
       while (cl == 0 ); // let the clinet memory map.
        
      page_size = sysconf(_SC_PAGE_SIZE);
       num_pages= len/page_size;
   }
      


    /**mapping and sync done*/

       state = malloc (num_pages * sizeof(char));
       int l = 0;
       for(l = 0; l<num_pages; l++ ){
           state[l] = I; /*Init everything in invalid  state*/
          }

/* uffd stuff  */

	long uffd;          /* userfaultfd file descriptor */
//	char *addr;         /* Start of region handled by userfaultfd */
//	unsigned long len;  /* Length of region handled by userfaultfd */
	pthread_t uffd_thr;      /* ID of thread that handles page faults */
	struct uffdio_api uffdio_api;
	struct uffdio_register uffdio_register;
	int s;


//	page_size = sysconf(_SC_PAGE_SIZE);
//	len = strtoul(argv[1], NULL, 0) * page_size; /**arv * 3, page size is 4KB*/
	uffd = syscall(__NR_userfaultfd, O_CLOEXEC | O_NONBLOCK);
	if (uffd == -1)
		errExit("userfaultfd");
	uffdio_api.api = UFFD_API;
	uffdio_api.features = 0;
	if (ioctl(uffd, UFFDIO_API, &uffdio_api) == -1)
		errExit("ioctl-UFFDIO_API");
	uffdio_register.range.start = (unsigned long) addr;
	uffdio_register.range.len = len;
	uffdio_register.mode = UFFDIO_REGISTER_MODE_MISSING;
	if (ioctl(uffd, UFFDIO_REGISTER, &uffdio_register) == -1)
		errExit("ioctl-UFFDIO_REGISTER");
	s = pthread_create(&uffd_thr, NULL, fault_handler_thread, (void *) uffd);
	if (s != 0) {
		errno = s;
		errExit("pthread_create");
	}
/*r_ w command*/


int tr;

while(1) {

/**************input****/
__fpurge(stdin);
char *page_str = (char*) malloc(page_size * sizeof(char));  
char rw = 'w';
int range  = 2;
printf("> Which command should I run? (r:read, w:write, v: view msi list):");
scanf("%c", &rw);
printf("> For which page? (0-%ld, or -1 for all):", num_pages);
__fpurge(stdin);
scanf("%d", &range);
/*******************/
     if (range == -1) {

         if(rw == 'r') {
	       // printf("-----read all the pages------------\n");
	        l = 0x0;
	        while (l < num_pages) {
                strcpy(page_str, addr+ l*page_size); /*read otut the str in the pages*/
                printf("[*] page:%d\n %s \n",l, page_str);
                state[l] = S;
                page_track = l;
                l++;
                        }
                    }

        if(rw == 'w'){   
                 printf("what do you want to write <all pages>: \n"); // problem in the stdin 
                __fpurge(stdin);
                fgets(page_str,page_size, stdin);

	             l = 0x0;
	            while (l < num_pages) {
                    page_track = l;
                    send(sock,invalid,strlen(invalid),0);
 //                   printf("Error %d\n", er);
                    char t[BUFF_SIZE] = "0";
                    sprintf(t,"%d", l );
                     send(sock,t,strlen(t),0); 
                    memcpy(addr + l * page_size, page_str, page_size );
                    state[l] = M;
                        l++;
	                     } 
                     }

        if (rw == 'v'){
            int l =0;
            while(l<num_pages){
                printf("page number: %d, MSI status: %d\n",l, state[l]);
                l++;
            }
        }
     }

     //page number     
     else  {
         page_track = range;

         if(rw == 'r') {
                strcpy(page_str, addr+ range*page_size); /*read otut the str in the pages*/
                printf("[*] page:%d\n %s \n",range, page_str);
                state[range] = S;

         }
        if(rw == 'w'){   
                 printf("What do you want to write in page - %d \n", range); // problem in the stdin 
                __fpurge(stdin);
                fgets(page_str,page_size, stdin);

                 send(sock,invalid,strlen(invalid),0);
                //    printf("main send failure %d\n", er);
                    char t[BUFF_SIZE] = "0";
                    sprintf(t,"%d", range );
                    send(sock,t,strlen(t),0); 
                    memcpy(addr + range * page_size, page_str, page_size );
                    state[range] = M;
                     }
        
        if (rw == 'v'){
            int l =0;
            while(l<num_pages){
                printf("page number: %d, MSI status: %d\n",l, state[l]);
                    l++;
            }
        }

                 } 

}
     /*  */

    pthread_join(server_thr, NULL);
    pthread_join(client_thr, NULL);

    if(munmap(addr, len) == 0){
        printf("Info: address unmapped\n");
    }
   
   exit(EXIT_SUCCESS);

}







