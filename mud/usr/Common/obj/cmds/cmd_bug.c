/**
 * the bug command
 * @todo get working
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to report a bug.\n";
}

/* rudimentary bug reporting */
mixed cmd_bug (string cmd, string str, object actor){
	actor->message("working on it\n");
	editor("a /logs/bugs.log");
	return 1;
}