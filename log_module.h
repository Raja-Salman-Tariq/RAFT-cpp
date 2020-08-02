#include <iostream>
#include <list>
#include "message_module.h"

#define us_int  unsigned short int


std::string FILE_EXTENSION=".txt";

struct LogEntry{
    LogEntry(us_int _term, Message _m):term(_term), m(_m){}

    us_int term;
    Message m;
};

class Log{
public:
    Log(){}

private:
    std::list<LogEntry> entries;
};



/*from collections import namedtuple
from typing import NamedTuple, Callable, Any, Dict, List
from enum import Enum
import asyncio

from .async_module import Scheduler, Handler, Job
from .message_module import Message, MessageType, Messenger
from .persistant_key_value_store import PersistantKeyValueStore
from .utils import debug_log, trace_log
FILE_EXTENSION = ".txt"

"""
Log :   Raft Log
"""
class Log():
    """
    log_transaction : Logs a Transaction in the Raft Log
        Input(s):
            term    :   Term Number for Entry
            msg     :   Message to Log
        Output(s):
            None
        Side Effect(s):
            Appends that message-entry to the Back of the Log
    """
    def log_transaction(self, term : int, msg : Message):
        assert(msg.has_type(MessageType.SET))
        term = term
        type = msg.type
        args = msg.args
        new_entry = LogEntry(term, type, args)
        self.entries.append(new_entry)
    #---------------------------------------------------------------------------
    """
    latest_term : Get the latest term
        Input(s):
            None
        Output(s):
            Latest Term or -1 if no Entries Logged
    """
    def latest_term(self):
        if len(self.entries) != 0:
            return self.entries[-1].term
        else:
            return -1
    #---------------------------------------------------------------------------
    """
    latest_index : Get the latest index
        Input(s):
            None
        Output(s):
            Latest Index or -1 if no Entries Logged
    """
    def latest_index(self):
        last_index = len(self.entries) - 1
        return last_index
    #---------------------------------------------------------------------------
    """
    term_at_index : Get the term at index
        Input(s):
            index   :   Index to get the Term at
        Output(s):
            Term at the Index or None if that index DNE
    """
    def term_at_index(self, index : int):
        l = len(self.entries)
        if index < 0 or index >= l:
            return None
        return self.entries[index].term
    #---------------------------------------------------------------------------
    """
    replace_log_at_and_after_index  :   Replaces Log At and After Index
        Input(s):
            index       :   Index to Begin Replacing at
            other_log   :   Log to Replace with Starting at Index
        Output(s):
            None
        Side Effect(s):
            Replace entries including and after index with `entries`
            Example
            self.entries             [1,2,3,4,5,6,7]
                other_log                [1,2,2,3]
                index = 2
                [1, 2] + [1,2,2,3] = [1,2,1,2,2,3]
    """
    def replace_log_at_and_after_index(self, index : int, other_log: List):
        self.entries[index:] = other_log
    #---------------------------------------------------------------------------
    """
    get_all_entries_starting_at_index  :   Gets All Entries Starting at
                                           Index
        Input(s):
            index       :   Index Get from
        Output(s):
            List : Log Entries
        Side Effect(s):
            None
    """
    def get_all_entries_starting_at_index(self, index):
        return self.entries[index:]
    #---------------------------------------------------------------------------
    """
    get_entries_between_indexes  :   Gets All Entries between 2 indexes
                                     [index1, index2]
        Input(s):
            index1      :   Starting Index
            index2      :   Ending Index
        Output(s):
            List : Log Entries
        Side Effect(s):
            None
    """
    def get_entries_between_indexes(self, index1, index2):
        return self.entries[index1:(index2+1)]
#-------------------------------------------------------------------------------
"""
LogEntry    : Entry in the Log
    term    :   Term for that Entry
    type    :   Type of that Message for Entry
    args    :   Contains Args for that Message for Entry
"""
class LogEntry(NamedTuple):
    term: int
    type: str
    args: Dict[str, str]
*/