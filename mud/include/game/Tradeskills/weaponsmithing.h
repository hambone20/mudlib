/**
 *     weaponsmithing.h       
 * This contains recipes and  
 * the definitions for being a
 * weaponsmith                
 */

/** libs */
#define LIB_ANVIL "/usr/Game/lib/Tradeskills/anvil"
#define LIB_COMPONENT "/usr/Game/lib/Tradeskills/component"
#define COMPONENT "/usr/Game/obj/component"

/** skill tag name */
#define W_SMITH "weaponsmithing"

/** a log file for things related to weapon smithing */
#define W_SMITH_LOG(x) LOGD->log(x, "weaponsmithing");

/** prime attribute */
#define W_SMITH_PA  "strength"

/** skills                 accuracy   quality      damage    magical props */
#define W_SMITH_SKILLS ({ "shaping", "tempering", "edging", "imbuing" })

/* object files */

/** components */
#define C_HILT   "hilt"    /**< effects accuracy and speed */
#define C_POMMEL "pommel"  /**< effects accuracy and quality */
#define C_GUARD  "guard"   /**< effects quality */
#define C_BLADE  "blade"   /**< effects everything */
#define C_AXE    "axehead" /**< the blade equivelant of an axe */
/* shaft defined by woodworker */

#define W_SMITH_REC ([ "sword" : ({ C_BLADE, C_HILT, C_POMMEL, C_GUARD }), "axe"   : ({ C_AXE, "shaft" }) ])

#define W_SMITH_TEMP_EDGE ({ C_AXE, C_BLADE })


/** component types */
#define C_OIL    "oil"
#define C_ALLOY  "alloy"
#define C_HAMMER "hammer"

/**
 * @brief Each part of this process will have   
 * an overall effect on the outcome of   
 * the weapon.  Accuracy, quality,       
 * damage, magical props, anything on    
 * the weapon that varies will be molded 
 * through this creation process.        
 */


/** @todo recipes */
