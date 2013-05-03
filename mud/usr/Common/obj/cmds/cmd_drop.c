/*
 * the drop command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to drop an item.\n";
}

mixed cmd_drop(string cmd, string str, object actor){
    object thing;
    string res, type;
    int num;

    if (!str)
	return "Usage: drop <item>\n";

    /***COINS***/
    if(sscanf(str, "%d %s coin", num, type) == 2 || sscanf(str, "%d %s", num, type) == 2){
	str = catch(actor->wealth_move(actor->query_environment(), type, num));
	if(!str){
	    actor->message("You drop "+num+" "+type+".\n");
	    actor->query_environment()->message(actor->query_Name()+" drops "+num+" "+type+".\n", ({actor}));
	    return 1;
	}
	return str;
    }
    /***END COINS***/
    thing = actor->present(str) ;
    if(!thing)
	return "You don't possess "+article(str)+" "+str+".\n";

    res = catch(thing->move(actor->query_environment()));
    if(res){
	actor->message("You can't bring yourself to drop it. "+res+"\n") ;
	return 1;
    }
    str = thing->query_short();
    actor->message("You drop the "+str+".\n") ;
    actor->query_environment()->message(actor->query_Name()+" drops "+str+".\n", ({actor}));
    return 1;
}