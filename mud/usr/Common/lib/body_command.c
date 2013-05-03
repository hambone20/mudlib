/**
 * for now this is kind of stubby, but all body commands will inherit it for future
 * combining of functionalities
 * we can add in event's and other things in here to intercept calls or change behavior
 */

mixed do_cmd(string cmd, string args, object actor){
    /* do test for -h form, in which case, this should return helpfile */
	if(strlen(args) && args == "-h"){/* help entry */
		return this_object()->get_help(cmd, args, actor);
	}
	LOGD->log("In command "+cmd+" with "+args, "command");
	return call_other(this_object(), CMD_PREFIX+cmd, cmd, args, actor);
}

mixed get_help(string cmd, string str, object actor){
    /* search for help in the file first, then in help directories */
    if(function_object("help", this_object())){/* we've found help */
        return this_object()->help(cmd, actor);
    }
    return "No help for now.\n";
}