/*
 * the show command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to show channels.\n";
}

/* channeld */
mixed cmd_show (string cmd, string str, object actor){
    if (!str || !strlen(str))
	return "Usage: show <channel>\n";

    actor->message(CHANNELD->show(str, actor));
    return 1;
}