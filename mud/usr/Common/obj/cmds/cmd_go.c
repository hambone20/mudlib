/*
 * the go command
 */
inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Go in a given direction.\n";
}

/** go in a direction */
mixed cmd_go(string cmd, string str, object actor)
{
    object environ, dest;
    string err;

    environ = actor->query_environment ();

    if (!environ)
	return "You must have an environment to leave from!\n";

    dest = environ->query_exit (str);
    if (!dest)
	return "You cannot go in that direction.\n";

    err = catch (actor->move (dest, str));

    if (err)
	return err;

    return 1;
}
