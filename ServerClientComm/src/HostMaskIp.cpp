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

#include <sys/ioctl.h>
#include <net/if.h>
#include <ifaddrs.h>

#include <linux/if_link.h>
//#define _GNU_SOURCE     	/* To get defns of NI_MAXSERV and NI_MAXHOST */
#define MSG_SIZE 400        // Maximum size of the message

// Error Message
void error(const char *msg){
    perror(msg);
    exit(0);
}

// Various methods to get the IP address of host machine.
// Using ioctl() function
void HostIP_IOCTL();
// Using gethostbyname() function
void HostIP_GETHOSTBYNAME();
// Using getifaddrs() function
void HostIP_GETIFADDRS();

// main function
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
    
    // Get the last octate of the machine IP address
    HostIP_GETHOSTBYNAME();
	HostIP_IOCTL();
	HostIP_GETIFADDRS();

    // Get the last octate of the machine IP address
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

// Get host IP using gethostbyname() function
void HostIP_GETHOSTBYNAME(){
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
        std::cout << "HostInfo IP (gethostbyname): " << 
		inet_ntoa(*addrList[iIP]) << std::endl;
    }

    //Parses to get my identifier
    int myID[4] = {0};
    for(int iIP=0; addrList[iIP] != NULL; iIP++){
		char *myIDtoken = NULL;
        myIDtoken = strtok(inet_ntoa(*addrList[iIP]), ".");
		int i = 0;
		while(myIDtoken != NULL){
			myID[i] = atoi(myIDtoken);
			//std::cout << "Just for checking: " << myID[i] << std::endl;

            // Skip the loopback address
            if(myID[i] == 127){
				break;
			}
            myIDtoken = strtok(NULL, ".");
			i++;
        }
    }
    std::cout << "IP stored as integer: " << 
		myID[0] << "." << myID[1] << "." << 
		myID[2] << "." << myID[3] << std::endl;
}

// The following code is borrowed from -- 
// http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
// Get the host machine IP address from the host name using IOCTL()
void HostIP_IOCTL(){
	int fd;
	struct ifreq ifr;
	
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	/* I want to get an IPv4 IP address */
	ifr.ifr_addr.sa_family = AF_INET;
	
	/* I want IP address attached to "eth0" */
	//strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);
	strncpy(ifr.ifr_name, "enp0s31f6", IFNAMSIZ-1);
	
	ioctl(fd, SIOCGIFADDR, &ifr);
	
	close(fd);
	
	/* display result */
	std::cout << "Hostinfo IP (IOCTL): " << 
		inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr) << 
		std::endl;
}

// Get the host machine IP, netmask, broadcast IP, etc using getifaddrs()
void HostIP_GETIFADDRS(){
	// Structures to hold various network related information.
	struct ifaddrs *ifaddr, *ifa;
	int family, s, n;
	char host[NI_MAXHOST];
	
	if (getifaddrs(&ifaddr) == -1) {
	    perror("getifaddrs");
	    exit(EXIT_FAILURE);
	}
	
	/* Walk through linked list, maintaining head pointer so we can free list 
 	 * later */
	
	for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
	    if (ifa->ifa_addr == NULL)
	        continue;
	
	    family = ifa->ifa_addr->sa_family;
	
	    /* Display interface name and family (including symbolic form of the 
 		 * latter for the common families) */
	
	    printf("%-8s %s (%d)\n",
	           ifa->ifa_name,
	           (family == AF_PACKET) ? "AF_PACKET" :
	           (family == AF_INET) ? "AF_INET" :
	           (family == AF_INET6) ? "AF_INET6" : "???",
	           family);
	
	    /* For an AF_INET* interface address, display the address */
	
	    //if (family == AF_INET || family == AF_INET6) {
	    if (family == AF_INET) {
	        s = getnameinfo(ifa->ifa_addr,
	                (family == AF_INET) ? sizeof(struct sockaddr_in) :
	                                      sizeof(struct sockaddr_in6),
	                host, NI_MAXHOST,
	                NULL, 0, NI_NUMERICHOST);
	        if (s != 0) {
	            printf("getnameinfo() failed: %s\n", gai_strerror(s));
	            exit(EXIT_FAILURE);
	        }
	
	        std::cout << "\t\taddress: " << host << std::endl;

	        s = getnameinfo(ifa->ifa_broadaddr,
	                (family == AF_INET) ? sizeof(struct sockaddr_in) :
	                                      sizeof(struct sockaddr_in6),
	                host, NI_MAXHOST,
	                NULL, 0, NI_NUMERICHOST);
	        if (s != 0) {
	            printf("getnameinfo() failed: %s\n", gai_strerror(s));
	            exit(EXIT_FAILURE);
	        }
	        std::cout << "\t\tbraodcast: " << host << std::endl;
	
	
	    } else if (family == AF_PACKET && ifa->ifa_data != NULL) {
	        struct rtnl_link_stats *stats = reinterpret_cast<struct rtnl_link_stats *>(ifa->ifa_data);
	
	        printf("\t\ttx_packets = %10u; rx_packets = %10u\n"
	               "\t\ttx_bytes   = %10u; rx_bytes   = %10u\n",
	               stats->tx_packets, stats->rx_packets,
	               stats->tx_bytes, stats->rx_bytes);
	    }
	}
	
	freeifaddrs(ifaddr);
	exit(EXIT_SUCCESS);
}
