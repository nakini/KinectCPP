#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MSG_SIZE 400			// Message size
int n, sock;
unsigned int length;
struct sockaddr_in anybody, from;
char buffer[MSG_SIZE];			// To store received messages or messages to be sent.

// Error Message
// -------------
void error(const char *msg)
{
    perror(msg);
    exit(0);
}

// Thread to receive all messages
// ------------------------------
void *receiveSignals(void *ptr){
    while(1){
        // bzero: to "clean up" the buffer.
        bzero(buffer,MSG_SIZE);
        // receive message
        n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
        if (n < 0)
            error("recvfrom");

        printf("Message from %s is: %s\n", inet_ntoa(from.sin_addr), buffer);
    }
    pthread_exit(0);
}

// Display command line usage
// --------------------------
void displayMessage(){
    printf("\nThe messages should either on of the following\n");
    printf("\tTest -- Test established link with all machines\n");
    printf("\t&# Test -- Test with individual machine with last ip octate as #\n");
    printf("\tKinect/Kill -- Turn ON/OFF all kinects\n");
    printf("\t&# Kinect/Kill -- Turn ON/OFF individual kinect connected to machine "
           "ip(last octate) #\n\n");
}

// Main program
// ------------
int main(int argc, char *argv[]){
    int boolval = 1;			// for a socket option

    //    if (argc != 2)
    //    {
    //        printf("usage: %s port\n", argv[0]);
    //        exit(1);
    //    }

    sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if (sock < 0)
        error("socket");

    // change socket permissions to allow broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        printf("error setting socket options\n");
        exit(-1);
    }

    anybody.sin_family = AF_INET;							// Symbol constant for Internet domain
    //    anybody.sin_port = htons(atoi(argv[1]));		// Port field
    anybody.sin_port = htons(5000);						// Port field
    anybody.sin_addr.s_addr = inet_addr("192.168.1.255");		// Broadcast address

    length = sizeof(struct sockaddr_in);		// size of structure

    /* This variable is our reference to the second thread */
    pthread_t sigThID;

    /* Create a second thread to receive messages from peers */
    if(pthread_create(&sigThID, NULL, receiveSignals, NULL)){
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    do
    {
        // bzero: to "clean up" the buffer.
        bzero(buffer,MSG_SIZE);
        printf("Please enter the message (q to exit):\n");
        fgets(buffer,MSG_SIZE-1,stdin); // MSG_SIZE-1 because a null character is added

        if (buffer[0] != 'q'){
            if (buffer[0] == '&'){								// Send to Individual IP
                // Split strings into pieces (tokens) using strtok
                char *anybodyIP = NULL;
                anybodyIP = strtok(buffer, "&");
                anybodyIP = strtok(anybodyIP, " ");		// Get the IP
                char *msg = strtok(NULL, " ");			// Get the message like Start/Stop
                msg = strtok(msg, "\n");
                char anybodyIP_Full[15];
                printf("The value of the last IP is: %s\n", anybodyIP);
                sprintf(anybodyIP_Full, "192.168.1.%d", atoi(anybodyIP)%256);
                printf("You will send the message \"%s\" to the IP: %s\n\n", msg, anybodyIP_Full);

                // Send the message
                anybody.sin_addr.s_addr = inet_addr(anybodyIP_Full);	// Unicast address
                n = sendto(sock, msg, strlen(msg), 0,
                           (const struct sockaddr *)&anybody,length);
                if (n < 0)
                    error("Sendto");
            }else if((strncasecmp(buffer, "Kill", 4) == 0) ||
                     (strncasecmp(buffer, "Kinect", 6) == 0)||
                     (strncasecmp(buffer, "Test", 4) == 0)){	// Send to All IPs
                printf("You will send the message \"%s\" to all the IPs\n\n",
                       strtok(buffer, "\n"));
                // Change the address to brodcast.
                anybody.sin_addr.s_addr = inet_addr("192.168.1.255");	// Broadcast address
                n = sendto(sock, buffer, strlen(buffer), 0,
                           (const struct sockaddr *)&anybody,length);
                if (n < 0)
                    error("Sendto");
            }else{
                // Set it to the broadcast IP as it is the default.
                anybody.sin_addr.s_addr = inet_addr("192.168.1.255");	// Broadcast address
                displayMessage();
            }
        }
    } while (buffer[0] != 'q');

    close(sock);						// Close Socket.
    return 0;
}
