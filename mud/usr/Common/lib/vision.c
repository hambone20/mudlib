/**
 * This module is tied onto living objects.  It provides the functionality of vision, and tries to take
 * passes from the body object to pass through things that the living object should see, or should not
 * see.
 * @todo flush out
 */

int allseeing;  /**< This is an immortal bypass for sight */
string sight_reason; /**< this holds a string for the person's failure to see, defaults to, "You are blind and can't see."
                        a clever wiz might want to make this special to the effect */

void set_allseeing(int i){/* set variable */
	/** @todo restrict calls to this? */
	allseeing = i;
}

int query_allseeing(){ return allseeing; }

