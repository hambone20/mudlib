/**
 * thingd : used for serving up trash items like objects
 * and other things that we want to track and make sure
 * they're removed
 * security is a must
 * @todo move things to a define?
 */

# define THINGS ({ "/usr/Common/obj/object", "/usr/Common/obj/weapon", "/usr/Game/obj/component" })

private object *things;           /**< handles on things */
private string *allowable_things; /**< things we allow to be created */ 

void create(){
    allowable_things = THINGS;
    if(!things)things = ({});
}

/**
 * @brief get_clone returns a new object of type str, checking to make sure its in THINGS.
 */
object get_clone(string str){/** @todo security */
    object thing;
    
    if(!sizeof(allowable_things & ({ str })))error("Cannot do that.\n");
    
    if(!find_object(str))compile_object(str);
    
    thing = clone_object(str);
    
    things += ({ thing });
    
    return thing;
}

object *query_things(){
    return things;
}

void add_allowable(string str){/** @todo security */
    allowable_things += ({ str });
}


