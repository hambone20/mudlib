/**
 * @brief the xa command, gives health rating on target
 */

inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Used to determine someone's health.\n";
}

mixed cmd_xa (string cmd, string str, object actor){
    object target;
    if (!str || !strlen(str)){/* target an attacker */
	target = actor->query_target();
	if(!target)return "Not currently in combat!\n";

	actor->message(target->query_Name() + ":\n" + target->query_xa() + "\n");

	return 1;
    }

    target = actor->query_environment()->present(str);
    if (target && target->query_has_health()) {
	actor->message(target->query_Name() + ":\n" + target->query_xa() + "\n");
	return 1;
    }
    return "No such thing to xa.\n";
}
