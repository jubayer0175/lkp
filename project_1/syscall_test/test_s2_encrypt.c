/*
 *@description: the code is written to the system call properties of an encryption syscalls
 *@Jubayer, Virgina Tech
 */

#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>


/*Error display message*/

void print_error(void){
    printf("Error: usage s2_encrypt_user -s <string> -k <key> \n");

}
/*---------------------------------*/

int main(int argc, char **argv){
// default for test
char *msg="hello";
unsigned int key=1; // these value are replaced later with the argvs
//int kflag=0;
//int sflag=0;
int c;
/*check id the number of argument is fine*/
if(argc < 5) {
    print_error();
    exit(2);
}
/*Check for argument options*/
while ((c = getopt (argc, argv, "s:k:")) != -1){
    switch(c) {
        case 's':
  //          sflag=1;
            msg=argv[2];
           // printf("Message %s\n",msg);
            break;
        case 'k':
    //        kflag=1;
            key=atoi(argv[4]);
           // printf("key is %d\n",key);
            break;
        default: 
                print_error();
    } 
}


long int ret =syscall(335,msg,strlen(msg)+1,key);
if (ret==0) printf("Return value %ld\n",ret);
else printf("Returned value: %d\n",errno);

return 0;

}
