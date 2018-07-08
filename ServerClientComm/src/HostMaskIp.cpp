// Here we are going to find the IP of the local machine, subnet mask and the 
// broadcast IP.

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MSG_SIZE 400        // Maximum size of the message

// Error Message
void error(const char *msg){
    perror(msg);
    exit(0);
}

// Hostname to IP
void HostnameToIP(){
    // Get the hostInfo IP address
    // -----------------------
    char name[MSG_SIZE];
    struct hostent *hostInfo;
    gethostname(name, sizeof name);
    if ((hostInfo = gethostbyname(name)) == NULL) {		// get the hostInfo info
        std::cout << "Error: can't get hostInfo name" << std::endl;
        exit(2);
    }
    std::cout << "Hostname: " << name << std::endl;
    std::cout << "Official Hostname: " << hostInfo->h_name << std::endl;

    struct in_addr **addrList;
    addrList = reinterpret_cast<struct in_addr **>(hostInfo->h_addr_list);
    for(int iIP=0; addrList[iIP] != NULL; iIP++){
        std::cout << "hostInfo IP: " << inet_ntoa(*addrList[iIP]) << std::endl;
    }

    //Parses to get my identifier
    char *myId;
    for(int iIP=0; addrList[iIP] != NULL; iIP++){
        myId = strtok(inet_ntoa(*addrList[iIP]), ".");
        for(int i=0; i<3; i++){
            myId = strtok(NULL, ".");
            // Skip the loopback address
            if(atoi(myId) == 127){
                continue;
            }
        }
    }
    int MyID = atoi(myId);
    std::cout << "Last octate of the IP - " << MyID << std::endl;

}

int main(int argc, char* argv[]){
    int sock, length, n;
    int boolval = 1;			// For a socket option
    socklen_t fromlen;
    struct sockaddr_in serverAddr;
    struct sockaddr_in anybodyAddr;
    char buffer[MSG_SIZE];		// To store received messages or messages to be sent.
    char ackBuffer[MSG_SIZE];	// Store the acknowledgement that to be returned

    if (argc < 2){
        printf("usage: %s port\n", argv[0]);
        exit(0);
    }

	// Creates socket. Connectionless.
    sock = socket(AF_INET, SOCK_DGRAM, 0); 
    if (sock < 0)
        error("Opening socket");

    // Set the options in the address structure. The address structure options
    // and the socket options must match.
    length = sizeof(serverAddr);			// length of address structure
    bzero(&serverAddr,length);				// sets all values to zero. memset() could be used
    serverAddr.sin_family = AF_INET;		// symbol constant for Internet domain
	
	// IP address of the machine on which the server is running
    serverAddr.sin_addr.s_addr = INADDR_ANY;		
    serverAddr.sin_port = htons(atoi(argv[1]));		// port number
    
    // Get the last octate of the machine ip address
    HostnameToIP();

    // Get the last octate of the machine ip address
    // binds the socket to the address of the hostInfo and the port number
    if (bind(sock, (struct sockaddr *)&serverAddr, length) < 0)
        error("binding");

    // change socket permissions to allow broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        printf("error setting socket options\n");
        exit(-1);
    }

    fromlen = sizeof(struct sockaddr_in);	// size of structure

    int sysReturnVal;
	return 0;
}
