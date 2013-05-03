/**
 * @brief the wield command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to wield weaponry.\n";
}

mixed cmd_wield(string cmd, string str, object actor){
    object thing;

    if (!str || !strlen(str)) {
	return "Usage: wield <item>\n";
    }
    thing = actor->present(str);
    if (thing){

	if((str = catch(actor->wield(thing)))){
	    return str;
	}

	if(!thing->query_wield_func() || catch(call_other(thing, thing->query_wield_func()))){/* messages not handled */
	    actor->message("You wield "+thing->query_short()+".\n");
	    actor->query_environment()->message(actor->query_Name()+" wields "+thing->query_short()+".\n", ({ actor }));
	}
	return 1;
    }
    actor->message("No "+str+" to wield.\n");
    return 1;
}