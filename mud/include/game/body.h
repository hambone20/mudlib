/*******************************
 *           @file body.h      *
 * This contains defining info *
 * about the generic body      *
 *******************************/

/** body parts */
#define BP_HEAD "head"
#define BP_LARM "left arm"
#define BP_RARM "right arm"
#define BP_TORS "torso"
#define BP_LLEG "left leg"
#define BP_RLEG "right leg"

#define LIMB "/usr/Common/obj/limb"     /**< limb object */
#define CORPSE "/usr/Common/obj/corpse" /**< corpse object */

/** arrayed, effective for random splash damage */
#define BP_ALL ({ BP_HEAD, BP_LARM, BP_RARM, BP_TORS, BP_LLEG, BP_RLEG })

/** essential body parts to take damage and determine life, if they are gone, victim dies */
#define BP_ESS ({ BP_TORS, BP_HEAD })

/** arms defined, this is for wielding */
#define BP_ARMS ({ BP_RARM, BP_LARM })
/** this is for further expansion with multiple armed beasts, for now,
   we assume everything is right armed */

/** part : percentage of total */
#define BODY_PARTS ([ BP_HEAD :  5, BP_LARM : 10, BP_RARM : 10, BP_TORS : 45, BP_LLEG : 15, BP_RLEG : 15 ])

/** condition bits */
#define H_SUPERB 0 	/**< NOTE: we default to superb */
#define H_LIGHT  1 	/**< set when subject has had light wounding */
#define H_MOD    2 	/**< set when subject has had moderate wounding */
#define H_SEVERE 4 	/**< set when subject has had severe wounding */
#define H_CRIT   8	/**< set when subject has had critical wounding */
#define H_LOST   16 /**< limb is lost, essentials die */
/**< NOTE: lost is a special case */

/** a scale of these conditions */
#define H_SCALE ({ H_CRIT, H_SEVERE, H_MOD, H_LIGHT, H_SUPERB })
/** and colorization */
#define H_COLOR ([ H_LOST : "%^BLACK%^", H_CRIT : "%^RED%^%^BOLD%^", H_SEVERE : "%^RED%^", H_MOD : "%^BOLD%^%^YELLOW%^", H_LIGHT : "%^CYAN%^", H_SUPERB : "%^BOLD%^%^GREEN%^" ])
#define H_PRINT ([ H_SUPERB : "superb", H_LIGHT : "light", H_MOD : "moderate", H_SEVERE : "severe", H_CRIT : "critical", H_LOST : "lost" ])

/** defines when critical hits apply */
#define CRITICAL H_MOD

/** defines states of player.  NOTE: may need long instead of int */
#define S_PRONE 1 /**< player is one ground */
#define S_GRAP  2 /**< player is grappling */
#define S_LOCK  4 /**< player is locked with someone */
#define S_COVER 8 /**< player is under cover */
#define S_MOUNT 16/**< player is mounted */
#define S_FLY   32/**< player is in flight */
#define S_FREE  0 /**< NOTE: we default to free */

/********************************************************************
 * Note on body states:                                             *
 * -These states depict what our player is doing at present.        *
 * -Weapons check these for effectiveness.                          *
 * -Reach weapons will check the range property, different of this. *
 * -Bits for quick resolution.                                      *
 * -Each change of state will issue an event in present objects.    *
 ********************************************************************/


