#ifndef keyval 
    #define keyval
#include <iostream>
#include <map>
#include "utils.h" 

#define us_int unsigned short int

std::string NODE_DATA = "nodes";
std::string FILE_EXTENSION = ".txt";


class PersistantKeyValueStore{
public:
    PersistantKeyValueStore(us_int _addr){
        db=(NODE_DATA+"/"
            +std::to_string(_addr)+"/"
            +std::to_string(_addr)+"_db"
            +FILE_EXTENSION);

        db_backup=(NODE_DATA+"/"
            +std::to_string(_addr)+"/"
            +std::to_string(_addr)+"_db_backup"
            +FILE_EXTENSION);

        log=(NODE_DATA+"/"
            +std::to_string(_addr)+"/"
            +std::to_string(_addr)+"_log"
            +FILE_EXTENSION);

        log_out=fopen(log.c_str(), "a");    
    }

    void log_set(std::string key, std::string value){
        std::string tmp=key+","+value;
        fputs(tmp.c_str(), log_out);
    }

    void set(std::string key, std::string val){
        trace_log("PersistantStorage: setting key "+key+" to value "+val);
        dict.insert({key,val});
        log_set( key, val);
    }

    std::string get(std::string key){
        auto tmp=dict.find(key);
        if (tmp!=dict.end())
            return tmp->second;         // key-val pair present and val returned
        std::string nullString="";
        return nullString;              // key-val pair not present
    }

    void flush(){
        FILE* _db_backup=fopen(db_backup.c_str(),"w");

        auto i=dict.cbegin(), end=dict.cend();

        //#1. BEGIN WRITING CACHE -> DISK
        for(;i!=end; i++){
            fputs((i->first+","+i->second+"\n").c_str(),_db_backup);
        }

        //#2. ONCE ALL CACHE->DISK, BACKUP becomes MAIN
        if (remove(db.c_str()))
            rename(db_backup.c_str(), db.c_str());
        else 
            if (rename(db_backup.c_str(), db.c_str()))
                printf("\nERROR: Converting file %s to %s unsuccessful.\n",db_backup, db);

        //#3. Handle Log
        //fseek(log_out, 0, SEEK_SET);
        fclose(log_out);
        log_out=fopen(log.c_str(),"w");
    }

    bool recover(){
        dict.clear();
        bool recoveredSomething=0;


        //# RECOVERING FROM DISK
        FILE* checker=fopen(db.c_str(),"r");    // checks existence
        if (checker){
            fclose(checker);
            fstream inFile(db.c_str(),ios::in);
            string key;
            string s;

            while (inFile.peek()!=inFile.eof()){
                getline(inFile,s);               // key extraction
                key=s;
                getline(inFile,s);          // val extraction

                dict.emplace(key, s);            // adding pair
            }
            recoveredSomething=1;
        }

        //# RECOVERING FROM MEMORY
        checker=fopen(log.c_str(),"r");
        if(checker){
            fclose(checker);
            fstream inFile(log.c_str(), ios::in);
            string key;
            string s;

            while (!inFile.peek()==inFile.eof()){
                getline(inFile, s);
                key=s;
                getline(inFile, s);

                dict.emplace(key, s);
            }
            recoveredSomething=1;

        }
        return recoveredSomething;
    }

private:
    std::string db;
    std::string db_backup;
    std::string log;
    FILE* log_out;
    std::map<std::string, std::string> dict; // for storing key value pairs
};/*
    """
    startup :   Startup for Key-Value Store
    """
    async def startup(self):
        pass
        */
    /*
    #---------------------------------------------------------------------------
    #                                   RECOVERY                               #
    #---------------------------------------------------------------------------
    
*/
#endif 