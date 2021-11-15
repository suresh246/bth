#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>



// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG

#define DEBUG
// INCLUDED MAX RECV  BUF SIZE

#define MAXDATASIZE 1401 //MAX NUM OF BYTES WE CAN GET AT ONCE
// Included to get the support library

#include <calcLib.h>
// get sockaddr, IPv4 or IPv6:



void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	if (sa->sa_family == AF_INET6) {
	  	return &(((struct sockaddr_in6*)sa)->sin6_addr);
	}
	printf("Unknown FAMILY!!!!\n");
	return(0); 
}



int main(int argc, char *argv[]){

  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'.
     SAMPLE USAGE 13.53.76.30:5000 
  */
  if (argc != 2) {
     fprintf(stderr,"usage: client hostname:port\n");
     exit(1);
    }
  char delim[]=":";
  char calcDelim[] = " ";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  int  sockfd, numbytes;
  char buf[MAXDATASIZE];
  char *test_buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
  char message[]="OK\n";


  /* Initialize the library, this is needed for this library. */
  //initCalcLib();
  char *ptr;
  double f1,f2,fresult;
  int i1,i2,iresult;
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  //int  port=atoi(Destport);
  #ifdef DEBUG 
  printf("Host %s, and port %s.\n",Desthost,Destport);
  #endif
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;


 if ((rv = getaddrinfo(Desthost, Destport, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }    
 
 //Desthost
 
 // loop through all the results and connect to the first we can
 for(p = servinfo; p != NULL; p = p->ai_next) {
	 if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
	printf("Connection successfull.\n");
        break;
    }
    /* Why are we here??? p == NULL, Looped no match (socket &| connect).. 
       Success?? */
    if (p == NULL) {
        fprintf(stderr, "client: failed to socket||connect\n");
	/* Clear servinfo */
	freeaddrinfo(servinfo); // all done with this structure
        return 2;
    }
    //int q=inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);
    /* q ok or not */
    printf("client: connected to %s:%s\n", s,Destport);
    freeaddrinfo(servinfo); // all done with this structure
    int totalBytes=0;
    buf[0] = '\0';
    char *smsg; // string to send to server
    unsigned int smsgLen; // length of string to server
    double s1,s2,sresult;
    
    /*
    s1 = 24.7;
    s2 = 31.724;
    sresult = s1*s2;
    char sresult_s[MAXDATASIZE];

    sprintf(sresult_s, "%8.8g", sresult);
    //strcat(sresult_s,"\n");
    send(sockfd, &sresult_s, sizeof(sresult_s),0);
    */
    while(1){
      numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0);
      if(numbytes == -1 ){
        perror("recv");
	/* close socket */
        exit(1);
      }
      if (numbytes == 0) {
	printf("Server closed.\n");
	break;
      }
      
      totalBytes+=numbytes;
  
      printf("client (%d/%d) : received '%s'\n",numbytes, totalBytes, buf);
      
      break;
      
    }
    numbytes = send(sockfd,message,sizeof(message),0);


    while(1) {

	    numbytes = recv(sockfd, buf, MAXDATASIZE-1,0);
	    if(numbytes == -1) {

		    perror("recv");
		    exit(1);

             }

	    if(numbytes == 0) {

		    printf("server closed.\n");
		    break;
	    }


	    totalBytes+= numbytes;
	    //printf("client (%d/%d) : received '%s'\n",numbytes, totalBytes, buf);
            ptr = buf;

	    //printf("string = %c, \n", ptr[0]);
	    /*
	    double s1,s2,sresult,f1,f2,fresult;
            int i1,i2,iresult;
	    s1 = 24.7;
	    s2 = 31.724;
	    sresult = s1*s2;
	    char sresult_s[MAXDATASIZE];
	    sprintf(sresult_s, "%f", sresult);
            strcat(sresult_s,"\n");
            send(sockfd, sresult_s, sizeof(sresult_s),0);
            */
	    if(ptr[0] == 'f'){	   
                   printf("client (%d/%d) : received float operation : '%s'\n",numbytes, totalBytes, ptr);
		   char *cmd = strtok(ptr,calcDelim);
		   printf("cmd operation is : '%s'\n", cmd);
		   char *remStr1 = strtok(NULL,calcDelim);
		   f1 = atof(remStr1);
		   printf("1st value is : '%f'\n", f1);
		   char *remStr2 = strtok(NULL,calcDelim);
		   f2 = atof(remStr2);
		   printf("2nd value is : '%f'\n", f2);
		   if (strcmp(cmd,"fmul")==0){
		   fresult = f1*f2;
		   //printf("fresult_usingstd %s\n",std::to_string(fresult));
		   }
		   else {
		   break;
		   }
		   printf("fresult is : '%f'\n",fresult);
		   char fresult_s[MAXDATASIZE];
		   memset(fresult_s,0,MAXDATASIZE);
		   printf("fresults_before:%s,len of:%ld\n", fresult_s,strlen(fresult_s)); 
		   sprintf(fresult_s, "%f", fresult);
		   strcat(fresult_s,"\n");
		   //printf("Enter the result:\n");
		   //char linebuffer[20];
		   //fresult_s = sscanf(linebuffer,"%s",cmmd);
                   //scanf("%s",fresult_s);
		   //printf("scanned_input:%s\nLength of the scanned ip is:%d\n",fresult_s,strlen(fresult_s));
		   //memcpy(&fresult_s[MAXDATASIZE-1],"\n",strlen("\n"));
		   numbytes = send(sockfd, fresult_s, strlen(fresult_s),0);
		   printf("fresult is %s\n%d bytes has been sent to server.\n", fresult_s, numbytes);
		   //sleep(3);
		   //buf[0] = '\0';
		   memset(buf,0,MAXDATASIZE);
                   numbytes = recv(sockfd, buf, strlen(buf),0);
		         if(numbytes == -1 ){
				 perror("recv");
				 exit(1);
			 }
                         if (numbytes == 0) {
	                 printf("Server closed.\n");
	                 break;
                         }
		   printf("received buffer is %s Size is %d \n",test_buf,numbytes);
		   
		   break;
		   
	    }
	    else {
		    printf("client (%d/%d) : received int operation: '%s'\n",numbytes, totalBytes, ptr);
		    break;
		    }
    }
    
    close(sockfd);
    return 0;
}




