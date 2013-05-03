/**
 * Sea grid 
 */
#define SEA_D "/usr/Game/sys/sea_d"

#define LIB_SHIP_ROOM "/usr/Game/lib/ship_room"
#define SHIP_ROOM "/usr/Game/obj/ship_room"
#define LIB_PORTAGE "/usr/Game/lib/portage"
#define PORTAGE   "/usr/Game/obj/portage"
#define SHIP  "/usr/Game/obj/ship"
#define LIB_SHIP "/usr/Game/lib/ship"
 
#define X_DIM 10 /**< x dimension */
#define Y_DIM 10 /**< y dimension */

#define H_X 0
#define H_Y 1

#define SG_PORT ({ 0, 5 }) /**< where our port is */

#define WHEEL_DIRS ({ "port", "starboard" }) /**< wheel directions */

#define DIRS_SIZE 8

#define CARDINALS ({ "north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest"}) /**< @todo maybe use from std.h? */

#define SG_ROOM 0  /**< indexes into the grid array */
#define SG_FUNC 1  /**< function to be called */
#define SG_OBJ  2  /**< object which contains the entering code, defaults to sea daemon */
