# RAFT-cpp
Raft implementation in C++

Some information regarding the code...

Too much detail regarding C++ isn't presented here, but if you'd want to know, please feel free to express so and I shall explain any portion of the code.
The README mainly focuses on the structure of the code.

1 - Keeping in view potentially rigorous usage, a small optimization has been introduced as 'us_int'
  - An unsigned shot integer consumes only 2 bytes, so it's been used wherever possible as 'us_int'
  - As us_int is unsigned, we can't use negative values, so a null/empty/invalid value would be 0 instead of -1

2 - void* (void pointers) have been used for handling futures and function pointers.
  - This might not hold as futures as "temporary", although I was unable to test this.
  - Futures also are templatized and differ with respect to the relevant async function
    call's return type, so to make a container which could hold any future, they were
    casted to void *
    
    NOTE: by active variable below, it is meant a variable which keeps reference to a 
          future for a task.
          
3 - A cancelled_tasks list is maintained which keeps track of tasks which were 'cancelled'
  - As cancellation functionality was not available, the employed mechanism was to simply
    shift a "future" from an active variable, like candidacy or replication_running, to
    the cancelled_tasks list. To do so, simply the future's void* could be shifted with
    minimal overhead and complications (hopefully).
  - Now a new void * (belonging to a future) can be stored in the active variable like
    candidacy or replication_running when a new task is created.
  - It is thus important to prevent massive memory leak and every once in a while, maybe
    also asynchronously, traverse cancelled_tasks and call future::get() to unblock,
    release, and terminate threads associated with "cancelled tasks."
    
4 - Generally, in class definitions, the constructor is shown at the top, followed by
    methods, including getters, setters, helpers, etc., and the class data members are 
    listed at the end of the class definition. 
