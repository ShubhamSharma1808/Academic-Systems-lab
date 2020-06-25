#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

#define Max 255   //max is the size of the buffer used to pass the message
#define Clients 5 //no. of concurrent clients at most, will be used in the listen function.


void error(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd, newfd;
    if (argc < 2 || atoi(argv[1]) < 1024)
        error("please pass the port no.( > 1024 )...\n");

    int portno = atoi(argv[1]);
    //socket function returns the socket file descriptor, which is stored in sockfd
    // it returns 0 if socket is successfully created, otherwise -1.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("unable to create tcp socket...\n");

    // This structure makes it easy to reference elements of the socket address.
    struct sockaddr_in ServAdd;
    // This bzero function will be used a lot of times as it clears the buffer.
    bzero((char *)&ServAdd, sizeof(ServAdd));
    ServAdd.sin_family = AF_INET;	// ipv4 
    ServAdd.sin_port = htons(portno);
    ServAdd.sin_addr.s_addr = INADDR_ANY; //INADDR_ANY to bind to the local IP address.

    //we use BIND() Function to associate the socket with a port on our local machine.
    if (bind(sockfd, (struct sockaddr *)&ServAdd, sizeof ServAdd) < 0)
        error("unable to bind port with the tcp socket...\n");

    if (listen(sockfd, Clients) < 0)
        error("error in the listen function...\n");

    struct sockaddr_in cliadd;
    socklen_t clilen;
    int pid, count=0;
    /*
    there is a while loop below because the server is not limited to the single client,
    we are running concurrently for multiple clients. the below loop ends only when
    the limit of the connections has been reached. else it waits for the client forever.
    */
    while (1)
    {
        clilen = sizeof(cliadd);
        newfd = accept(sockfd, (struct sockaddr *)&cliadd, &clilen);
        if (newfd < 0)
            error("error while accepting the tcp connection...\n");
        //else
            //printf("tcp Connection Successfull with new client...\n");
        
        
        /*
            now we fork the process and creates a child, we do this so that the
            socket 'sockfd' at the parent process becomes available for other clients.

            So, the code in the if condition is run by the child only, and the parent 
            clears the new socket created through accept and gets ready for accepting
            th connections from other clients.  
        */        
        pid = fork();
        count++;
        if (pid == 0) //for child process
        {            
            close(sockfd); //closes the socket which was created in the parent process for further connections
            int udpfd, byte_count, i;
            char buff[Max];
            udpfd = socket(AF_INET, SOCK_DGRAM, 0);     //udp socket created.
            if (udpfd < 0)
                error("unable to create udp socket...\n");

            /*
            the formula written below can be changed in a no. of ways, i just wrote in this way
            as by adding in the previous portno itself the new portno will not be the same as 
            that of the previous one.
            */
            portno = 2*portno + count; 
            /*
             below all three assignments were not reuired, only updating the port no would have
             worked but still to avoid unknown errors i kept this way. 
            */
            bzero((char *)&ServAdd, sizeof(ServAdd));
            ServAdd.sin_family = AF_INET;
            ServAdd.sin_port = htons(portno);
            ServAdd.sin_addr.s_addr = INADDR_ANY;	//loopback address

            if (bind(udpfd, (struct sockaddr *)&ServAdd, sizeof(ServAdd)) < 0)
                error("unable to bind udp socket...\n");
            
            // A request message from client for the udp port no. is read below.
            bzero(buff, Max);
            byte_count = read(newfd, buff, sizeof(buff));   
            if (byte_count < 0)
                error("error in reading from the client\n");
            //else
                //printf("changing from TCP to UDP Connection( pid - %d )\n", pid % Clients + 1);

            bzero(buff, Max);
            sprintf(buff,"%d",portno);  //it converts the int to string which will be stored in a buff.

            //printf("sending UDP port no. to client...\n");

            if (write(newfd, buff, sizeof(buff)) < 0)
                error("error in writing to ther server\n");

            close(newfd); //closes the tcp connection as the udp connection has been setup.
            /*
                now everything is done the TCP connection has been changed to UDP connection and,
                in the below while loop, only the messages are been exchaned between client and server.
                when the server receives the Bye message from the client the connection ends.
            */
            while (1)
            {
                bzero(buff, Max);
                // receiving the message from the client.
                byte_count = recvfrom(udpfd, buff, sizeof buff, 0, (struct sockaddr *)&cliadd, &clilen);
                if (byte_count < 0)
                {
                    printf("error while reading from the client %d", count);
                    break;
                }
                printf("Client %d : %s", count, buff);
                i = strncmp("Bye", buff, 3); //compares the starting 3 letters in the string buff with const string "Bye".
                if (i == 0) break;

                bzero(buff, Max);
                printf("Server to C %d: ",count);
                fgets(buff, Max, stdin);    //reading from the terminal what u entered.
                // sending the message to the client.
                sendto(udpfd, (const char *)buff, strlen(buff), 0, (const struct sockaddr *)&cliadd, clilen);
                
            }
            printf("Connection closed with client %d\n", count);
            close(udpfd);   //closing the udp connection.
            return 0;
        }
        close(newfd);   //it closes the tcp socket in the parent process.
        if(count == Clients)
            break;
    }
    close(sockfd);  //closing the parent socket.
    return 0;
}
