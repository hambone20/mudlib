/**
 * @brief the wealth command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to see your wealth.\n";
}

mixed cmd_wealth(string cmd, string str, object actor){
    actor->message(actor->query_wealth());
    return 1;
}