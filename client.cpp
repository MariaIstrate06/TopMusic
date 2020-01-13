#include "pch.h"

int main (int argc, char *argv[])
{
    int sd; // socket descriptor
    struct sockaddr_in server; //socketul plm
    char msg[100]; // mesajul trimis
    if(argc!=3)
    {
        printf("[CLIENT] Waiting for <SERVER_ADDRESS>, <PORT>\n"); 
        return -1;
    }
    int port; 
    port = atoi(argv[2]); //converting port from input to int
    if((sd=socket(AF_INET, SOCK_STREAM,0))==-1)
    {
        perror("[CLIENT] socket() error.\n");
    }
    /* gunoaie */ 
    server.sin_family = AF_INET; 
    server.sin_addr.s_addr = inet_addr(argv[1]); 
    server.sin_port = htons(port); 

    if(connect(sd, (struct sockaddr *)&server, sizeof(sockaddr)) == -1){
        perror("[CLIENT] connect() error.\n"); 
    }
    close(sd); 
}