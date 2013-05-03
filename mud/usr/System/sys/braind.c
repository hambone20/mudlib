/** this daemon does some of the random necessary things to keep the mud operating 
 *  @brief keeps this_player in TLS
 *  @todo Cleanup
 */
#include <config.h>
#include <kernel/tls.h>

inherit tls API_TLS;

static void create() {
    tls::create();
    set_tls_size(1); /** one thread local variable, index 0 
                      * @todo Add defines for these offsets into TLS
                      */
}

/** 
 *  @brief set_this_player is called to set this_player for current thread
 *  @param player Object containing the player to be set
 *  @todo check to make these calls are from a reliable source, security
 */
void set_this_player(object player) {
    set_tlvar(0, player); /**< @todo Add defines for offsets? */
}

/**
 * @brief this_player is used to get the current player in thread
 * @retval return the current player 
 * @todo security on call, and any nil checking?
 */
object this_player() {
    return get_tlvar(0);
}
