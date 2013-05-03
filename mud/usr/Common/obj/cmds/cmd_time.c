/**
 * the time command
 * @todo debug 0 minutes and seconds
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to get game time.\n";
}

/* channeld */
mixed cmd_time (string cmd, string str, object actor){
	mapping time;
	time = "/usr/Common/sys/weatherd"->get_date();
    actor->message("/usr/Common/sys/weatherd"->format_time(time)+"\n");
    return 1;
}