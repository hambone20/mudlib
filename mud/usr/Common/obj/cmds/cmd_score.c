/*
 * the score command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to see your vital stats.\n";
}

mixed cmd_score(string cmd, string str, object actor){
    mapping stats;
    int i;
    string *attributes;

    stats = actor->query_stats();
    attributes = map_indices(stats);
    i = sizeof(attributes);

    str = actor->query_Name() + "\n";

    while(i--){
	str += "%^BOLD%^%^CYAN%^"+pad(capitalize(attributes[i])+": ", 15)+"%^RESET%^"+stats[attributes[i]]+"\n";
    }
    #ifdef ASTARIA_CLONE
	str += "hp "+actor->query_health()+" / "+actor->query_max_health()+"\n";
    #else
    str += actor->query_diagram();
    #endif
    actor->message(str);
    return 1;
}