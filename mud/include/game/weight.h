/**
 * this defines what weight is
 * in this world              
 */
/** the common units this world uses, to be pluralized with a tagged on "s" */
#define WEIGHT_UNIT "stone"

/** for simplicity, this unit will equal 1 pound, or otherwise note here */

/** defaults */
#define DEF_CAPACITY 50 /**< how much weight held */
#define DEF_BULK     50 /**< how much room */

/** for inserting */
#define NOT_CONTAINER 0
#define TOO_BULKY     NOT_CONTAINER - 1
#define TOO_HEAVY     NOT_CONTAINER - 2
#define NO_INS        NOT_CONTAINER - 3 /**< can't insert a container in a container */
#define ALREADY_IN    NOT_CONTAINER - 4 /**< already inside */
#define NOT_INSIDE    NOT_CONTAINER - 5
#define SUCCESS       NOT_CONTAINER + 1
