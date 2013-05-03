#include <kernel/user.h>
#include <config.h>

static void create(varargs int clone){
    if(!find_object(SYSTEM_USER))
	compile_object(SYSTEM_USER);

    USERD->set_telnet_manager(0, this_object());
}

/**
 * @brief Return a user object, selected by the argument string, which is the
 *	      first line of input on that connection.
 */
object select(string str){
    if(str == "admin")/**< admin requests kernel user */
        return clone_object(DEFAULT_USER);
        
    return clone_object(SYSTEM_USER);
}

/**
 * @brief Return a timeout, after which the given connection is closed if no
 *  	  user object has been associated with it yet.  If the timeout is -1,
 * 	      the connection is closed immediately.
 */
int query_timeout(object connection){
    return DEFAULT_TIMEOUT;
}

/**
 * @brief Return a login banner for the given connection.  Nil can be returned to
 *	      indicate that no banner should be shown at all.
 */
string query_banner(object connection){
    return "S.O.S.S.\nWhat's your name? ";
}
