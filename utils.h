#include <iostream>
#include <fstream>

bool debug = true;
bool debug_to_file = true;
bool trace = true;

//"""
//debug_log   :   Prints Message to General Purpose Debug Log
//"""

void debug_log(std::string args){
    if (debug){
        printf("%s\n", args); // print to console

        if(debug_to_file){
            FILE * logFile= fopen("log.txt", "a");
            args.append("\n");
            fputs(args.c_str(),logFile);
        }
    }
}


//"""
//trace_log   :   Prints Message to Screen
//"""
void trace_log(std::string args){
    if (trace)
        printf("%s\n", args); // print to console
}