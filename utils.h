#ifndef utils_present
#define utils_present

#include <iostream>
#include <fstream>
#include <string>


#define us_int unsigned short int

bool debug = true;
bool debug_to_file = true;
bool trace = true;
bool test_to_screen = true;
bool test_to_file = true;



// NOTE DIRECTORIES MUST EXIST 


/*
    debug_log   :   Prints Message to General Purpose Debug Log
*/
void debug_log(std::string args, us_int& addr){
    // NOTE a message can be passed directly as a
    //  string argument using Message::toStr()
    if (debug){
        printf("%s\n", args.c_str()); // print to console

        if(debug_to_file){
            // open file with given path
            FILE * logFile= fopen(("NODE_DATA/"+std::to_string(addr)+"/"+std::to_string(addr)+"_debug_log.txt").c_str(), "a");
            // insert new line character at end of string to be written to file
            args.append("\n");
            // write to file
            fputs(args.c_str(),logFile);
        }
    }
}


/*
trace_log   :   Prints Message to Screen
*/
void trace_log(std::string args){
    if (trace)
        printf("%s\n", args.c_str()); // print to console
}


void test_log(std::string args, us_int& addr){
    if (test_to_screen)
        printf("%s\n", args.c_str() );    

    if (test_to_screen){
            FILE * logFile= fopen(("NODE_DATA/"+std::to_string(addr)+"/"+std::to_string(addr)+"_test_log.txt").c_str(), "a");
            args.append("\n");
            fputs(args.c_str(),logFile);
        }
}
#endif