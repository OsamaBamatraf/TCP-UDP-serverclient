# TCP-UDP-serverclient
How to run:

On a linux machine do the following
1- ssh into csx3.cpsc.ucalgary.ca
2- navigate to the directory where the cpp files are
3- compile everything using g++
4- run the three micro-servers first (CurrencyConversionService.cpp, TranslationService.cpp, VotingService.cpp)
5- run IndirectionServer.cpp
6- run TCPclient.cpp
7- Enter commands using the TCPclient interface

Testing: 

- was done by sshing into university servers
- used debugging messages to figure out what was going wrong

What’s not so good:
- after a user receives the final response for a microservice, they must connect to it again by typing the appropriate command (Example: CONVERT).

What the translation service supports:

- translating five English words (hello, give, me, good, grade) into english.
- all other required functionality based on assignment specs

What the currency conversion service supports:
- converting from CAD into GBP, EUR, USD, BTC
- all other required functionality based on assignment specs

what the voting service supports:
- voting allowed based on UTC time
- viewing results allowed based on UTC time and whether user has already votes
- decrypting received voteID
- all other required functionality based on assignment specs

what the indirection server supports:
- facilitating communication between client and the three udp micro-servers.
- encrypting vote ID for voting service

What the client supports:
- communicating with indirection server
