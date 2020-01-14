#include "pch.h"

#define PORT 3005
int nrusers; 
int votes[100];
char genres[10][10]={"Rock", "Indie", "HipHop", "Rap","EDM"};
struct a_user{
    int index; 
    char username[100];
    char passwd[100]; 
    short int aoru; 
}userArray[20]; 
struct a_song{
    char name[256], artist[256], description[256], link[256], genre[256];
    int votes;
    //int indexOfGenre;
}songArray[100]; int nrSongs;
void my_users(); 
void createSongList();
void printSongList(); 
void print_datafromfile(); 
void my_users(); 
void saveSongVotes();
char * conv_addr(struct sockaddr_in); 
int VerifClient(int); 
int userOption(int);


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
    
    createSongList();

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
    //createSongList();
    //printSongList();
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
                    saveSongVotes();
                    fflush (stdout);
                    close (fd);		/* inchidem conexiunea cu clientul */
                    FD_CLR (fd, &actfds);/* scoatem si din multime */
                    strcpy(isLogged,"YES"); 
                }
                else
                {
                    strcpy(isLogged, "NO"); 
                }
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
    return userOption(fd);  
}
int userOption(int cd){
    char command[256];
    while(1){
       // printf("sex\n");
        fflush(stdout);
        
        if(read(cd, command, 256) <= 0)
            return 1; 
        
        printf("%s\n", command);
        fflush(stdout);
        if(strcmp(command,"refresh")==0){
            write(cd, &nrSongs, 4); 
            printf("No songs %d\n", nrSongs);
            fflush(stdout);
            for(int i = 0; i<nrSongs;i++){
                write(cd, songArray[i].name, 256);
                write(cd, songArray[i].artist, 256);
                write(cd, songArray[i].genre, 256);
                write(cd, songArray[i].description, 256);
                write(cd, songArray[i].link, 256);
                write(cd, &songArray[i].votes, 4);
            }

        }
        if(strcmp(command, "vote") == 0) 
        {
            char votedSongName[256];
            read(cd, votedSongName, 256);

            for(int i = 0; i < nrSongs; i++)
            {
                if( strcmp(votedSongName, songArray[i].name ) == 0 )
                {
                    songArray[i].votes++;
                    break;
                }
            }
        }
        if(strcmp(command, "addSong") == 0)
        {
            a_song newSong;
            read(cd, newSong.name, 256);
            read(cd, newSong.artist, 256);
            read(cd, newSong.genre, 256);
            read(cd, newSong.description, 256);
            read(cd, newSong.link, 256);
            newSong.votes = 0;

            FILE *songsFile = fopen("Songs.txt", "a");
            fprintf(songsFile, "%s,%s,%s,%s,%s,%d\n", 
                newSong.name, newSong.artist, newSong.genre,
                newSong.description,newSong.link, newSong.votes);
            
            songArray[nrSongs++] = newSong;

            fclose(songsFile);
        }
    }
    
}

void saveSongVotes()
{
    char songLine[256];
    FILE* myFile = fopen("Songs.txt", "w");
    for(int i = 0; i < nrSongs; i++)
    {
        fprintf(myFile, "%s,%s,%s,%s,%s,%d\n", songArray[i].name,
                songArray[i].artist, songArray[i].genre,
                songArray[i].description, songArray[i].link,
                songArray[i].votes);
    }
    fflush(myFile);
    fclose(myFile);
}

void createSongList( ){
    char songLine[256];
    FILE* myFile = fopen("Songs.txt", "r"); 
    while(!feof(myFile)){
        memset(songLine, 0, 256);
        fgets(songLine, 256, myFile);
       // printf("%s", songLine);
       // fflush(stdout);
        char *p; 
        p=strtok(songLine,",\n");
        if(p==NULL)
            break; 
        strcpy(songArray[nrSongs].name, p); 
        p=strtok(NULL,",\n");
        strcpy(songArray[nrSongs].artist, p);
        p=strtok(NULL,",\n");
        strcpy(songArray[nrSongs].genre, p);
        p=strtok(NULL,",\n");
        strcpy(songArray[nrSongs].description, p);
        p=strtok(NULL,",\n");
        strcpy(songArray[nrSongs].link, p);
        p=strtok(NULL, ",\n");
        songArray[nrSongs].votes = atoi(p);
         nrSongs++; 
    }
    fclose(myFile); 
}
void printSongList(){
    for(int i = 0; i<nrSongs;i++)
        printf("%s\n%s\n%s\n%s\n%s\n\n\n",songArray[i].name,songArray[i].artist, 
        songArray[i].genre, songArray[i].description, songArray[i].link);
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
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
