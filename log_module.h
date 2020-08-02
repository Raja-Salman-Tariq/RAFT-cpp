#include <iostream>
#include <list>
#include "message_module.h"
#include "utils.h"

#define us_int  unsigned short int


std::string FILE_EXTENSION = ".txt";

struct LogEntry {
    LogEntry(us_int _term, Message _m): term(_term), m(_m) {}

    us_int term;
    Message m;
};

class Log {
public:
    Log() {}

    void log_transaction(us_int term, Message m) {
        LogEntries.push_back({term, m});
    }

    us_int latest_term() {
        if (LogEntries.empty())
            return -1;
        return LogEntries.back().term;
    }

    us_int latest_index() {
        return LogEntries.size() - 1;
    }

    us_int term_at_index(int index/*indexing MUST begin at 0*/) {
        if (index >= LogEntries.size() || index < 0)
            return -1;                              //  ** CAUTION ** : assuming term will never be negative

        auto i = LogEntries.cbegin();
        advance(i, index);

        return i->term;
    }

    void replace_log_at_and_after_index(us_int index, std::list<LogEntry> & _log) {
        LogEntries.resize(index);

        auto insertAt = LogEntries.cbegin(), insertFrom = _log.cbegin(), insertTill = _log.cend();

        advance(insertAt, index);
        //advance(insertFrom, 3);

        LogEntries.insert(insertAt, insertFrom, insertTill);

    }

    void replace_log_at_and_after_index(us_int index, std::list<LogEntry> & _log, us_int indexToInsertFrom) {
        LogEntries.resize(index);

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
        advance(end, stop_index+1)

        toRet.assign(i, end);
        return toRet;

}

private:
    std::list<LogEntry> LogEntries;
};