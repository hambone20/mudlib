#include <kernel/kernel.h>
#include <kernel/user.h>
#include <kernel/rsrc.h>
#include <kernel/version.h>
#include <config.h>
#include <kernel/access.h>
#include <std.h>

#include <game/sea_grid.h>

inherit access API_ACCESS;
inherit rsrc API_RSRC;

private static void _load (string daemon){/**< cleaning up repeated calls */
    find_object(DRIVER)->message ("Initd loading: " + daemon + "\n");
    if (!find_object (daemon))
	compile_object (daemon);
}

static int create (varargs int clone){
    object driver;
    object rsrcd;

    access::create ();
    rsrc::create ();

    /* adding Common */
    add_owner ("Common");
	add_owner ("Game");

    /* System requires root access */
    /** Set some global access for our domains */
    access::set_global_access("Common", READ_ACCESS);	
    access::set_global_access("Game", READ_ACCESS);
    /** @todo may have to add in other things to common, will they have to write? */

    driver = find_object (DRIVER);
    driver->message ("Initd....\n");
	
	driver->message("Kernel version: "+KERNEL_LIB_VERSION+"\n");
    
    /** @todo Versioning for mudlib? */

    _load (ERRORD);
    _load (LOGD);
    _load (OBJECTD);
    
    /** setup objectd */
	OBJECTD->do_initial_obj_setup();
	
    _load (TEL_MANAGER);
    _load (BIN_MANAGER);
    _load (ROOMD);
    _load (HEARTD);
    _load (CHANNELD);
    _load (ANSID);
    _load (HELPD);
    _load (SEA_D);
	_load (BRAIND);
	_load (ACCOUNTD);
	_load (WEATHERD);
    _load (WORLDBANKD);
    _load (THINGD);
}
/**
 * @brief called by driver before a reboot, logs out players, messages them
 * @todo cleanup
 */
void prepare_reboot(){
    object *users;
    int i;

    find_object(LOGD)->log("Mud is rebooting.", "boot");
    users = users();
    for(i = sizeof(users); i--; ){
        users[i]->message("Mud is rebooting...you will be disconnected.\n");
        users[i]->logout(1); /** @todo check argument */
    }
}
/**
 * @brief things that need done at reboot should go here
 * @todo cleanup
 */
void reboot(){
    find_object(LOGD)->log("Rebooted", "boot");
}
