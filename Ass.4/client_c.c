#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

#define Max 255 //max is the size of the buffer, used while exchanging the message.

/*
this function prints the error and exits the code.
*/
void error(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc < 3 || atoi(argv[2]) < 1024)
        error("please pass the servers IP address and port no.( > 1024 )\n");
    
    // This structure makes it easy to reference elements of the socket address.
    struct sockaddr_in ServAdd;
    int portno = atoi(argv[2]);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    char buff[Max];

    if (sockfd < 0)
        error("unable to create tcp socket...\n");

    // This bzero function will be used a lot of times as it clears the buffer.
    bzero((char *)&ServAdd, sizeof(ServAdd));
    ServAdd.sin_family = AF_INET; //IPv4
    ServAdd.sin_port = htons(portno);
    ServAdd.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&ServAdd, sizeof(ServAdd)) < 0)
        error("failed to connect with the server...\n");
    //else
       // printf("TCP Connection Successfull...\n");

    int udpfd = socket(AF_INET, SOCK_DGRAM, 0); //udp socket is created.
    if (udpfd < 0)
        error("cannot create UDP Socket\n");

    //printf("changing from TCP to UDP Connection\n");
    if (write(sockfd, buff, sizeof(buff)) < 0)
        error("error in writing to ther server\n");

    bzero(buff, Max);
    socklen_t Servlen = sizeof(ServAdd);
    int byte_count = read(sockfd, buff, sizeof(buff));
    if (byte_count < 0)
        error("Server details is corrupted\n");
    //else
        //printf("type \"Bye\" for closing the connection\n\n");

    ServAdd.sin_port = htons(atoi(buff));
    close(sockfd);
    int i;
    /*
    Now everything is done, all the requirements has been setup now we wil
    just send messages and recieve meaasge from the server until connection 
    is closed by saying bye.
    */
    while (1)
    {
        bzero(buff, Max);
        printf("Client : ");
        fgets(buff, Max, stdin); //reading from the terminal what u entered.
        sendto(udpfd, (const char *)buff, strlen(buff), 0, (const struct sockaddr *)&ServAdd, sizeof(ServAdd));
        i = strncmp("Bye", buff, 3); //compares the starting 3 letters in the string buff with const string "Bye".
        if (i == 0)
            break;
        bzero(buff, Max);
        byte_count = recvfrom(udpfd, (char *)buff, Max, 0, (struct sockaddr *)&ServAdd, &Servlen);
        if (byte_count < 0)
            error("error in reading\n");
        buff[byte_count] = '\0';
        printf("Server : %s", buff);        
    }
    close(udpfd);
    return 0;
}
