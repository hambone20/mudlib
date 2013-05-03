/*
 * the skills command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to see your skills.\n";
}

mixed cmd_skills(string cmd, string str, object actor){
    actor->message(actor->query_skills());
    return 1;
}