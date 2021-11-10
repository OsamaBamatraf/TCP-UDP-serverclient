/* Osama Bamatraf
CPSC 441 assignment 2
Simple currency conversion microservice based on Carey's wordlen-server.c code
Converts currency from CAD to USD, EUR, GBP, BTC

 */
 
/* Include files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

/* Manifest constants */
#define MAX_BUFFER_SIZE 100
#define PORT 1919

/* Verbose debugging */
/* #define DEBUG 1 */

/* Main program */
int main()
  {
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    int s, i, len=sizeof(si_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
	char messageCurrency[MAX_BUFFER_SIZE];
	char messageAmount [MAX_BUFFER_SIZE];
	char messageWelcomePrompt [MAX_BUFFER_SIZE];
    int readBytes;
	int amount;
	float converted;

    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
      {
	printf("Could not setup a socket!\n");
	return 1;
      }
    
    memset((char *) &si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(PORT);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *) &si_server;
    client = (struct sockaddr *) &si_client;

    if (bind(s, server, sizeof(si_server))==-1)
      {
	printf("Could not bind to port %d!\n", PORT);
	return 1;
      }
    fprintf(stderr, "Welcome! I am the Currency Conversion server!!\n");
    printf("server now listening on UDP port %d...\n", PORT);
	bzero(messagein, MAX_BUFFER_SIZE);
	bzero(messageout, MAX_BUFFER_SIZE);
	bzero(messageAmount, MAX_BUFFER_SIZE);
	bzero(messageCurrency, MAX_BUFFER_SIZE);
	bzero(messageWelcomePrompt, MAX_BUFFER_SIZE);
    /* big loop, looking for incoming messages from clients */
    for( ; ; )
      {
	/* clear out message buffers to be safe */
	bzero(messagein, MAX_BUFFER_SIZE);
	bzero(messageout, MAX_BUFFER_SIZE);
	bzero(messageAmount, MAX_BUFFER_SIZE);
	bzero(messageCurrency, MAX_BUFFER_SIZE);
	bzero(messageWelcomePrompt, MAX_BUFFER_SIZE);

	/* see what comes in from a client, if anything */
	if ((readBytes=recvfrom(s, messagein, MAX_BUFFER_SIZE, 0, client, (socklen_t*)&len)) < 0)
	  {
	    printf("Read error!\n");
	    return -1;
	  }
#ifdef DEBUG
	else printf("Server received %d bytes\n", readBytes);
#endif

	printf("  server received \"%s\" from IP %s port %d\n",
	       messagein, inet_ntoa(si_client.sin_addr), ntohs(si_client.sin_port));

#ifdef DEBUG
	printf("server thinks that word has %d characters...\n", strlen(messagein));
#endif
//converting to usd
	if( strncmp(messagein, "USD", 3) == 0 ){

		sprintf(messageWelcomePrompt, "Please enter amount: ");
		sendto(s, messageWelcomePrompt, strlen(messageWelcomePrompt), 0, client, len);		
      
		if ((readBytes=recvfrom(s, messageAmount, MAX_BUFFER_SIZE, 0, client, (socklen_t*)&len)) < 0)
	  {
	    printf("Read error!\n");
	    return -1;
	  }
	amount = atoi(messageAmount);
	converted = amount * 0.79;
	sprintf(messageout, "%fUSD",converted);
	}
		 
	
//converting to eur

	else if( strncmp(messagein, "EUR", 3) == 0 ){

		sprintf(messageWelcomePrompt, "Please enter amount: ");
		sendto(s, messageWelcomePrompt, strlen(messageWelcomePrompt), 0, client, len);
		if ((readBytes=recvfrom(s, messageAmount, MAX_BUFFER_SIZE, 0, client, (socklen_t*)&len)) < 0)
	  {
	    printf("Read error!\n");
	    return -1;
	  }
	amount = atoi(messageAmount);
	converted = amount * 0.69;
	sprintf(messageout, "%fEUR",converted);
		}

//converting to gbp
	else if( strncmp(messagein, "GBP", 3) == 0 ){

		sprintf(messageWelcomePrompt, "Please enter amount: ");
		sendto(s, messageWelcomePrompt, strlen(messageWelcomePrompt), 0, client, len);
		if ((readBytes=recvfrom(s, messageAmount, MAX_BUFFER_SIZE, 0, client, (socklen_t*)&len)) < 0)
	  {
	    printf("Read error!\n");
	    return -1;
	  }
	amount = atoi(messageAmount);
	converted = amount * 0.59;
	sprintf(messageout, "%fGBP",converted);
		}


//converting to btc
	else if( strncmp(messagein, "BTC", 3) == 0 ){

		sprintf(messageWelcomePrompt, "Please enter amount: ");
		sendto(s, messageWelcomePrompt, strlen(messageWelcomePrompt), 0, client, len);
		if ((readBytes=recvfrom(s, messageAmount, MAX_BUFFER_SIZE, 0, client, (socklen_t*)&len)) < 0)
	  {
	    printf("Read error!\n");
	    return -1;
	  }
	amount = atoi(messageAmount);
	converted = amount *0.000013;
	sprintf(messageout, "%fBTC",converted);
		}


//if the user enters unsupported currency
		else sprintf(messageout, "Unrecognized currency. Sorry!\n");

#ifdef DEBUG
	printf("Server sending back the message: \"%s\"\n", messageout);
#endif

	/* send the result message back to the client */
	sendto(s, messageout, strlen(messageout), 0, client, len);	

	bzero(messagein, MAX_BUFFER_SIZE);
	bzero(messageout, MAX_BUFFER_SIZE);
	bzero(messageAmount, MAX_BUFFER_SIZE);
	bzero(messageCurrency, MAX_BUFFER_SIZE);
	bzero(messageWelcomePrompt, MAX_BUFFER_SIZE);
      }

    close(s);
    return 0;
  }