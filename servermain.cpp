#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

// Included to get the support library
#include <calcLib.h>

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
#define DEBUG
#define BACKLOG 1	 // how many pending connections queue will hold
#define SECRETSTRING "gimboid"

void sigchld_handler(int s)
{
	(void)s; // quiet unused variable warning

	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


using namespace std;

int main(int argc, char *argv[]){
  
  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
	
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  int port=atoi(Destport);
  #ifdef DEBUG  
  printf("Host %s, and port %d.\n",Desthost,port);
  #endif

  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;
  #define PORT "5000"  // the port users will be connecting to
  
    /* Initialize the library, this is needed for this library. */
  initCalcLib();
  char *ptr;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
  
  	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	printf("server: waiting for connections BINJER...\n");
	char msg[1500];
	char msg_result[1500];
	int j, MAXSZ=sizeof(msg);

	int  childCnt=0;
	int  numbytes;
	char command[10];
	char temp1[20], temp2[20];
	char optionstring[128];
	int optionint1;
	int optionint2;

	while(1) {  // main accept() loop
	  
	  sin_size = sizeof(their_addr);
	  new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	  if (new_fd == -1) {
	    perror("accept");
	    continue;
	  }
	  /* ------------- */
	  inet_ntop(their_addr.ss_family,
		    get_in_addr((struct sockaddr *)&their_addr),
		    s, sizeof s);
	  printf("server: Connection %d from %s\n",childCnt, s);

	  printf("server: Sending TEXT TCP 1.0\n\n\n");
	  struct sockaddr_in *local_sin=(struct sockaddr_in*)&their_addr;

	  sprintf(msg,"%s\n\n","TEXT TCP 1.0");

      numbytes = send(new_fd, msg, strlen(msg), 0);

      if(numbytes == -1 ){
         perror("send");
         exit(1);
      }

      printf("\nChild[%d] (%s:%d): send(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);
      bzero(msg,MAXSZ);


	  while(1){
	    numbytes=recv(new_fd,&msg,MAXSZ,0);
	    
	    
		if (msg[0] == 'O' & msg[1] == 'K' & msg[2] == '\n'){
			printf("\nChild[%d] (%s:%d): recv(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);
			bzero(msg,MAXSZ);

		  ptr=randomType(); // Get a random arithemtic operator. 

          double f1,f2,fresult;
          int i1,i2,iresult;

          i1=randomInt();
          i2=randomInt();

          f1=randomFloat();
          f2=randomFloat();

          //printf("Int Values: %d %d \n",i1,i2);
          //printf("Float Values: %8.8g %8.8g \n",f1,f2);
		
		  /* Act differently depending on what operator you got, judge type by first char in string. If 'f' then a float */
  
          if(ptr[0]=='f'){
          /* At this point, ptr holds operator, f1 and f2 the operands. Now we work to determine the reference result. */
   
          if(strcmp(ptr,"fadd")==0){
               fresult=f1+f2;
          } else if (strcmp(ptr, "fsub")==0){
               fresult=f1-f2;
          } else if (strcmp(ptr, "fmul")==0){
               fresult=f1*f2;
          } else if (strcmp(ptr, "fdiv")==0){
               fresult=f1/f2;
          }

          //printf("%s %8.8g %8.8g = %8.8g\n",ptr,f1,f2,fresult);
		  sprintf(msg, "%s %8.8g %8.8g",ptr, f1, f2);
		  sprintf(msg_result,"%8.8g\n",fresult);

		  numbytes = send(new_fd, msg, strlen(msg), 0);

          if(numbytes == -1 ){
             perror("send");
             exit(1);
           }

          printf("\nChild[%d] (%s:%d): send(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);		  
		  bzero(msg,MAXSZ);
          numbytes=recv(new_fd,&msg,MAXSZ,0);
	      printf("\nChild[%d] (%s:%d): recv(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);


		  for (j=0; j<=strlen(msg); j++ ) {
		        if (msg[j] != msg_result[j]) {
				   bzero(msg, MAXSZ);
			       printf("ERROR\n");
				   msg[0] = 'E';
				   msg[1] = 'R';
				   msg[2] = 'R';
				   msg[3] = 'O';
				   msg[4] = 'R';
				   msg[5] = '\n';
				   msg[strlen(msg)] = '\0';
				   numbytes = send(new_fd, msg, strlen(msg), 0);
		  
                if(numbytes == -1 ){
                    perror("send");
                    exit(1);
                    }

                printf("\nChild[%d] (%s:%d): send(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);
		        bzero(msg,MAXSZ);
				break;

		        }
		        bzero(msg, MAXSZ);
				msg[0] = 'O';
				msg[1] = 'K';
				msg[2] = '\n';
				msg[strlen(msg)] = '\0';
				numbytes = send(new_fd, msg, strlen(msg), 0);
		  
                if(numbytes == -1 ){
                     perror("send");
                     exit(1);
                    }
                printf("\nChild[%d] (%s:%d): send(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);
		        bzero(msg,MAXSZ);

		}
		bzero(temp1,sizeof(temp1));
		bzero(temp2,sizeof(temp2));
		bzero(msg,sizeof(msg));
		bzero(msg_result,sizeof(MAXSZ));

        } else {
          if(strcmp(ptr,"add")==0){
             iresult=i1+i2;
        } else if (strcmp(ptr, "sub")==0){
            iresult=i1-i2;
        } else if (strcmp(ptr, "mul")==0){
            iresult=i1*i2;
        } else if (strcmp(ptr, "div")==0){
          iresult=i1/i2;
        }
		  
        //printf("%s %d %d = %d \n",ptr,i1,i2,iresult);
		sprintf(msg,"%s %d %d",ptr,i1,i2);
		sprintf(msg_result,"%d\n",iresult);

		int j;

		numbytes = send(new_fd, msg, strlen(msg), 0);
		  
        if(numbytes == -1 ){
             perror("send");
             exit(1);
        }

        printf("\nChild[%d] (%s:%d): send(%d bytes): %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);

		bzero(msg,MAXSZ);
        numbytes=recv(new_fd,&msg,MAXSZ,0);
	    printf("\nChild[%d] (%s:%d): recv(%d bytes): %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);

        for (j = 0; j <= strlen(msg); j++ ){
			  if (msg[j] != msg_result[j]) {
				  
				bzero(msg, MAXSZ);
				msg[0] = 'E';
				msg[1] = 'R';
				msg[2] = 'R';
				msg[3] = 'O';
				msg[4] = 'R';
				msg[5] = '\n';
				msg[strlen(msg)] = '\0';

				numbytes = send(new_fd, msg, strlen(msg), 0);
		  
                if(numbytes == -1 ){
                     perror("send");
                     exit(1);
                    }

                printf("\nChild[%d] (%s:%d): send(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);
		        bzero(msg,MAXSZ);
				break;
		    }
                bzero(msg,MAXSZ);
				msg[0] = 'O';
				msg[1] = 'K';
				msg[2] = '\n';
				msg[strlen(msg)] = '\0';

				numbytes = send(new_fd, msg, strlen(msg), 0);
		  
                if(numbytes == -1 ){
                     perror("send");
                     exit(1);
                    }

                printf("\nChild[%d] (%s:%d): send(%d bytes) : %s\n", childCnt,s,ntohs(local_sin->sin_port),numbytes,msg);
		        bzero(msg,MAXSZ);
	
		}
		bzero(temp1,sizeof(temp1));
		bzero(temp2,sizeof(temp2));
		bzero(msg,sizeof(MAXSZ));
		bzero(msg_result,sizeof(MAXSZ));
        }

		}
	    if(numbytes==0){
	      printf("\nChild [%d] died.\n",childCnt);
	      close(new_fd);
	      break;
	    }
	    msg[numbytes]=0;
		}
		continue;
	}
	
return 0;
}
