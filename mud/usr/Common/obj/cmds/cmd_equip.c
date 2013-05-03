/*
 * the equip command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to equip armor.\n";
}

mixed cmd_equip(string cmd, string str, object actor){
    object thing;

    if (!str || !strlen(str)) {
	return "Usage: equip <armor>\n";
    }
    thing = actor->present(str);
    if (thing){

	if((str = catch(actor->set_worn(thing)))){
	    return str;
	}

	if(!thing->query_equip_func() || catch(call_other(thing, thing->query_equip_func()))){/* messages not handled */
	    actor->message("You equip "+thing->query_short()+".\n");
	    actor->query_environment()->message(actor->query_Name()+" equips "+thing->query_short()+".\n", ({ actor }));
	}
	return 1;
    }
    actor->message("No "+str+" to equip.\n");
    return 1;
}