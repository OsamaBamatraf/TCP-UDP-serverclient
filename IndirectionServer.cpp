/* Osama Bamatraf
CPSC 441 assignment 2
Indirection server based on Carey's server code for assignment 0. Modified to be a TCP server for the user and UDP client which communicates with 3 UDP micro-servers 


 */

/* Include files for C socket programming and stuff */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

/* Global manifest constants */
#define MAX_MESSAGE_LENGTH 100
#define SERVERPORTNUM 12343
#define SERVER_IP "136.159.5.27"  /* csx3.cpsc.ucalgary.ca */
#define TRANSPORT 1818
#define CONVPORT 1919
#define VOTEPORT 2119


/* Optional verbose debugging output */
#define DEBUG 1

/* Global variable */
int childsockfd;

/* This is a signal handler to do graceful exit if needed */
void catcher( int sig )
  {
    close(childsockfd);
    exit(0);
  }

/* Main program for server */
int main()
  {
    struct sockaddr_in server, client;
    static struct sigaction act;
	struct sockaddr_in utr_server;
    struct sockaddr *utserver;
	struct sockaddr_in conv_server;
    struct sockaddr *convserver;
	struct sockaddr_in vote_server;
    struct sockaddr *voteserver;
	char messagestat[MAX_MESSAGE_LENGTH];
    char messagein[MAX_MESSAGE_LENGTH];
    char messageout[MAX_MESSAGE_LENGTH];
	char transWelcome[MAX_MESSAGE_LENGTH];
	char convWelcome[MAX_MESSAGE_LENGTH];
	char convAmountPrompt[MAX_MESSAGE_LENGTH];
	char voteWelcome[MAX_MESSAGE_LENGTH];
	char convmessagein [MAX_MESSAGE_LENGTH];
	char convmessageamount[MAX_MESSAGE_LENGTH];
	char votemessagein[MAX_MESSAGE_LENGTH];
	char voteid[MAX_MESSAGE_LENGTH];
	char voteidprompt[MAX_MESSAGE_LENGTH];
	char voteidencry[MAX_MESSAGE_LENGTH];
	char votemessageout[MAX_MESSAGE_LENGTH];
	char udptranslatemessagein[MAX_MESSAGE_LENGTH];
    int parentsockfd, c, pid, conns;
	int translate_sock, ti, tlen = sizeof(utr_server);
	int conv_sock, ci, clen = sizeof(conv_server);
	int vote_sock, vi, vlen = sizeof(vote_server);
	int readTrans;
	int readConv;
	int readVote;
	int termi = 0;
	int voteID;

    /* Set up a signal handler to catch any unusual termination conditions. */
    act.sa_handler = catcher;
    sigfillset(&(act.sa_mask));
    sigaction(SIGPIPE, &act, NULL);

    /* Initialize server sockaddr structure */
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVERPORTNUM);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    /* set up the transport-level end point to use TCP */
    if( (parentsockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1 )
      {
	fprintf(stderr, "testserver: socket() call failed!\n");
	exit(1);
      }

    /* bind a specific address and port to the end point */
    if( bind(parentsockfd, (struct sockaddr *)&server, sizeof(struct sockaddr_in) ) == -1 )
      {
	fprintf(stderr, "testserver: bind() call failed!\n");
	exit(1);
      }

    /* start listening for incoming connections from clients */
    if( listen(parentsockfd, 5) == -1 )
      {
	fprintf(stderr, "testserver: listen() call failed!\n");
	exit(1);
      }


/* initializing translation socket to communication with translation microservice*/
//inspired by Carey's UDP client code

 if ((translate_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      {
	printf("Could not setup a socket!\n");
	return 1;
      }
      
    memset((char *) &utr_server, 0, sizeof(utr_server));
    utr_server.sin_family = AF_INET;
    utr_server.sin_port = htons(TRANSPORT);
    utserver = (struct sockaddr *) &utr_server;

    if (inet_pton(AF_INET, SERVER_IP, &utr_server.sin_addr)==0)
      {
	printf("inet_pton() failed\n");
	return 1;
      }

/* initializing conversion socket */

if ((conv_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      {
	printf("Could not setup a socket!\n");
	return 1;
      }
      
    memset((char *) &conv_server, 0, sizeof(conv_server));
    conv_server.sin_family = AF_INET;
    conv_server.sin_port = htons(CONVPORT);
    convserver = (struct sockaddr *) &conv_server;

    if (inet_pton(AF_INET, SERVER_IP, &conv_server.sin_addr)==0)
      {
	printf("inet_pton() failed\n");
	return 1;
      }


/* initializing voting socker */
if ((vote_sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      {
	printf("Could not setup a socket!\n");
	return 1;
      }
      
    memset((char *) &vote_server, 0, sizeof(vote_server));
    vote_server.sin_family = AF_INET;
    vote_server.sin_port = htons(VOTEPORT);
    voteserver = (struct sockaddr *) &vote_server;

    if (inet_pton(AF_INET, SERVER_IP, &vote_server.sin_addr)==0)
      {
	printf("inet_pton() failed\n");
	return 1;
      }



    /* initialize message strings just to be safe (null-terminated) */
    bzero(messagein, MAX_MESSAGE_LENGTH);
    bzero(messageout, MAX_MESSAGE_LENGTH);

    fprintf(stderr, "Welcome! I am the CPSC 441 test server for L01!!\n");
    fprintf(stderr, "Server listening on TCP port %d...\n\n", SERVERPORTNUM);

    conns = 0;
    c = sizeof(struct sockaddr_in);
    
    /* Main loop: server loops forever listening for requests */
    for( ; ; )
      {
	/* accept a connection */
	if( (childsockfd = accept(parentsockfd, (struct sockaddr *)&client, (socklen_t *) &c)) == -1 )
	  {
	    fprintf(stderr, "testserver: accept() call failed!\n");
	    exit(1);
	  }

	/* increment server's counter variable */
	conns++;
	
	/* try to create a child process to deal with this new client */
	pid = fork();

	/* use process id (pid) returned by fork to decide what to do next */
	if( pid < 0 )
	  {
	    fprintf(stderr, "testserver: fork() call failed!\n");
	    exit(1);
	  }
	else if( pid == 0 )
	  {
	    /* this is the child process doing this part */

	    /* don't need the parent listener socket that was inherited */
	    close(parentsockfd);

	    /* obtain the message from this client */
	    int bytes;
	    int done = 0;
	    while( !done )
	      {
		bytes = recv(childsockfd, messagein, MAX_MESSAGE_LENGTH, 0);
#ifdef DEBUG
		/* print out the received message */
		printf("Child process received %d bytes with command: <%s>\n",
		       bytes, messagein);
#endif
	
		/* create the outgoing message based on command */

		if( strncmp(messagein, "BYE", 3) == 0 )
		  {
		    sprintf(messageout, "Goodbye! See you next time.\n");
		    done = 1;
		  }
		

//sending to translation service

		else if ( strncmp(messagein, "TRANSLATE", 9) == 0 ) {
			int translateDone = 0;
			while (!translateDone){
			sprintf(transWelcome, "Enter word to translate: ");
			if( send(childsockfd, transWelcome, strlen(transWelcome), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
			bytes = recv(childsockfd, udptranslatemessagein, MAX_MESSAGE_LENGTH, 0);
		if (sendto(translate_sock, udptranslatemessagein, strlen(messagein), 0, utserver, sizeof(utr_server)) == -1)
	  		{
	   		 printf("sendto failed\n");
	    		return 1;
	  		}

		if ((readTrans=recvfrom(translate_sock, messageout, MAX_MESSAGE_LENGTH, 0, utserver, (socklen_t*)&tlen))==-1)
	 	 	{
	    printf("Read error!\n");
	  		}
			messageout[readTrans] = '\0'; // padding with end of string symbol

			if( send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
		bzero(udptranslatemessagein, MAX_MESSAGE_LENGTH);
		translateDone = 1;

			}




		}

//sending to conversion service

		else if ( strncmp(messagein, "CONVERT", 7) == 0 ) {
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
		bzero(convmessageamount, MAX_MESSAGE_LENGTH);
		bzero(convmessagein, MAX_MESSAGE_LENGTH);
		bzero(convAmountPrompt, MAX_MESSAGE_LENGTH);
			int convertDone = 0;
			
			while (!convertDone){
			sprintf(convWelcome, "Enter currency to convert CAD into (USD, EUR, GBP, BTC): ");
			if( send(childsockfd, convWelcome, strlen(convWelcome), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
			bytes = recv(childsockfd, convmessagein, MAX_MESSAGE_LENGTH, 0);
			//sending currency to converter
		if (sendto(conv_sock, convmessagein, strlen(convmessagein), 0, convserver, sizeof(conv_server)) == -1)
	  		{
	   		 printf("sendto failed\n");
	    		return 1;
	  		}
			//prompting for amount
		if ((readConv=recvfrom(conv_sock, convAmountPrompt, MAX_MESSAGE_LENGTH, 0, convserver, (socklen_t*)&clen))==-1)
	 	 	{
			 printf("Read error!\n");

			  }
			
		if( send(childsockfd, convAmountPrompt, strlen(convAmountPrompt), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
		bytes = recv(childsockfd, convmessageamount, MAX_MESSAGE_LENGTH, 0);
		//sending amount to converter
		if (sendto(conv_sock, convmessageamount, strlen(convmessageamount), 0, convserver, sizeof(conv_server)) == -1)
	  		{
	   		 printf("sendto failed\n");
	    		return 1;
	  		}

		//recieving the converted amount
		if ((readConv=recvfrom(conv_sock, messageout, MAX_MESSAGE_LENGTH, 0, convserver, (socklen_t*)&clen))==-1)
	 	 	{
	    printf("Read error!\n");
	    //quit = 1;
	  		}
			messageout[readConv] = '\0'; // padding with end of string symbol

			if( send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
		bzero(convmessageamount, MAX_MESSAGE_LENGTH);
		bzero(convmessagein, MAX_MESSAGE_LENGTH);
		bzero(convAmountPrompt, MAX_MESSAGE_LENGTH);
		convertDone = 1;

			}

		}


//sending to voting service
		else if ( strncmp(messagein, "VOTING", 7) == 0 ) {
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
		bzero(voteid, MAX_MESSAGE_LENGTH);
		bzero(votemessagein, MAX_MESSAGE_LENGTH);
		bzero(voteidencry, MAX_MESSAGE_LENGTH);
		bzero(voteidprompt, MAX_MESSAGE_LENGTH);
		bzero(voteWelcome, MAX_MESSAGE_LENGTH);
			int voteDone = 0;
			while (!voteDone){
			sprintf(voteWelcome, "Please enter a command (VOTE, CANDIDATES, RESULTS): ");
			if( send(childsockfd, voteWelcome, strlen(voteWelcome), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
			bytes = recv(childsockfd, votemessagein, MAX_MESSAGE_LENGTH, 0);
			
		if (sendto(vote_sock, votemessagein, strlen(votemessagein), 0, voteserver, sizeof(vote_server)) == -1)
	  		{
	   		 printf("sendto failed\n");
	    		return 1;
	  		}

		//if the user wants to vote
		if ((strncmp(votemessagein, "VOTE", 4) == 0) ){
			if ((readVote=recvfrom(vote_sock, voteidprompt, MAX_MESSAGE_LENGTH, 0, voteserver, (socklen_t*)&vlen))==-1)
	 	 		{
				 printf("Read error!\n");

			 	 }
		if( send(childsockfd, voteidprompt, strlen(voteidprompt), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }
		bytes = recv(childsockfd, voteid, MAX_MESSAGE_LENGTH, 0);
	
	//encrypting the vote id
		voteID = atoi(voteid);
		voteID = voteID*4;
		sprintf(voteidencry, "%d", voteID);
		
		if (sendto(vote_sock, voteidencry, strlen(voteidencry), 0, voteserver, sizeof(vote_server)) == -1)
	  		{
	   		 printf("sendto failed\n");
	    		return 1;
	  		}

		if ((readVote=recvfrom(vote_sock, messageout, MAX_MESSAGE_LENGTH, 0, voteserver, (socklen_t*)&vlen))==-1)
	 	 	{
	    printf("Read error!\n");
	    //quit = 1;
	  		}
			messageout[readVote] = '\0'; // padding with end of string symbol

			if( send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  {
		    fprintf(stderr, "Send failed on connection %d\n", conns);
		  }

			}
		//if the user does anything else other than voting
		else {

			if ((readVote=recvfrom(vote_sock, messageout, MAX_MESSAGE_LENGTH, 0, voteserver, (socklen_t*)&vlen))==-1)
	 	 		{
				 printf("Read error!\n");

			 	 }

				if( send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  		{
		   		 fprintf(stderr, "Send failed on connection %d\n", conns);
		  		}
		

		}
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
		bzero(voteid, MAX_MESSAGE_LENGTH);
		bzero(votemessagein, MAX_MESSAGE_LENGTH);
		bzero(voteidencry, MAX_MESSAGE_LENGTH);
		bzero(voteidprompt, MAX_MESSAGE_LENGTH);
		bzero(voteWelcome, MAX_MESSAGE_LENGTH);
		voteDone = 1;
			
			}

		}





		//user enters invalid command
		else{


 		sprintf(messageout, "Unrecognized command. Sorry! \n");
		if( send(childsockfd, messageout, strlen(messageout), 0) < 0)
		  		{
		   		 fprintf(stderr, "Send failed on connection %d\n", conns);
		  		}

		}

		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
		  }
		

	


		
	  
#ifdef DEBUG
		printf("Child about to send message: %s\n", messageout);
#endif

		/* send the result message back to the client */
		

		/* clear out message strings again to be safe */
		bzero(messagein, MAX_MESSAGE_LENGTH);
		bzero(messageout, MAX_MESSAGE_LENGTH);
	      

	    /* when client is no longer sending information to us, */
	    /* the socket can be closed and the child process terminated */
	    fprintf(stderr, "Shutting down child process for conn %d\n", conns);
	    close(childsockfd);
	    exit(0);
	  } /* end of then part for child */
	else
	  {
	    /* the parent process is the one doing this part */
	    fprintf(stderr, "Created child process %d to handle that client\n", pid);
	    fprintf(stderr, "Parent going back to job of listening...\n\n");

	    /* parent doesn't need the childsockfd */
	    close(childsockfd);
		close(translate_sock);
		close(conv_sock);
		close(vote_sock)

	  }
	  }
      
  }
  