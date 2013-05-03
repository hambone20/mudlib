/**
 * Logger, log daemon, for your all purpose error logging, debugging,
 * and tree harvesting needs
 * @author Hymael
 * @todo  Add ability for wiz to listen in on logs
 *        add functions for sorting out logs and retrieving via different info,
 *        eventually the log will just log into an object
 */
#include <kernel/kernel.h>
#define LOG_EXT ".log"
#define LOG_DIR "/logs"
#define DEFAULT LOG_DIR + "/general" + LOG_EXT
#define STAMP "- <"+timestamp+"> "+prev_obj+" "+user_name+":\n\t"
#undef DEBUG  /**< debug mode */
#define WIZTOOL_CALL  /**< send along wiztool messages to their owner */

/**
 * @brief log Function to write a message to a log file
 * @param mess String of message to write.
 * @param type @em optional type of log file, determines where it goes.
 */
void log(string mess, varargs string type){
    string timestamp, prev_obj, user_name;
    int sendto;
    if(type && strlen(type)){/**< place in appropriate file, may add checking to make sure this isn't exploited */
        if(sscanf(type, "%*s..%*s"))/**< @todo possible exploit, ignore for now */
            return;
        type = LOG_DIR + "/" + type + LOG_EXT;
    }else{
        type = DEFAULT;
    }
    timestamp = ctime(time())[4..];
    prev_obj = object_name(previous_object());
    if(this_user()){
        user_name = this_user()->query_Name();
        #ifdef DEBUG
        sendto = 1;
        #endif
    }else if(previous_program()){
        user_name = previous_program();
    }else{
        user_name = "?System?";
    }

    /** stamp message */
    mess = STAMP + mess + "\n\n";
    /** @todo HAcK */
    if(sendto)
        this_user()->message(mess);
    /** now append to file */
    write_file(type, mess);
    #ifdef DEBUG
    find_object(DRIVER)->message(mess);

    if(type == "/logs/compile_errors.log" && this_user()){/**< send on to user of note */
        this_user()->message(mess);
    }
    #endif
#ifdef WIZTOOL_CALL
    if(type && type=="/logs/wiztool.log"){
        previous_object()->query_user()->message(mess); 
    }

    if(type && (type=="/logs/runtime_errors.log" || type=="/logs/compile_errors.log") && this_user() && this_user()->query_wiztool()){/* check for wiz */
        this_user()->message(mess);
    }

#endif
}

