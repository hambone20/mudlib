/**
 * @brief the alias command
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to get list of aliases or to make an alias.\n";
}

/* alias command - to set and query aliases */
mixed cmd_alias (string cmd, string str, object actor){
	string alias, set, a, b;

	if(!str || !strlen(str)){/* we execute the command to list aliases */
		actor->message("Current aliases:" + actor->query_alias());
		return 1;
	}

	alias = actor->query_alias(explode(str, " ")[0]);
	if(sscanf(str, "%s %s", a, b) < 2){/* alias query */
		if(alias == str){/* no alias */
			return "No such alias.\n";
		}
		actor->message("Alias: " + str + " = " + alias + "\n");
		return 1;
	}

	sscanf(str, "%s %s", str, set);

	/* if we reach here, we're modifying aliases */
	if(!set) return "Usage: alias <name> <value>\n";

	actor->message(actor->add_alias(str, set));
	return 1;
}
