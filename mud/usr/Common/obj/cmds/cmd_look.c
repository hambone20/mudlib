/*
 * the hp command
 */
inherit BODY_COMMAND;

mixed help(string cmd, object actor){
	return "The look command allows one to look.\n";
}


/** look function */
mixed cmd_look(string cmd, string str, object actor){
    string arg;    
    #if 0
    /* Can we see? */
    if(!actor->query_vision()){
	write("It is dark and you can't see a thing.\n");
	return 1;
    }
    #endif

    if(str && sscanf(str, "in %s", arg))/* let this pass up to container */
	return "look [in] <container>\n";

    if(!str)
	str = "";
    
    if(str == "me" || str == "at me"){
        str = actor->query_name(); 
    }
    /* Was an argument passed? If so, find an object that matches that
     *        string and return its long description.
     *               Hymael - made it play nicer
     *                      Eliminated need for at syntax
     *                             Possible future problem with compound names
     *                                    but for now it works
     *                                           Working for compound names
     *                                                */
    if(sscanf(str, "at %s", arg) || sscanf(str, " %s", arg) || (arg = str)){
	object env, thing;

	/* If no argument was passed, then we're looking in our room. */
	env = actor->query_environment();
	if(!arg ||  implode(explode(" "+arg+" "," "),"") == ""){
	    if(!env){
		actor->message("You are in the void and can see nothing.\n");
		return 1;
	    }
	    actor->message(env->query_long(0, actor));
	    return 1;
	}
	thing = env->present(arg);
	if(thing){
	    actor->message(thing->query_long());
	    return 1;
	}
	thing = actor->present(arg);
	if(thing){
	    actor->message(thing->query_long());
	    return 1;
	}
	actor->message("I don't see "+article(arg)+" "+arg+" here.\n");
	return 1;
    }
    /* Give up. */
    return nil;
}