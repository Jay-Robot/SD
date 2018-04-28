#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>

pthread_t listener, writer;

void error(char *msg)
{
    perror(msg);
    exit(0);
}


void *receiveMessage(void *socket){
    int sockfd, n, i;
    char buffer[256];

    sockfd = *(int *)socket;
    bzero(buffer, 256);

    while (1){
        bzero(buffer,256);
		n = read(sockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("Message received: %s\n",buffer);

    }
    pthread_cancel(writer);
    return NULL;
}

void *sendMessage (void *socket){
    char buffer[256];
    int sockfd, n;
    
    sockfd = *(int *)socket;
    bzero(buffer,256);
    
    while (strcmp(buffer, "bye\n") != 0) {
        printf("Please enter the message: ");
        
        bzero(buffer,256);
        fgets(buffer,255,stdin);

        n = write(sockfd,buffer, strlen(buffer)-1);
        if (n < 0) error("ERROR reading from socket");
    }
    
    pthread_cancel(listener);
    return NULL;
}


int initSocket(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
    return sockfd
}


void doConnect(int sockfd, int portno, struct hostent *server) {
    struct sockaddr_in serv_addr;
    
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
}

int main(int argc, char *argv[]){
    
    int sockfd, portno, n;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    //inicializa socket
    sockfd = initSocket();
    
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    //Realiza conexão com o server
    doConnect(sockfd, atoi(argv[2]), server)
    
    
    pthread_create(&listener, NULL, receiveMessage, &sockfd);
    pthread_create(&writer, NULL, sendMessage, &sockfd);
    
    pthread_join(listener, NULL);
    pthread_join(writer, NULL);
    
    close(sockfd);
   
    return 0;

}
