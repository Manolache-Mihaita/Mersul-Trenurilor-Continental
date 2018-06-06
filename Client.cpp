//
// Created by ubuntu on 12/6/17.
//

#include <cstring>
#include <unistd.h>
#include <netdb.h>
#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#define Cmd_max 5
using namespace std;

char* stations[] {
        "Iasi 1", "Iasi 2","Bucuresti 1","Bucuresti 2", "Ploiesti 1","Ploiesti 2", "Ploiesti 3"
};
char* commands[]{
        "Arrival Train",
        "Departure Train",
        "Delay Train",
        "Station Arrival",
        "Station Departure"
};
short print_commands(){
    std::cout<<"Chose the index then press enter:";
    std::cout<<std::endl;
    for (int index=0;index<Cmd_max;++index){
        std::cout<<index<<")"<<commands[index]<<std::endl;
    }
    short p;
    std::cin>>p;
    if (p>=Cmd_max)exit (-1);
    return p;
}
char* Threat_first_command_part(int cmd_index,char* cmd){
    char* returned=new char;
    if (cmd_index==0 ||cmd_index==1){
        std::cout<<"Introduce the name of the Train: ";
        char Train[20];// [20];
        std::cin>>Train;
        std::cout<<Train;
        strcat(returned,cmd);
        std::cout<<"OK";
        strcat(returned,"/");
        std::cout<<"OK";
        strcat(returned,Train);
        strcat (returned,"/");
        //strcat (returned,NULL);
        std::cout<<"OK";
        return returned;
    }
    if(cmd_index==2){
        std::cout<<"Introduce the name of the Train: ";
        char Train [20];
        std::cin>>Train;

        std::cout<<std::endl;
        std::cout<<"Introduce the station: ";
        char Station[20];
        std::cin>>Station;
        std::cout<<std::endl;
        std::cout<<"Introduce the arrival delay: ";
        char Arr_delay[20];
        std::cin>>Arr_delay;
        std::cout<<std::endl;
        std::cout<<"Introduce the departure delay: ";
        char Dep_delay[20];
        std::cin>>Dep_delay;
        std::cout<<std::endl;
        strcat (returned,cmd);
        strcat (returned,"/");
        strcat (returned,Train);
        strcat (returned,"/");
        strcat (returned,Station);
        strcat (returned,"/");
        strcat (returned,Arr_delay);
        strcat (returned,"/");
        strcat (returned,Dep_delay);
        strcat (returned,"/");
        //strcat (returned,NULL);
        return returned;
    }
    if(cmd_index==3||cmd_index==4){
        std::cout<<"Chose the index then press enter:";
        for(int index=0;index<7;++index){
            std::cout<<index<<")"<<stations[index]<<std::endl;
        }
        std::cout<<"Introduce the name of the Station: ";
        char Station [20];
        std::cin>>Station;
        strcat (returned,cmd);
        strcat (returned,"/");
        strcat (returned,Station);
        strcat (returned,"/");
        //strcat (returned,NULL);
        return returned;
    }
    exit(-1);
}
char* maino();

/* codul de eroare returnat de anumite apeluri */
//extern int errno;

/* portul de conectare la server*/
int port;

int main3 (int argc, char *argv[])
{
    int sd;			// descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare
    // mesajul trimis
    int nr=0;
    char* buf=new char;

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi (argv[2]);

    /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons (port);

    /* ne conectam la server */
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");
        return errno;
    }

    /* citirea mesajului */
    printf ("[client]Introduceti un numar: ");
    fflush (stdout);
//    buf=maino();
    //read (0, buf, sizeof(buf));
    //nr=atoi(buf);
    //scanf("%d",&nr);
    nr=100;
    std::cout<<nr;
    std::cout<<buf;

    /* trimiterea mesajului la server */
    if (write (sd,&buf,sizeof(int)) <= 0)
    {
        perror ("[client]Eroare la write() spre server.\n");
        return errno;
    }

    /* citirea raspunsului dat de server
       (apel blocant pina cind serverul raspunde) */
    char* wad=new char;
    if (read (sd, &wad,sizeof(int)) < 0)
    {
        perror ("[client]Eroare la read() de la server.\n");
        return errno;
    }
    /* afisam mesajul primit */
    printf ("[client]Mesajul primit este: %s\n", wad);

    /* inchidem conexiunea, am terminat */
    close (sd);
}

int main32(){
    std::cout<<maino();
}

char* maino(){

    int cmd_index=print_commands ();
    // int cmd_index=0;
    std::cout<<cmd_index;
    std::cout<<commands[cmd_index];
    char* cmd_to_send=Threat_first_command_part (cmd_index,commands[cmd_index]);
    std::cout<<std::endl;
    //std::string p=cmd_to_send;
    //std::cout<<p;
    return cmd_to_send;
}


int main (int argc, char *argv[])
{
    int sd;			// descriptorul de socket
    struct sockaddr_in server;	// structura folosita pentru conectare
    // mesajul trimis
    int nr=0;
    char buf[10];

    /* exista toate argumentele in linia de comanda? */
    if (argc != 3)
    {
        printf ("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
        return -1;
    }

    /* stabilim portul */
    port = atoi (argv[2]);

    /* cream socketul */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Eroare la socket().\n");
        return errno;
    }

    /* umplem structura folosita pentru realizarea conexiunii cu serverul */
    /* familia socket-ului */
    server.sin_family = AF_INET;
    /* adresa IP a serverului */
    server.sin_addr.s_addr = inet_addr(argv[1]);
    /* portul de conectare */
    server.sin_port = htons (port);

    /* ne conectam la server */
    if (connect (sd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
        perror ("[client]Eroare la connect().\n");

        return errno;
    }

    /* citirea mesajului */
    /*  printf ("[client]Introduceti un numar: ");
      fflush (stdout);
      read (0, buf, sizeof(buf));0
      //scanf("%d",&nr);
      */
    char* a=maino ();
    int q=strlen (a);

    printf("[client] Am citit %s\n",a);

    /* trimiterea mesajului la server */
    if (write (sd,a,q) <= 0)
    {
        perror ("[client]Eroare la write() spre server.\n");
        return errno;
    }

    /* citirea raspunsului dat de server
       (apel blocant pina cind serverul raspunde) */
    //std::string p;
    // char*var=new char[100];
    std::string o;
    if (read (sd, a,1000) < 0)
    {
        perror ("[client]Eroare la read() de la server.\n");
        return errno;
    }
    /* afisam mesajul primit */
    else
    printf ("[client]Mesajul primit este:%s \n",a );
    //std::cout<<o;

    /* inchidem conexiunea, am terminat */
    close (sd);
}
