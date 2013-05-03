/*
 * the get command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to get an item.\n";
}

mixed cmd_get(string cmd, string str, object actor){
    object thing, env;
    string s1, s2;
    int num;

    string res;

    if (!str || sscanf(str, "%s from %s", s1, s2) == 2)
	return "Usage: get all | <item> [from <container>]\n";

    env = actor->query_environment() ;
    if(!env) return "No environment.\n";

    if(str == "all"){/* get everything */
	int sz, i;
	object *things;

	things = env->query_inventory();
	if(!things || !(sz=sizeof(things))){
	    actor->message("Nothing to get.\n");
	    return 1;
	}
	while(sz--){
	    if(things[sz]->is_body() || things[sz]->prevent_get())
		continue;

	    i++;
	    res = catch(things[sz]->move(actor));
	    if(res){
		/* nothing for now */
	    }
	}
	if(i){
	    actor->message("You get everything.\n");
	}else{
	    actor->message("Nothing to take.\n");
	}
	return 1;
    }
    /***MONEY***/
    if(sscanf(str, "%d %s", num, s1) == 2 || sscanf(str, "%d %s coin", num, s1) == 2){
	string type;
	thing = env->present(s1);
	if(!thing || !thing->is_coins())
	    return "No money.\n";

	type = thing->query_cointype();
	str = catch(thing->coin_move(actor, num));
	if(!str){
	    actor->message("You get "+num+" "+type+" coin"+( (num > 1) ? "s" : "" )+".\n");
	    actor->query_environment()->message(actor->query_Name()+" gets some money.\n", ({ actor }));
	    return 1;
	}
	return str;
    }

    thing = env->present(str) ;
    if(!thing)
	return "No "+str+" to get.\n";

    /* Check to see if the item is willing to be picked up. */
    if (thing->prevent_get()) {
	actor->message("You cannot pick that up.\n");
	return 1;
    }
    res = catch(thing->move(actor));
    if (res) {
	actor->message("You can't pick it up."+res+"\n");
	return 1;
    }
    str = thing->query_short();
    actor->message("You get the "+str+".\n");
    actor->query_environment()->message(actor->query_Name()+" takes "+str+".\n", ({ actor }));
    return 1;
}