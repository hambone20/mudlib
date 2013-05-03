/*
 * the inventory command
 */

# include <game/weight.h>

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to check your inventory.\n";
}

mixed cmd_inventory(string cmd, string str, object actor){

    object *stuff;
    string name;
    int i, sz, p_cap, p_volume, max_volume, max_cap;

    p_cap = actor->query_p_cap();
    p_volume = actor->query_p_volume();
    max_cap = actor->query_capacity();
    max_volume = actor->query_volume();

    str = " Carried: ["+p_cap+" / "+max_cap+"] "+WEIGHT_UNIT+"s\n";

    stuff = actor->query_inventory();
    if(!stuff || !(sz=sizeof(stuff))){
	actor->message(str+"\nYou are empty-handed.\n");
	return 1;
    }
    for(i=0; i<sz; i++){
	name = stuff[i]->query_short();
	/* If it has no short it shouldn't show up in your inventory list. */
	if (!name || !strlen(name))
	    continue;

	str += "  "+capitalize(name)+"\n";
    }
    actor->message(str);
    return 1;
}