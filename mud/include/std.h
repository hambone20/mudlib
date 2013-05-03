# define TRUE	1
# define FALSE	0

# define ASTARIA_CLONE "/astaria/" /**< set this option for astaria clone code */

# define BODY "/usr/Common/obj/body" /**< body */

# ifndef ASTARIA_CLONE

# define BODY_LIB "/usr/Common/lib/body"
# define HEALTH "/usr/Common/lib/health" /**< health */
# define COMBAT "/usr/Common/lib/combat" /**< combat */
# define WEAPON "/usr/Common/lib/weapon" /**< weapon */
# define ARMOR "/usr/Common/lib/armor"

# else /* astaria clone combat and shit */

# define BODY_LIB "/usr/Common/lib" + ASTARIA_CLONE + "body"
# define HEALTH "/usr/Common/lib" + ASTARIA_CLONE + "health" /**< health */
# define COMBAT "/usr/Common/lib" + ASTARIA_CLONE + "combat" /**< combat */
# define WEAPON "/usr/Common/lib" + ASTARIA_CLONE + "weapon" /**< weapon */
# define ARMOR "/usr/Common/lib" + ASTARIA_CLONE + "armor"

# endif

# define WEIGHT "/usr/Common/lib/weight" /**< weight logic */
# define SKILLS "/usr/Common/lib/skills" /**< skills */
# define WEALTH "/usr/Common/lib/wealth" /**< wealth */
# define ALIAS  "/usr/Common/lib/alias"  /**< aliases */

# define RACE_KIT "/usr/Common/lib/race_kit"   /**< race info */
# define OBJECT "/usr/Common/lib/object"       /**< object */
# define CONTAINER "/usr/Common/lib/container" /**< container */
# define STATS "/usr/Common/lib/stats"         /**< stats */
/** @todo: player user interface tool,
   not sure if this is proper route, make object
   for easy redirection? */

# define LIB_ROOM "/usr/Common/lib/room" /**< lib room */
# define ROOM "/usr/Common/obj/room"     /**< obj room */
# define COIN "/usr/Common/obj/coin"     /**< coin */
# define THING "/usr/Common/obj/object"  /**< ubiquitous nothing object */

# undef TRACKCOINS   /**< whether or not to track coins */
# define OBJ_ACCOUNT /**< this uses the account as objects
                        system, deprecated */

# define BRAIND "/usr/System/sys/braind"         /**< to track some things intelligently, like this_player */
# define HEARTD "/usr/System/sys/heartd"         /**< periodic call */
# define LOGD "/usr/System/sys/logd"             /**< logger */
# define HELPD "/usr/System/sys/helpd"           /**< helper daemon */
# define ROOMD "/usr/System/sys/roomd"           /**< room daemon */
# define CHANNELD "/usr/System/sys/channeld"     /**< channel daemon */
# define ACCOUNTD "/usr/System/sys/accountd"     /**< handles account chores */
# define WEATHERD "/usr/Common/sys/weatherd"     /**< weather daemon */
# define THINGD "/usr/System/sys/thingd"         /**< thing server, tracks items of type thing */
# define WORLDBANKD "/usr/Common/sys/worldbankd" /**< finance tracker */

# define TEL_MANAGER "/usr/System/sys/telnetd"	/**< telnet manager */
# define BIN_MANAGER "/usr/System/sys/binaryd"	/**< binary manager */
# define ERRORD "/usr/System/sys/errord"        /**< errord handler */
# define OBJECTD "/usr/System/sys/objectd"      /**< objectd handler */
# define ANSID "/usr/System/sys/ansi_parser"    /**< ansi parser */

# define LIB_LWO      "/usr/Common/data/lib_issue"          /**< helper for objectd */
# define CLONABLE_LWO "/usr/Common/data/clonable_issue"     /**< helper for objectd */
# define HEAVY_ARRAY "/usr/Common/obj/heavy_array"          /**< helper for objectd */
# define ISSUE_LWO   "/usr/Common/lib/issue_lwo"            /**< helper for objectd */

# define BODY_COMMAND "/usr/Common/lib/body_command" /**< body commands inherit this */
# define BODY_BIN "/usr/Common/obj/cmds/"            /**< where we keep body commands */
# define BODY_INPUT "/usr/Common/lib/input"          /**< body input logic */
# define CMD_PREFIX "cmd_"                           /**< command bin prefix */
# define OBJ_ACTION "perform_action"                 /**< function called in object to perform action */
# define PER_ACTION mixed perform_action(string verb, varargs string arg)  /**< perform the action */

/* direction mappings */
# define DIRS ([ "east" : "west", "south" : "north", "west" : "east", "north" : "south" ]) /**< direction mapping for inverses */
# define A_DIRS ({ "east", "west", "south", "north" })  /**< core directions */

# define VOWELS ({ 'a', 'e', 'i', 'o', 'u', 'y' }) /**< vowels */
