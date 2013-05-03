/**
 * this handles subscribed 
 * events in the daemon   
 * @author Hymael           
 * @todo determine if this is needed 
 */
#include <config.h>

void evt_logon(mixed args...){
    write("in evt_logon....\n");
    this_object()->catch_tell("[ "+args[1]->query_cap_name()+" has logged on. ]\n");
}

void evt_quit(mixed args...){
    this_object()->catch_tell("[ "+args[1]->query_cap_name()+" has logged off. ]\n");
}

