/**
 * @brief the unequip command, unequips the given armor piece
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to unequip armor.\n";
}

mixed cmd_unequip(string cmd, string str, object actor){
    object thing;

    if (!str || !strlen(str)) {
	return "Usage: unequip <armor>\n";
    }
    thing = actor->present(str);
    if (thing){

	if((str = catch(actor->remove_worn(thing)))){
	    return str;
	}

	if(!thing->query_unequip_func() || catch(call_other(thing, thing->query_unequip_func()))){/* messages not handled */
	    actor->message("You unequip "+thing->query_short()+".\n");
	    actor->query_environment()->message(actor->query_Name()+" unequips "+thing->query_short()+".\n", ({ actor }));
	}
	return 1;
    }
    actor->message("No "+str+" to unequip.\n");
    return 1;
}