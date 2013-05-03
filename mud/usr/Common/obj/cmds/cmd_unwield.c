/**
 * @brief the unwield command, unwields a weapon
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to unwield weaponry.\n";
}

mixed cmd_unwield(string cmd, string str, object actor){
    object thing;

    if (!str || !strlen(str)) {
	return "Usage: unwield <item>\n";
    }
    thing = actor->present(str);
    if (thing){

	if((str = catch(actor->unwield(thing)))){
	    return str;
	}

	if(!thing->query_unwield_func() || catch(call_other(thing, thing->query_unwield_func()))){/* messages not handled */
	    actor->message("You unwield "+thing->query_short()+".\n");
	    actor->query_environment()->message(actor->query_Name()+" unwields "+thing->query_short()+".\n", ({ actor }));
	}
	return 1;
    }
    actor->message("No "+str+" to unwield.\n");
    return 1;
}