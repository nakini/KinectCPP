#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define MSG_SIZE 400			// message size

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sock, length, n;
    int boolval = 1;			// for a socket option
    socklen_t fromlen;
    struct sockaddr_in server;
    struct sockaddr_in addr;
    char buffer[MSG_SIZE];	// to store received messages or messages to be sent.
    char ackBuffer[MSG_SIZE];	// Store the acknowledgement that to be returned

    //   if (argc < 2)
    //   {
    //	  printf("usage: %s port\n", argv[0]);
    //      exit(0);
    //   }

    sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if (sock < 0)
        error("Opening socket");

    length = sizeof(server);			// length of structure
    bzero(&server,length);			// sets all values to zero. memset() could be used
    server.sin_family = AF_INET;		// symbol constant for Internet domain
    server.sin_addr.s_addr = INADDR_ANY;		// IP address of the machine on which
    // the server is running
    //   server.sin_port = htons(atoi(argv[1]));	// port number
    server.sin_port = htons(5000);	// port number

    // Get the host IP address
    // -----------------------
    char name[MSG_SIZE];
    struct hostent *host;
    struct in_addr **addr_list;
    gethostname(name, sizeof name);
    if ((host = gethostbyname(name)) == NULL) { // get the host info
        printf("Error: can't get host name");
        exit(2);
    }
    printf("Hostname Got: %s\n", name);

    addr_list = (struct in_addr **) host->h_addr_list;
    printf("Host IP: %s\n", inet_ntoa(*addr_list[0]));

    //Parses to get my identifier
    char *my_id = strtok(inet_ntoa(*addr_list[0]), ".");
    int i;
    for(i=0; i<3; i++)
    {
        my_id = strtok(NULL, ".");
    }
    int MyID = atoi(my_id);
    printf("Last octate of the IP - %d\n", MyID);

    // binds the socket to the address of the host and the port number
    if (bind(sock, (struct sockaddr *)&server, length) < 0)
        error("binding");

    // change socket permissions to allow broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        printf("error setting socket options\n");
        exit(-1);
    }

    fromlen = sizeof(struct sockaddr_in);	// size of structure

    int sysReturnVal;
    while (1)
    {
        // bzero: to "clean up" the buffer. The messages aren't always the same length...
        bzero(buffer,MSG_SIZE);		// sets all values to zero. memset() could be used

        printf("Waiting to receive from peer.\n");
        // receive from a client
        n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&addr, &fromlen);
        if (n < 0)
            error("recvfrom");

        printf("Received a datagram. It says: %s", buffer);

        if (strncasecmp(buffer, "sudo date", 9) == 0){
            sysReturnVal = system(buffer);
            bzero(ackBuffer, MSG_SIZE);
            sprintf(ackBuffer, "System return for setting date and time is %d", sysReturnVal);
            n = sendto(sock, ackBuffer, MSG_SIZE, 0, (struct sockaddr *)&addr, fromlen);
            if (n < 0){
                error("sendto");
            }
        }else if (strncasecmp(buffer, "Kinect", 6) == 0){
            //system("ls -l");
            sysReturnVal = system("/home/ubuntu/Documents/libfreenect2/build/bin/Protonect -t \"/media/SataHDD/Data/\" &");
            bzero(ackBuffer, MSG_SIZE);
            sprintf(ackBuffer, "System return is %d", sysReturnVal);
            n = sendto(sock, ackBuffer, MSG_SIZE, 0, (struct sockaddr *)&addr, fromlen);
            if (n < 0){
                error("sendto");
            }
        }else if(strncasecmp(buffer, "Kill", 4) == 0){
            sysReturnVal = system("killall Protonect");
            bzero(ackBuffer, MSG_SIZE);
            sprintf(ackBuffer, "System return is %d", sysReturnVal);
            n = sendto(sock, ackBuffer, MSG_SIZE, 0, (struct sockaddr *)&addr, fromlen);
            if (n < 0){
                error("sendto");
            }
        }else{
            //       addr.sin_addr.s_addr = inet_addr("192.168.1.255");	// broadcast address
            bzero(ackBuffer, MSG_SIZE);
            sprintf(ackBuffer, "Got this message: %s\n", buffer);
            n = sendto(sock, ackBuffer, MSG_SIZE, 0, (struct sockaddr *)&addr, fromlen);
            if (n  < 0)
                error("sendto");
        }
        printf("Acknowledgement sent\n");
    }

    return 0;
}

