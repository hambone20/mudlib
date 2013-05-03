/*
 * the kill command
 */
 
inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Use this command to attack people or monsters.\n";
}

mixed cmd_kill (string cmd, string str, object actor)
{
    object room, thing;
    string name, tname;

    if (!str)
	return "Usage: kill <person>\n";

    room = actor->query_environment ();
    if (!room)
	return "Where are you?";

    thing = room->present (str);
    if (!thing)
	return "There is no " + str + " to attack.\n";

    if (!thing->query_is_attackable ())
	return "You cannot attack that.\n";

    if (thing == actor)
	return "You masochist.\n";

    tname = thing->query_Name ();
    name = actor->query_Name ();
    switch (actor->attack (thing))
    {
    case 1:
	name = actor->query_Name ();
	actor->message ("You attack " + tname + "!\n");
	actor->query_environment ()->message (name + " attacks " +
	  tname + "!\n", ({ actor, thing }));
	thing->message (name + " attacks you!\n");
	break;
    default:
	actor->message ("You redouble your efforts against " + tname +
	  ".\n");
	break;
    }
    return 1;
}