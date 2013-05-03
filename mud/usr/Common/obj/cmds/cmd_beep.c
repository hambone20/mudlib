/*
 * the beep command
 */
 
#define BEEP ""
inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "Use this to make beeping noises, but don't be annoying.\n";
}

mixed cmd_beep (string cmd, string str, object actor)
{
    object person;

    if (!str || !strlen (str))
    {
	actor->message (BEEP + "You beep.\n");
	actor->query_environment ()->message (BEEP +
	  actor->query_Name () +
	  " beeps.\n", ({ actor }));
	return 1;
    }

    person = actor->query_environment ();

    if (!person)
	return "No room to beep!\n";

    person = person->present (str);

    if (!person)
	return "No one by that name to beep.\n";

    actor->message (BEEP + "You beep " + person->query_Name () +
      ".\n");
    actor->query_environment ()->message (BEEP +
      actor->
      query_Name () + " beeps " +
      person->query_Name () +
      ".\n", ({ person,	actor }));

    person->message (BEEP + actor->query_Name () + " beeps you.\n");

    return 1;
}