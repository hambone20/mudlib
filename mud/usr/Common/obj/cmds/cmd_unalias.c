/*
 * the unalias command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used remove an alias.\n";
}

/* remove given alias */
mixed cmd_unalias (string cmd, string str, object actor){
	if(!str || !strlen(str)) return "Usage: unalias <alias>\n";

	actor->message(actor->remove_alias(str));
	return 1;
}