/*
 * the date command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to get machine date.\n";
}

/* date - shows the current date/time of the mud's server */
mixed cmd_date (string cmd, string str, object actor){
    return "Date: " + ctime(time()) + "\n";
}