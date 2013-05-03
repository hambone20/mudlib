/*
 * the tune command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to tune channels.\n";
}

mixed cmd_tune (string cmd, string str, object actor){
    if (!strlen (str))
	return "Usage: tune <channel>\n";

    switch(CHANNELD->tune(str, actor)){
    case -1:
	actor->message("You tune out of channel " + str + "\n");
	break;
    case 1:
	actor->message("You tune into channel " + str + "\n");
	break;
    case 0:
	actor->message("You cannot tune into that channel.\n");
    }
    return 1;
}