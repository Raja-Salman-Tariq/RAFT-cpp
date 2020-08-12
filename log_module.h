#ifndef logg 
#define logg 

#include <iostream>
#include <list>
#include "message_module.h"

#define us_int  unsigned short int


struct LogEntry {
    // constructor
    LogEntry(us_int _term=0, Message _m={"", 0,0,{}}): term(_term), m(_m) {}
    // the first and second arguments above posses default values
    // the default for _m is constructed via a temporary 
    // Message object which is initialized by brace enclosed list

    us_int term;
    Message m;
};

class Log {
public:
    // uses default ctor;


    /* ===========================
        --- MEMBER FUNCTIONS ---
       ==========================*/

    // member functions use same functionality as py code
    // any significant alteration shall be mentioned in
    // comments in relevant functions


    void log_transaction(us_int term, Message m) {
        LogEntries.push_back({term, m});
    }

    us_int latest_term() {
        if (LogEntries.empty())
            return 0;   // 0 is our null value
        return LogEntries.back().term;
    }

    int latest_index() {
        return LogEntries.size() - 1;
    }

    us_int term_at_index(int index/*indexing MUST begin at 1*/) {
        if (index > LogEntries.size() || index < 1)
            return 0;                              //  ** CAUTION ** : assuming term will never be negative

        auto i = LogEntries.cbegin();
        advance(i, index);

        return i->term;
    }

    void replace_log_at_and_after_index(us_int index, std::list<LogEntry> & _log) {

        auto insertAt = LogEntries.cbegin(), insertFrom = _log.cbegin(), insertTill = _log.cend();

        advance(insertAt, index);

        LogEntries.insert(insertAt, insertFrom, insertTill);

    }

    void replace_log_at_and_after_index(us_int index, std::list<LogEntry> & _log, us_int indexToInsertFrom) {

        auto insertAt = LogEntries.cbegin(), insertFrom = _log.cbegin(), insertTill = _log.cend();

        advance(insertAt, index);
        advance(insertFrom, indexToInsertFrom);

        LogEntries.insert(insertAt, insertFrom, insertTill);

    }

    std::list<LogEntry> get_all_entries_starting_at_index(us_int index){
        auto i=LogEntries.cbegin();
        advance(i,index);

        std::list<LogEntry> toRet;
        toRet.assign(i, LogEntries.cend());
        return toRet;
    }

    std::list<LogEntry> get_entries_betweem_indexes(us_int start_index, us_int stop_index /*INCLUDES start and stop*/){

        std::list<LogEntry> toRet;

        if (stop_index>=LogEntries.size()){
            cout<<"\nERROR: In function call to get_entries_betweem_indexes in log_module.h; stop_index >= LogEntries.size -- must be less.";
            return toRet;
        }

        if (stop_index<start_index){
            cout<<"\nERROR: In function call to get_entries_betweem_indexes in log_module.h; stop_index > start_index -- must be less or equal.";
            return toRet;
        }

        auto i=LogEntries.cbegin(), end=LogEntries.cbegin();
        advance(i,start_index);
        advance(end, stop_index+1);

        toRet.assign(i, end);
        return toRet;

}

private:
    std::list<LogEntry> LogEntries;
};
#endif