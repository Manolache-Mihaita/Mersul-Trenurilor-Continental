#include <iostream>
#include <fstream>
#include <cstring>
#include <regex>
#include "xmlStuff.h"
#include <unordered_map>
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include <netinet/in.h>
#include <time.h>
#include <iomanip>
#include <ctime>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>


struct Time{
    unsigned short Hour;
    unsigned short Minute;
};
struct Data{
    Time Arrival;
    int Arrival_delay;
    Time Departure;
    int Departure_delay;
};
struct Train_Station {
    std::string ID;
    Data D;
};
struct Trains{
    std::string ID;
    std::vector<Train_Station> Stations;
};
struct train_save_station{
    std::string ID;
    Data D;
};
struct Stations{
    std::string ID;
    std::vector<train_save_station*> Trains;
};

struct cmd_call_input{
    std::string command;
    tm * a;
    int socket_id;
};
std::vector<Trains> Trains_Data;
std::vector<Stations> Stations_Data;
std::vector<cmd_call_input> Command_Queue;

void GetArrivalData232422 () {
    //std::cout << "@#!";
    std::ifstream fil;
    fil.open ("departures.xml");
    if ( fil.is_open ()) { std::cout << "We can read from the file!"<<std::endl; }
    else std::cout << "File couldn't be open!"<<std::endl;
    rapidxml::xml_document<> doc;
    std::stringstream buffer;
    buffer << fil.rdbuf ();
    fil.close ();
    std::string content (buffer.str ());
    doc.parse<0> (&content[ 0 ]);
    rapidxml::xml_node<> *Root = doc.first_node ();
    //std::cout<<std::endl;
    for ( rapidxml::xml_node<> *pos = Root->first_node ("train") ; pos ; pos = pos->next_sibling ()) {
        //std::cout<<"TRAIN: "<<pos->first_attribute ("ID")->value ()<<std::endl;
        //########
        Trains Tr_Dta;
        Tr_Dta.ID=pos->first_attribute ("ID")->value ();
        //########
        for(rapidxml::xml_node<> *stat=pos->first_node ("station");stat;stat=stat->next_sibling ()) {
            //#######
            Train_Station Temp_station;
            //#######
            //std::cout<<"Station: "<<std::endl;

            rapidxml::xml_node<> *temp=stat->first_node ("ID");
            //std::cout<<"ID: "<<temp->value ()<<std::endl;
            Temp_station.ID=temp->value ();




            temp=stat->first_node ("arrival");
            //std::cout<<"arrival: ";
            //std::cout<<temp->value ()<<std::endl;
            char*data= temp->value();
            char*p1=strtok(data,":");
            Temp_station.D.Arrival.Hour=atoi(p1);
            p1=strtok (NULL,":");
            Temp_station.D.Arrival.Minute=atoi(p1);


            temp=stat->first_node ("arrival_early_delay");
            //std::cout<<"arrival_early_delay: ";
            //std::cout<<temp->value ()<<std::endl;
            Temp_station.D.Arrival_delay=atoi(temp->value ());

            temp=stat->first_node ("arrival_late_delay");
            //std::cout<<"arrival_late_delay: ";
            //std::cout<<temp->value ()<<std::endl;


            temp=stat->first_node ("departure");
            //std::cout<<"departure: ";
            //std::cout<<temp->value ()<<std::endl;
            data= temp->value();
            p1=strtok(data,":");
            Temp_station.D.Departure.Hour=atoi(p1);
            p1=strtok (NULL,":");
            Temp_station.D.Departure.Minute=atoi(p1);



            temp=stat->first_node ("departure_delay");
            //std::cout<<"departure_delay: ";
            //std::cout<<temp->value ()<<std::endl;
            Temp_station.D.Departure_delay=atoi(temp->value ());

            //std::cout<<std::endl<<std::endl;


            Tr_Dta.Stations.push_back (Temp_station);
        }
        Trains_Data.push_back (Tr_Dta);


    }
    //std::cout<<std::endl;

}
int check_if_station_exists(std::string Station_ID){
    for(int index=0;index<Stations_Data.size ();++index){
        if(Stations_Data[index].ID==Station_ID) return index;
    }
    return -1;
}

int check_if_train_exists(std::string Train_ID){
    for(int index=0;index<Trains_Data.size ();++index){
        if(Trains_Data[index].ID==Train_ID)return index;
    }
    return -1;
}

void Stations_data(){
    for(int index=0;index<Trains_Data.size ();++index){
        for(int index_stations=0;index_stations<Trains_Data[index].Stations.size ();++index_stations){

            int pos=check_if_station_exists (Trains_Data[index].Stations[index_stations].ID);

            if (pos==-1){
            train_save_station* temp=new train_save_station;
                //std::cout<<Trains_Data[index].ID;
                temp->ID=Trains_Data[index].ID;
                temp->D=Trains_Data[index].Stations[index_stations].D;
                Stations temp2;
                temp2.ID=Trains_Data[index].Stations[index_stations].ID;
                temp2.Trains.push_back (temp);
                Stations_Data.push_back (temp2);
            }
            else{
                train_save_station*temp3=new train_save_station;
                temp3->ID=Trains_Data[index].ID;
                //std::cout<<std::endl<<std::endl<<Trains_Data[index].ID<<std::endl<<std::endl;
                temp3->D=Trains_Data[index].Stations[index_stations].D;
                Stations_Data[pos].Trains.push_back (temp3);
                //std::cout<<std::endl<<"WWWAASDDS 1:"<<Stations_Data[pos].Trains[0]->ID<< " 2: "<<Stations_Data[pos].Trains[1]->ID<<std::endl;
            }
        }
    }
}







struct queue{
    int socket_id;
    std::string command;
};

std::vector<queue>command_queue;



std::string int_to_string(int val){
/*        std::string returned;
        returned+=val/10+'0';
        returned+=val%10+'0';
        return returned;
*/
    std::string a;
    while(val){
        a+=val%10+'0';
        val/=10;
    }
    for(int index=0;index<a.size ()/2;++index){
        std::swap (a[index],a[a.size ()-index-1]);
    }
    return a;
 }
class BaseCommand {
public:
    virtual std::string Base_Command (cmd_call_input data)=0;
};
class TrainArrival:public BaseCommand{
public:

    std::string Base_Command (cmd_call_input data)override {
     std::string returned;
        char a[300];
        strcpy (a,data.command.c_str ());
        std::string temp=strtok (a,"/");
        int train_index=check_if_train_exists (temp);
        if (train_index==-1)return "train doesnt exist!";
        else{
        returned+="Train: ";
        returned+=Trains_Data[train_index].ID;
        returned+='\n';


            for(auto index=0;index<Trains_Data[train_index].Stations.size ();++index){
                returned+="Station: ";
                returned+=Trains_Data[train_index].Stations[index].ID;
                returned+=" At:";
                returned+=int_to_string (Trains_Data[train_index].Stations[index].D.Arrival.Hour);
                returned+="-";
                returned+=int_to_string (Trains_Data[train_index].Stations[index].D.Arrival.Minute);
                int q=Trains_Data[train_index].Stations[index].D.Arrival_delay;
                if (q==0) returned+= " With No delay";
                if(q>0){
                    returned +=" With ";
                    returned+= int_to_string (abs(q));
                    returned+= " minutes delay";
                }
                if(q<0){
                    returned+=" With ";
                    returned+= int_to_string (abs(q));
                    returned+=" minutes early";
                }
                returned+='\n';
            }

        }
        return  returned;

    }
};
class TrainDeparture:public BaseCommand{
public:
    std::string Base_Command (cmd_call_input data)override {
        std::string returned;
        char a[300];
        strcpy (a,data.command.c_str ());
        std::string temp=strtok (a,"/");
        int train_index=check_if_train_exists (temp);
        if (train_index==-1)return "train doesnt exist!";
        else{
        returned+="Train: ";
        returned+=Trains_Data[train_index].ID;
        returned+='\n';

            for(auto index=0;index<Trains_Data[train_index].Stations.size ();++index){
                returned+="Station: ";
                returned+=Trains_Data[train_index].Stations[index].ID;
                returned+=" At:";
                returned+=int_to_string (Trains_Data[train_index].Stations[index].D.Departure.Hour);
                returned+="-";
                returned+=int_to_string (Trains_Data[train_index].Stations[index].D.Departure.Minute);
                int q=Trains_Data[train_index].Stations[index].D.Departure_delay;
                if (q==0) returned+= " With No delay";
                if(q>0){
                    returned +=" With ";
                    returned+= int_to_string (abs(q));
                    returned+= " minutes delay";
                }
                if(q<0){
                    returned+=" With ";
                    returned+= int_to_string (abs(q));
                    returned+=" minutes early";
                }
                returned+='\n';
            }

        }
        return  returned;

    }
};
class TrainDelay:public BaseCommand{
public:
    std::string Base_Command (cmd_call_input data)override {
        std::string returned;
        char a[300];
        strcpy (a,data.command.c_str ());
        std::string temp=strtok (a,"/");
        int train_index=check_if_train_exists (temp);
        if(train_index<0)return"Train doesnt exist!";
        returned+="Train: ";
        returned+=Trains_Data[train_index].ID;
        returned+='\n';
        temp=strtok (NULL,"/");
        int station_index=0;
        for(station_index=0;station_index<Trains_Data[train_index].Stations.size ();++station_index){
            if(temp==Trains_Data[train_index].Stations[station_index].ID)break;
            station_index=-1;
        }
        if(station_index==-1){returned+="Station doesn't exist!";
        return returned;}
        else{
            returned+=" Staion: ";
            returned+=temp;
            int dep=Trains_Data[train_index].Stations[station_index].D.Departure.Minute+Trains_Data[train_index].Stations[station_index].D.Departure.Hour*60;
            int arr=Trains_Data[train_index].Stations[station_index].D.Arrival.Minute+Trains_Data[train_index].Stations[station_index].D.Arrival.Hour*60;
            int arr_del=atoi(strtok (NULL,"/"));
            int dep_del=atoi(strtok (NULL,"/"));
            Trains_Data[train_index].Stations[station_index].D.Arrival_delay+=arr_del;
            Trains_Data[train_index].Stations[station_index].D.Departure_delay+=dep_del;
            arr+=arr_del;
            dep=dep_del;
            Trains_Data[train_index].Stations[station_index].D.Arrival.Minute=arr%60;
            Trains_Data[train_index].Stations[station_index].D.Arrival.Hour=arr/60%24;
            Trains_Data[train_index].Stations[station_index].D.Departure.Minute=dep%60;
            Trains_Data[train_index].Stations[station_index].D.Departure.Hour=dep/60%24;
            returned += "Update succesfuly";
        }
        return returned;
    }
};
class StationArrival:public BaseCommand{
public:
    std::string Base_Command (cmd_call_input data)override {
        std::string returned;
        char a[300];
        strcpy (a,data.command.c_str ());
        std::string temp=strtok (a,"/");
        returned+="Station: ";
        int station_index=check_if_station_exists (a);
        returned+=a;
        if(station_index==-1){
            return "Station doesnt exist!";
        }
        returned+='\n';
        int time_integer=data.a->tm_min;
        int time_integer_act=data.a->tm_min;
        if(data.a->tm_hour==0){
            time_integer+=2500;
            time_integer_act+=2400;
        }
        else{
            time_integer+=(data.a->tm_hour+1)*100;
            time_integer_act+=data.a->tm_hour*100;
        }
        bool test=false;
        for(auto index=0;index<Stations_Data[station_index].Trains.size ();++index){
                int time_train_integer=Stations_Data[station_index].Trains[index]->D.Arrival.Minute;
            if(Stations_Data[station_index].Trains[index]->D.Arrival.Hour==0){
                time_train_integer+=2400;
            }
            else {
                time_train_integer += Stations_Data[ station_index ].Trains[ index ]->D.Arrival.Hour * 100;
            }
            if(time_train_integer>=time_integer_act&&time_train_integer<=time_integer) {
                test=true;
                returned += " Train ";
                returned += Stations_Data[ station_index ].Trains[ index ]->ID;
                returned += " Arrives in the station at: ";
                returned += int_to_string (Stations_Data[ station_index ].Trains[ index ]->D.Arrival.Hour);
                returned += '-';
                returned += int_to_string (Stations_Data[ station_index ].Trains[ index ]->D.Arrival.Minute);
                returned += " With ";
                if ( Stations_Data[ station_index ].Trains[ index ]->D.Arrival_delay < 0 ) {
                    returned += int_to_string (abs (Stations_Data[ station_index ].Trains[ index ]->D.Arrival_delay));
                    returned += " minutes early .";
                }
                if ( Stations_Data[ station_index ].Trains[ index ]->D.Arrival_delay == 0 ) {
                    //returned += int_to_string (abs (Stations_Data[ station_index ].Trains[ index ]->D.Arrival_delay));
                    returned += "No delay.";
                }
                if ( Stations_Data[ station_index ].Trains[ index ]->D.Arrival_delay > 0 ) {
                    returned += int_to_string (abs (Stations_Data[ station_index ].Trains[ index ]->D.Arrival_delay));
                    returned += " minutes late .";
                }
                returned+='\n';
            }

        }
        if(!test){returned+="Has no Trains arriving in the next hour.";}
        return returned;
    }
};
class StationDeparture:public BaseCommand{
public:
    std::string Base_Command (cmd_call_input data)override {
        std::string returned;
        char a[300];
        strcpy (a,data.command.c_str ());
        std::string temp=strtok (a,"/");
        returned+="Station: ";
        int station_index=check_if_station_exists (a);
        returned+=a;
        if(station_index==-1){
            return "Station doesnt exist!";
        }
        returned+='\n';
        int time_integer=data.a->tm_min;
        int time_integer_act=data.a->tm_min;
        if(data.a->tm_hour==0){
            time_integer+=2500;
            time_integer_act+=2400;
        }
        else{
            time_integer+=(data.a->tm_hour+1)*100;
            time_integer_act+=data.a->tm_hour*100;
        }
        Stations_Data;
        bool test=false;
        for(auto index=0;index<Stations_Data[station_index].Trains.size ();++index){
            int time_train_integer=Stations_Data[station_index].Trains[index]->D.Departure.Minute;
            if(Stations_Data[station_index].Trains[index]->D.Departure.Hour==0){
                time_train_integer+=2400;
            }
            else {
                time_train_integer += Stations_Data[ station_index ].Trains[ index ]->D.Departure.Hour * 100;
            }
            if(time_train_integer>=time_integer_act&&time_train_integer<=time_integer) {
                test=true;
                returned += " Train ";
                returned += Stations_Data[ station_index ].Trains[ index ]->ID;
                returned += "Leaves the station at: ";
                returned += int_to_string (Stations_Data[ station_index ].Trains[ index ]->D.Departure.Hour);
                returned += '-';
                returned += int_to_string (Stations_Data[ station_index ].Trains[ index ]->D.Departure.Minute);
                returned += " With ";
                if ( Stations_Data[ station_index ].Trains[ index ]->D.Departure_delay < 0 ) {
                    returned += int_to_string (abs (Stations_Data[ station_index ].Trains[ index ]->D.Departure_delay));
                    returned += " minutes early .";
                }
                if ( Stations_Data[ station_index ].Trains[ index ]->D.Departure_delay == 0 ) {
                    //returned += int_to_string (abs (Stations_Data[ station_index ].Trains[ index ]->D.Arrival_delay));
                    returned += "No delay.";
                }
                if ( Stations_Data[ station_index ].Trains[ index ]->D.Departure_delay > 0 ) {
                    returned += int_to_string (abs (Stations_Data[ station_index ].Trains[ index ]->D.Departure_delay));
                    returned += " minutes late .";
                }
                returned+='\n';
            }

        }
        if(!test){
            returned +="Has no trains Leaving in the next hour.";
        }
        return returned;
    }

};
class CommandCall {
    std::map<std::string , BaseCommand *> commands;

    std::map<std::string , BaseCommand *>::iterator cmd_find(std::string &functionname) {
        auto itt = commands.find (functionname);
        if (itt==commands.end ())std::cout<<"not valid :";
        return itt;
    }

public:
    CommandCall(){
        commands.insert(std::make_pair ("Arrival Train",new TrainArrival));
        commands.insert(std::make_pair ("Departure Train",new TrainDeparture));
        commands.insert(std::make_pair ("Delay Train",new TrainDelay));
        commands.insert(std::make_pair ("Station Arrival",new StationArrival));
        commands.insert(std::make_pair ("Station Departure",new StationDeparture));
    }
    std::string execute(std::string &function_name,cmd_call_input data){
        // change them to return a struct of socket+ string or accept a struct of string + socket and return the same strucutre with the socket changed;
        time_t tmi = time (0);
        struct tm *n = localtime (&tmi);
        data.a=n;


        auto command=cmd_find (function_name);
        std::string a=command->second->Base_Command (data);

        //@@@@

        char a1[300];
        strcpy (a1 , a.c_str ());
          if (write (data.socket_id, &a1, a.size ()+1) <= 0)
          {
              printf("[Thread %d] ",data.socket_id);
              perror ("[Thread]Eroare la write() catre client.\n");
          }
          else
              //printf ("[Thread %d]Mesajul a fost trasmis cu succes.\n",data.socket_id);
              return a;
          }
      };


      void threat_queue();
\
// create a thread in main to check if the size of the queue
// is not null
// also if the bool value for working with the function is ok


//could also read the port tho
struct temp_used_for_queue{
    std::string cmd_name;
    cmd_call_input a;
};
void *call_cmd_pat(void *a){
    temp_used_for_queue* l;
    l=(temp_used_for_queue*)a;
    CommandCall o;
    o.execute (l->cmd_name,l->a);
}
bool queue_used=false;
bool global_temp=false;
void* threat_queue(void* a) {
    while ( 1 ) {
        CommandCall p;
        while ( queue_used&&Command_Queue.size ()==0 ) {

        }
        global_temp=true;
        queue_used = true;
        std::vector<cmd_call_input> temp2;
        std::vector<cmd_call_input> temp = Command_Queue;
        Command_Queue = temp2;
        global_temp= false;
        std::vector<temp_used_for_queue> first;
        std::vector<temp_used_for_queue> second;
        for ( auto index = 0 ; index < temp.size () ; ++ index ) {
            char a[300];
            strcpy (a , temp[ index ].command.c_str ());
            std::string temp_string = strtok (a , "/");
            temp_string.size ();
            temp[index].command.erase (temp[index].command.begin (),temp[index].command.begin ()+temp_string.size ()+1);
            if ( temp_string == "Delay Train" ) {
                temp_used_for_queue temp_a;
                temp_a.cmd_name = temp_string;
                temp_a.a = temp[ index ];
                first.push_back (temp_a);
            } else {
                temp_used_for_queue temp_a;
                temp_a.cmd_name = temp_string;
                temp_a.a = temp[ index ];
                second.push_back (temp_a);
            }
        }
        for ( auto index = 0 ; index < first.size () ; ++ index ) {
            p.execute (first[ index ].cmd_name , first[ index ].a);
        }
        pthread_t idt[100];
        for ( auto index = 0 ; index < second.size () ; ++ index ) {
            temp_used_for_queue* tmp=new temp_used_for_queue;
            tmp=&second[index];

            pthread_create (&idt[index],NULL,&call_cmd_pat,tmp);
        }
        for(auto index = 0; index<second.size ();++index){
            pthread_join (idt[index],NULL);
        }
        queue_used = false;
    }
}
#define PORT 2876
typedef struct thData{
    int idThread; //id-ul thread-ului tinut in evidenta de acest program
    int cl; //descriptorul intors de accept
}thData;

static void *treat(void *); /* functia executata de fiecare thread ce realizeaza comunicarea cu clientii */
void raspunde(void *);

int server ()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    int nr;		//mesajul primit de trimis la client
    int sd;		//descriptorul de socket
    int pid;
    pthread_t th[100];    //Identificatorii thread-urilor care se vor crea
    int i=0;


    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("[server]Eroare la socket().\n");
        return errno;
    }
    /* utilizarea optiunii SO_REUSEADDR */
    int on=1;
    setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
        perror ("[server]Eroare la bind().\n");
        return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 2) == -1)
    {
        perror ("[server]Eroare la listen().\n");
        return errno;
    }
    /* servim in mod concurent clientii...folosind thread-uri */
    while (1)
    {
        int client;
        thData * td; //parametru functia executata de thread
        int length = sizeof (from);

        printf ("[server]Asteptam la portul %d...\n",PORT);
        fflush (stdout);

        //client= malloc(sizeof(int));
        /* acceptam un client (stare blocanta pina la realizarea conexiunii) */
        if ( (client = accept (sd, (struct sockaddr *) &from,( socklen_t*)&length)) < 0)
        {
            perror ("[server]Eroare la accept().\n");
            continue;
        }

        /* s-a realizat conexiunea, se astepta mesajul */

        int idThread; //id-ul threadului
        int cl; //descriptorul intors de accept

        td=(struct thData*)malloc(sizeof(struct thData));
        td->idThread=i++;
        td->cl=client;

        pthread_create(&th[i], NULL, &treat, td);

    }//while
};
static void *treat(void * arg)
{
    struct thData tdL;
    tdL= *((struct thData*)arg);
    printf ("[thread]- %d - Asteptam mesajul...\n", tdL.idThread);
    fflush (stdout);
    pthread_detach(pthread_self());
    raspunde((struct thData*)arg);
    /* am terminat cu acest client, inchidem conexiunea */
    //close ((intptr_t)arg);
    return(NULL);

};


void raspunde(void *arg)
{
    int nr, i=0;
    struct thData tdL;
    //std::string var;
    tdL= *((struct thData*)arg);
    char smth[300];
    if (read (tdL.cl, &smth,sizeof(char)*300) <= 0)
    {
        printf("[Thread %d]\n",tdL.idThread);
        perror ("Eroare la read() de la client.\n");

    }

    printf ("[Thread %d]Mesajul a fost receptionat...%s\n",tdL.idThread,smth);
    //std::cout<<var;
    /*pregatim mesajul de raspuns */
    //printf("[Thread %d]Trimitem mesajul inapoi...%s\n",tdL.idThread, smth);

    cmd_call_input command;

    command.command=smth;
    command.socket_id=tdL.cl;
    //Command_Queue.push_back (command);
    time_t tmi = time (0);
    struct tm *n = localtime (&tmi);
    command.a=n;
    while(global_temp){};
    Command_Queue.push_back (command);
    /* returnam mesajul clientului */
  /*  if (write (tdL.cl, &nr, sizeof(int)) <= 0)
    {
        printf("[Thread %d] ",tdL.idThread);
        perror ("[Thread]Eroare la write() catre client.\n");
    }
    else
        printf ("[Thread %d]Mesajul a fost trasmis cu succes.\n",tdL.idThread);

*/
}

int main() {
    GetArrivalData232422 ();
    Stations_data ();
    pthread_t idt;
    pthread_create (&idt,NULL,threat_queue,&idt);
    server ();
    return 0;
}