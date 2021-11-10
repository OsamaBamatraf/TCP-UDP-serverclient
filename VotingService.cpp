/* Osama Bamatraf
CPSC 441 assignment 2
Simple voting microservice based on Carey's wordlen-server.c code

User can vote, view candidates list and view results

 */


/* Include files */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
/* Manifest constants */
#define MAX_BUFFER_SIZE 100
#define PORT 2119

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
	char firstcandidate[MAX_BUFFER_SIZE];
	char secondcandidate [MAX_BUFFER_SIZE];
	char thirdcandidate[MAX_BUFFER_SIZE];
	char fourthcandidate[MAX_BUFFER_SIZE];
	char votein[MAX_BUFFER_SIZE];
	char messageWelcomePrompt [MAX_BUFFER_SIZE];
    int readBytes;
	int voteid;
	float converted;
	int firstvote = 10;
	int secondtvote = 10;
	int thirdvote = 10;
	int fourthvote = 10;
	int voted = 0;

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
    fprintf(stderr, "Welcome! I am the Voting server!!\n");
    printf("server now listening on UDP port %d...\n", PORT);

		/* source: https://www.includehelp.com/c-programs/get-the-current-utc-time.aspx */

//setting up time structures in order to make voting based on UTC time
	time_t tmi;
    struct tm* utcTime;

    time(&tmi);
    utcTime = gmtime(&tmi);
//clearing out message buffers
	bzero(messagein, MAX_BUFFER_SIZE);
	bzero(messageout, MAX_BUFFER_SIZE);
	bzero(firstcandidate, MAX_BUFFER_SIZE);
	bzero(secondcandidate, MAX_BUFFER_SIZE);
	bzero(thirdcandidate, MAX_BUFFER_SIZE);
	bzero(fourthcandidate, MAX_BUFFER_SIZE);
	bzero(votein, MAX_BUFFER_SIZE);
	bzero(messageWelcomePrompt, MAX_BUFFER_SIZE);
    /* big loop, looking for incoming messages from clients */
    for( ; ; )
      {
	/* clear out message buffers to be safe */
	bzero(messagein, MAX_BUFFER_SIZE);
	bzero(messageout, MAX_BUFFER_SIZE);
	bzero(firstcandidate, MAX_BUFFER_SIZE);
	bzero(secondcandidate, MAX_BUFFER_SIZE);
	bzero(thirdcandidate, MAX_BUFFER_SIZE);
	bzero(fourthcandidate, MAX_BUFFER_SIZE);
	bzero(votein, MAX_BUFFER_SIZE);
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

	/* checking for commands from user*/
	if( strncmp(messagein, "VOTE", 4) == 0 ){


		sprintf(messageWelcomePrompt, "Please enter candidate ID: ");
		sendto(s, messageWelcomePrompt, strlen(messageWelcomePrompt), 0, client, len);		 // prompting for candidate ID
      
		if ((readBytes=recvfrom(s, votein, MAX_BUFFER_SIZE, 0, client, (socklen_t*)&len)) < 0)  //recieving candidate ID from user
	  {
	    printf("Read error!\n");
	    return -1;
	  }
	voteid = atoi(votein);
	voteid = voteid/4;

	if ((voteid == 1) && ((utcTime->tm_hour) < 15)){
		firstvote ++;
		sprintf(messageout, "Vote successful!");
		voted = 1;
		}
	else if ((voteid == 2) && ((utcTime->tm_hour) < 15)){
		secondtvote++;
		sprintf(messageout, "Vote successful!");
		voted = 1;
		}
	else if ((voteid == 3) && ((utcTime->tm_hour) < 15)){
		secondtvote++;
		sprintf(messageout, "Vote successful!");
		voted = 1;
		}
	else if ((voteid == 4) && ((utcTime->tm_hour) < 15)){
		secondtvote++;
		sprintf(messageout, "Vote successful!");
		voted = 1;
		}
	else {
		sprintf(messageout, "Ensure ID is valid, and voting before 15:00 UTC");
		}
		

		sendto(s, messageout, strlen(messageout), 0, client, len);

	
		}

		//giving user candidate list
	else if( strncmp(messagein, "CANDIDATES", 3) == 0 ){

		sprintf(messageWelcomePrompt, "Candidates: Bob McBob(ID= 1), John Notsmith (ID= 2), John Verysmith (ID= 3), Smith John (ID= 4).");
		sendto(s, messageWelcomePrompt, strlen(messageWelcomePrompt), 0, client, len);
		}

	//giving user results
	else if( strncmp(messagein, "RESULTS", 3) == 0 ){


		if ((voted = 1) && ((utcTime->tm_hour) > 15)){   //checking if user has voted alread and if the time is after 15:00 UTC
		sprintf(messageWelcomePrompt, "Results: McBob= %d, Notsmith= %d, Verysmith= %d, John= %d", firstvote, secondtvote, thirdvote, fourthvote);
		sendto(s, messageWelcomePrompt, strlen(messageWelcomePrompt), 0, client, len);


			}
		else {

			sprintf(messageout, "You can only check results after you've voted and after 15:00 UTC.");
			sendto(s, messageout, strlen(messageout), 0, client, len);

			}
		}

	
		else {sprintf(messageout, "Unrecognized command. Sorry!\n");
			sendto(s, messageout, strlen(messageout), 0, client, len);
		}

#ifdef DEBUG
	printf("Server sending back the message: \"%s\"\n", messageout);
#endif
	

	bzero(messagein, MAX_BUFFER_SIZE);
	bzero(messageout, MAX_BUFFER_SIZE);
	bzero(firstcandidate, MAX_BUFFER_SIZE);
	bzero(secondcandidate, MAX_BUFFER_SIZE);
	bzero(thirdcandidate, MAX_BUFFER_SIZE);
	bzero(fourthcandidate, MAX_BUFFER_SIZE);
	bzero(votein, MAX_BUFFER_SIZE);
	bzero(messageWelcomePrompt, MAX_BUFFER_SIZE);	
      }

    close(s);
    return 0;
  }