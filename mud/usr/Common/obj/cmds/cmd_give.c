/*
 * the give command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to give an item to someone.\n";
}

mixed cmd_give(string cmd, string str, object actor){
    object thing, target, env;
    string name, item;
    string res;

    if(!str || sscanf(str,"%s to %s",item,name) != 2)
	return "Usage: give <item> to <player>\n";

    thing = actor->present(item) ;
    if(!thing)
	return "You don't have a "+item+" to give away.\n";

    env = actor->query_environment();
    target = env->present(name);
    if(!target || !target->is_body())
	return "There is no one named "+capitalize(name)+" to give it to.\n";

    res = catch(thing->move(target));

    if(res){
	actor->message (capitalize(name)+" cannot carry it.\n");
	return 1 ;
    }
    str = thing->query_short();
    actor->message ("You give "+str+" to "+capitalize(name)+".\n");
    name = actor->query_Name();
    target->message(name+" gives you "+str+".\n") ;
    actor->query_environment()->message (name+" gives "+target->query_Name()+" "+str+".\n",
    ({ target, actor }) );
    return 1;
}
