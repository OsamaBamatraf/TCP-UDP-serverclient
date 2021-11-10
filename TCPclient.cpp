/* Osama Bamatraf
CPSC 441 assignment 2
Simple client code that communicates with indirection server. Largely based on Carey's client code from assignment 0 with minor edits.

User can vote, view candidates list and view results

 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

/* Some generic error handling stuff */
extern int errno;
void perror(const char *s);

/* Manifest constants used by client program */
#define MAX_HOSTNAME_LENGTH 64
#define MAX_MESSAGE_LENGTH 100
#define BYNAME 1
#define SERVERPORTNUM 12343   /* must match the server's port! */

/* Main program of client */
int main()
  {
    struct sockaddr_in server;
    struct hostent *hp;
    char hostname[MAX_HOSTNAME_LENGTH];
    char message[MAX_MESSAGE_LENGTH];
    char messageback[MAX_MESSAGE_LENGTH];
    int sockfd, len, bytes, done;
    char c;

    /* Initialization of server sockaddr data structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVERPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

#ifdef BYNAME
    /* use a resolver to get the IP address for a domain name */
    strcpy(hostname, "csx3.cpsc.ucalgary.ca");
    hp = gethostbyname(hostname);
    if (hp == NULL)
      {
	fprintf(stderr, "%s: unknown host\n", hostname);
	exit(1);
      }
    /* copy the IP address into the sockaddr structure */
    bcopy(hp->h_addr, &server.sin_addr, hp->h_length);
#else
    /* hard code the IP address so you don't need hostname resolver */
    server.sin_addr.s_addr = inet_addr("136.159.5.25");
#endif

    /* create the client socket for its transport-level end point */
    if( (sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
      {
	fprintf(stderr, "testclient: socket() call failed!\n");
	exit(1);
      }

    /* connect the socket to the server's address using TCP */
    if( connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in)) == -1 )
      {
	fprintf(stderr, "testclient: connect() call failed!\n");
	perror(NULL);
	exit(1);
      }

    /* Print welcome banner */
    printf("Welcome! This is a client for assignment 2 of CPSC 441!\n Choose a service (TRANSLATE, CONVERT, VOTING).\n" );

    /* main loop: read a word, send to server, and print answer received */
    done = 0;
    while( !done )
      {
	/* prompt user for the input */
	printf("Enter a command: ");
	len = 0;
	while( (c = getchar()) != '\n' )
	  {
	    message[len] = c;
	    len++;
	  }

	/* make sure the message is null-terminated in C */
	message[len] = '\0';

	/* send it to the server via the socket */
	send(sockfd, message, len, 0);

	/* see what the server sends back */
	if( (bytes = recv(sockfd, messageback, MAX_MESSAGE_LENGTH, 0)) > 0 )
	  {
	    /* make sure the message is null-terminated in C */
	    messageback[bytes] = '\0';
	    printf("Response from server: ");
	    printf("`%s'\n", messageback);
	  }
	else
	  {
	    /* an error condition if the server dies unexpectedly */
	    printf("Oh my! Server seems to have failed!\n");
	    close(sockfd);
	    exit(1);
	  }

	/* check to see if done this session */
	if( strncmp(message, "BYE", 3) == 0 )
	  done = 1;
      }

    /* Program all done, so clean up and exit the client */
    close(sockfd);
    exit(0);
  }