#include "pch.h"

#define PORT 3005
int nrusers; 
struct a_user{
    int index; 
    char username[100];
    char passwd[100]; 
    short int aoru; 
}userArray[20]; 
void my_users(); 
void print_datafromfile(); 
void my_users(); 
char * conv_addr(struct sockaddr_in); 
int VerifClient(int); 


//extern int errno; 
int main(){
    struct sockaddr_in server;	/* structurile pentru server si clienti */
    struct sockaddr_in from;
    fd_set readfds;		/* multimea descriptorilor de citire */
    fd_set actfds;		/* multimea descriptorilor activi */
    struct timeval tv;		/* structura de timp pentru select() */
    int sd, client;		/* descriptori de socket */
    int optval=1; 			/* optiune folosita pentru setsockopt()*/ 
    int fd;			/* descriptor folosit pentru parcurgerea listelor de descriptori */
    int nfds;			/* numarul maxim de descriptori */
    unsigned int len;			/* lungimea structurii sockaddr_in */
    
    if((sd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("[SERVER] socket() Error.\n");
    }
    /* gunoaie legate de server pe care oricum nu le intelege nici dracu */ 
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
    bzero(&server, sizeof(server)); 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    server.sin_port = htons (PORT);

    /* bind - atasarea socketului */ 

    if(bind(sd,(struct sockaddr *) &server, sizeof(struct sockaddr)) == -1){
        perror("[SERVER] bind() error.\n"); 
    }
    /*listen - sunt clienti? */
    if(listen(sd, 10)==-1){
        perror("[SERVER] listen() error.\n"); 
    }

    /*alte gunoaie pe care nimeni nu le intelege*/

    FD_ZERO(&actfds); // multimea e vida initial
    FD_SET(sd, &actfds); // includem socketul creat in multime
    tv.tv_sec = 1; //cica e un fel de wait 
    tv.tv_usec = 0; 
    nfds = sd; // valoare maxima a descriptorilor 

    printf("[SERVER] Waiting for connection at port. %d\n", PORT); 
    fflush(stdout); 
    
    while(1){
        bcopy((char*) &actfds, (char*)&readfds, sizeof(readfds)); //ajustam multimea descriptorilor activi
        //selects
        if(select(nfds+1, &readfds,NULL,NULL,&tv)<0){
            perror("[SERVER] select() error.\n");
            return errno; 
        }
        //is socket ready?
        if(FD_ISSET(sd,&readfds)){
            //pregatim o structura client
            len = sizeof(from); 
            bzero(&from, sizeof(from)); 
            //a venit un client, asteptam conexiunea
            client = accept (sd, (struct sockaddr *) &from, &len);
            if(client<0){
                perror("[SERVER] accept() error.\n");
                continue; 
            }
            if (nfds < client)
                nfds = client; 
            //adaugam acest socket in lista de descriptori activi
            FD_SET(client, &actfds); 
            printf("[SERVER] Clientul cu descriptorul %d, de la adresa %s s-a conectat.\n", client, conv_addr(from)); 
            fflush(stdout);
            
        }
        for (fd = 0; fd <= nfds; fd++)	/* parcurgem multimea de descriptori */
	    {
            /* este un socket de citire pregatit? */
            if (fd != sd && FD_ISSET (fd, &readfds))
            {	char isLogged[5]; 
                if (VerifClient(fd))
                {
                    printf ("[server] S-a deconectat clientul cu descriptorul %d.\n",fd);
                    fflush (stdout);
                    close (fd);		/* inchidem conexiunea cu clientul */
                    FD_CLR (fd, &actfds);/* scoatem si din multime */
                    strcpy(isLogged,"YES"); 
                }
                else
                {
                    strcpy(isLogged, "NO"); 
                }
                write(sd, isLogged, 256); 
	        }
	    }			
    }
}

int VerifClient(int fd){
    char buf1[50], buf2[50]; 
    int ok = 0; 
    read(fd, buf1,50);
    read(fd, buf2, 50); 
    my_users();
    for(int i =1;i<=nrusers;i++){
        if(strcmp(userArray[i].username, buf1)==0)
            if(strcmp(userArray[i].passwd, buf2)==0){
                printf("correct pass\n");
                write(fd, "YES", 256); 
                ok=1; 
            }
    }
    if(!ok){
        printf("wrong pass\n"); 
        write(fd, "NO", 256);
        return 0;
    }
    return 1; 
}
void print_datafromfile(){
    my_users(); 
    for(int j = 1; j<=nrusers; j++)
        printf("User cu nr %d, username %s, password %s, administrator right = %d\n",
               userArray[j].index, userArray[j].username, userArray[j].passwd, userArray[j].aoru);

}

void my_users() {
    FILE *f = fopen("users", "r");
    char word[100]; int i = 0; 
    while(fscanf(f,"%s",word)!=EOF){
        fflush(stdout); 
        if(strstr(word, "NR:"))
           userArray[++i].index = i; 
        if(strstr(word, "NAME:"))
            strcpy(userArray[i].username, word+5); 
        if(strstr(word,"PASS:"))
            strcpy(userArray[i].passwd, word+5); 
        if(strstr(word, "A/U:"))
            userArray[i].aoru=atoi(word+4);  
          
    }
    nrusers = i; 
    fclose(f); 
}


char * conv_addr(struct sockaddr_in address) 
{
  static char str[25];
  char port[7];

  /* adresa IP a clientului */
  strcpy (str, inet_ntoa (address.sin_addr));	
  /* portul utilizat de client */
  bzero (port, 7);
  sprintf (port, ":%d", ntohs (address.sin_port));	
  strcat (str, port);
  return (str);
}
