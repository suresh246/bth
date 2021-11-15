#include <stdio.h>
#include <stdlib.h>
#include<sys/utsname.h>



/* array of char* that points to char arrays.  */
char *arith[]={"add","div","mul","fsub","fadd","fdiv","fmul","fsub"};

int Listitems=sizeof(arith)/(sizeof(char*));
int main(){
struct utsname detect;
uname(&detect);

printf("########################\n");
printf(" Virtual machine Info \n");
printf("#######################\n");
printf("System name - %s \n", detect.sysname);
printf("Nodename    - %s \n", detect.nodename);
printf("Release     - %s \n", detect.release);
printf("Version     - %s \n", detect.version);
printf("Machine     - %s \n", detect.machine);
printf("Domain name - %s \n\n", detect.__domainname);



printf("Arith List size: %ld bytes \nChar Size %ld bytes \nList Items Size %d items\n",sizeof(arith),sizeof(char*), Listitems);


return 0;
}
