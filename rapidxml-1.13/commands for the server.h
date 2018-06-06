//
// Created by ubuntu on 1/14/18.
//

#ifndef RETELE_COMMANDS_FOR_THE_SERVER_H
#define RETELE_COMMANDS_FOR_THE_SERVER_H

#endif //RETELE_COMMANDS_FOR_THE_SERVER_H

#include <string>
#include <vector>
#include <map>
class BaseCommand {
public:
    virtual std::string Base_Command (int Hour,int Minute,std::string command)=0;
};
class TrainArrival:public BaseCommand{
public:
    std::string Base_Command (int Hour,int Minute,std::string command)override {

    }
};
class TrainDeparture:public BaseCommand{
public:
    std::string Base_Command (int Hour,int Minute,std::string command)override {

    }
};
class TrainDelay:public BaseCommand{
public:
    std::string Base_Command (int Hour,int Minute,std::string command)override {

    }
};
class StationArrival:public BaseCommand{
public:
    std::string Base_Command (int Hour,int Minute,std::string command)override {

    }
};
class StationDeparture:public BaseCommand{
public:
    std::string Base_Command (int Hour,int Minute,std::string command)override {

    }
};
class CommandCall {
    std::map<std::string , BaseCommand *> commands;

    std::map<std::string , BaseCommand *>::iterator execute (std::string &functionname) {
        auto itt = commands.find (functionname);
        if it
    }
};