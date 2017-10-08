
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

#define MSG_SIZE 400			// message size
int sock, n;
unsigned int length;
struct sockaddr_in anybody, from;
char buffer[MSG_SIZE];	// to store received messages or messages to be sent.

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void *receiveSignals(void *ptr){
    while(1){
        // receive message
        n = recvfrom(sock, buffer, MSG_SIZE, 0, (struct sockaddr *)&from, &length);
        if (n < 0)
            error("recvfrom");

        printf("Received something: %s\n", buffer);
    }
    pthread_exit(0);
}

int main(int argc, char *argv[])
{ 
    int boolval = 1;			// for a socket option

    //if (argc != 2)
    //{
    //    printf("usage: %s port\n", argv[0]);
    //    exit(1);
    //}

    sock = socket(AF_INET, SOCK_DGRAM, 0); // Creates socket. Connectionless.
    if (sock < 0)
        error("socket");

    // change socket permissions to allow broadcast
    if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &boolval, sizeof(boolval)) < 0)
    {
        printf("error setting socket options\n");
        exit(-1);
    }

    anybody.sin_family = AF_INET;		// symbol constant for Internet domain
    //anybody.sin_port = htons(atoi(argv[1]));				// port field
    anybody.sin_port = htons(5432);				// port field
    //   anybody.sin_addr.s_addr = inet_addr("10.3.52.255");	// broadcast address
    //   anybody.sin_addr.s_addr = inet_addr("127.0.0.1");	// broadcast address
    anybody.sin_addr.s_addr = inet_addr("192.168.1.255");	// broadcast address

    length = sizeof(struct sockaddr_in);		// size of structure

    /* this variable is our reference to the second thread */
    pthread_t sigThID;

    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&sigThID, NULL, receiveSignals, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    do
    {
        // bzero: to "clean up" the buffer. The messages aren't always the same length...
        bzero(buffer,MSG_SIZE);		// sets all values to zero. memset() could be used
        printf("Please enter the message (q to exit): ");
        fgets(buffer,MSG_SIZE-1,stdin); // MSG_SIZE-1 because a null character is added

        if (buffer[0] != 'q')
        {
            // send message to anyone there...
            n = sendto(sock, buffer, strlen(buffer), 0,
                       (const struct sockaddr *)&anybody,length);
            if (n < 0)
                error("Sendto");


        }
    } while (buffer[0] != 'q');

    close(sock);						// close socket.
    return 0;
}
