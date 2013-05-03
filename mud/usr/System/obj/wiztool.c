# include <kernel/kernel.h>
# include <kernel/user.h>
# include <kernel/access.h>
# include <type.h>
# include <status.h>

# define TAIL_MAX 30

/** add wrapper classes for higher level kernel commands */
inherit LIB_WIZTOOL;

private object user;		/**< associated user object */

/**
 * @brief initialize object
 */
static void create(int clone)
{
    if (clone) {
	::create(200);
	user = previous_object();/**< @todo make sure this is good */
    }
}

/**
 * @brief message pass on a message to the user
 * @param str String of the message
 */
static void message(string str)
{
    user->message(str);
}

/**
 * @brief query for user
 */
object query_user(){
    return user;
}

/**
 * @brief input	deal with input from user
 */
void input(string str)
{
    if (previous_object() == user) {
        call_limited("process", str);
    }
}

/**
 * @brief process() process user input
 * @param str String to process
 */
static void process(string str){
    string arg, err;

    /** @todo add in redirect to any object, sort of like editor? */
    if (query_editor(this_object())) {
	if (strlen(str) != 0 && str[0] == '!') {
	    str = str[1 ..];
	} else {
	    str = editor(str);
	    if (str) {
            message(str);
	    }
	    return;
	}
    }

    if (str == "") {
	return;
    }

    sscanf(str, "%s %s", str, arg);
    if (arg == "") {
	arg = nil;
    }

    switch (str) {
    case "code":
    case "history":
    case "clear":
    case "compile":
    case "clone":
    case "destruct":
    case "dest":
    case "actions":

    case "cd":
    case "pwd":
    case "ls":
    case "cp":
    case "mv":
    case "rm":
    case "mkdir":
    case "rmdir":
    case "ed":

    case "access":
    case "grant":
    case "ungrant":
    case "quota":
    case "rsrc":

    case "people":
    case "status":
    case "swapout":
    case "statedump":
    case "shutdown":
    case "reboot":

    case "od_report":
    case "full_rebuild":

    case "spectrum":
    case "summon":
    case "goto":
    case "call":
    case "tail":
    case "head":
    case "gauge":
    case "scan":
	err = call_other(this_object(), CMD_PREFIX + str, user, str, arg);
	if(err)LOGD->log(str + " caused error in "+user->query_name()+": "+err, "wiztool");
	break;

    default:
	message("No command: " + str + "\n");
	break;
    }
}

/**
 * @brief cmd_summon() summon player to you
 */
static void cmd_summon(object user, string cmd, string str)
{
    object player, *users; /** @brief hackish for now */
    int i;

    if (!str) {
	message("Usage: " + cmd + " <player>\n");
	return;
    }

    str = lowercase(str);
    users = users();
    if(!(i = sizeof(users)))
	return;

    while(i--){/** @todo code to use find_player() */
	if(users[i]->query_name() == str){
	    player = users[i]->query_body();
	    break;
	}
    }
    if(!player){
	message("No player named <"+capitalize(str)+">\n");
	return;
    }
    player->message(this_user()->query_Name()+" has summoned you!\n");
    player->move(this_user()->query_body()->query_environment(), "", 1);
    message("You summon " + player->query_Name()+".\n");
}

/**
 * @brief cmd_spectrum Display color spectrum to wiz
 * @todo change this over to pinkfish
 */
static void cmd_spectrum(object user, string cmd, string str)
{
    int i;
    string ret;

    ret = "Spectrum:\n";

    for(i=0; i<48; i++){
	ret += "\033["+i+"mcolor{"+i+"}\033[1m{bold}\033[0m\n";
    }
    message(ret);
}

/**
 * @brief cmd_clone() clone an object, move it to cloners dir
 */
static void cmd_clone(object user, string cmd, string str)
{
    mixed obj;

    obj = parse_obj(str);
    switch (typeof(obj)) {
    case T_INT:
	message("Usage: " + cmd + " <obj> | $<ident>\n");
    case T_NIL:
	return;

    case T_STRING:
	str = obj;
	obj = find_object(str);
	break;

    case T_OBJECT:
	str = object_name(obj);
	break;
    }

    if (sscanf(str, "%*s" + CLONABLE_SUBDIR + "%*s#") != 1) {
	message("Not a master object.\n");
    } else if (!obj) {
	message("No such object.\n");
    } else {
	str = catch(obj = clone_object(str));
	if (str) {
	    message(str + ".\n");
	} else if (obj) {
	    store(obj);
	    if(obj->is_object()){
		cmd = catch(obj->move(user->query_body()));
		if(cmd)
            str = catch(obj->move(user->query_body()->query_environment()));
            if(str)
            {
                message("Error: Object couldn't be moved - "+cmd+".\n");
            }
	    }
	}
    }
}

/**
 * @brief cmd_ls() override of inherited, list files
 */
static void cmd_ls(object user, string cmd, string str)
{
    mixed *files, *objects;
    string *names, timestr, dirlist, directory;
    int *sizes, *times, long, ancient, i, j, sz, max, len, rows, time, adjust;

    if (!str) {
	str = ".";
    } else if (sscanf(str, "-%s", str) != 0) {
	long = 1;
	if (str == "l") {
	    str = ".";
	} else if (sscanf(str, "l %s", str) == 0) {
	    message("Usage: " + cmd + " [-l] [<file> ...]\n");
	    return;
	}
    }

    directory = query_directory();

    files = expand(str, 1, FALSE);	/* must exist, short file names */

    if (files[4] == 1 && sizeof(files[0]) == 1 && files[1][0] == -2) {
	str = files[0][0];
	if (str[0] != '/') {
	    str = directory + "/" + str;
	}
	files = get_dir(str + "/*");
	if (!files) {
	    message(str + ": Access denied.\n");
	    return;
	}
    }

    names = files[0];
    sz = sizeof(names);
    if (sz == 0) {
	return;
    }
    sizes = files[1];
    times = files[2];
    objects = files[3];

    for (i = 0; i < sz; i++) {
	j = strlen(names[i]);
	if (j > max) {
	    max = j;
	}
	j = sizes[i];
	if (j > len) {
	    len = j;
	}
    }
    if (long) {
	len = strlen((string) len) + 1 + 11;/**< @todo check this */
	max += len + 14;
	ancient = time() - 6 * 30 * 24 * 60 * 60;
    }

    max += 2;
    j = (79 + 2) / (max + 1);
    if (j == 0) {
	rows = sz;
    } else {
	rows = (sz + j - 1) / j;
    }

    dirlist = "";
    for (i = 0; i < rows; i++) {
	j = i;
	for (;;) {
	    if (long) {
		str = "                        ";
		if (sizes[j] >= 0) {
		    str += (string) sizes[j];
		}

		time = times[j];
		timestr = ctime(time);
		if (time >= ancient) {
		    timestr = timestr[3 .. 15];
		} else {
		    timestr = timestr[3 .. 10] + timestr[19 .. 23];
		}
		str = str[strlen(str) - len ..] + timestr + " " + names[j];
	    } else {
		str = names[j];
	    }

	    if (sizes[j] < 0) {
		str = "%^BOLD%^%^BLUE%^" + str + "/%^RESET%^";
		adjust = 25;
	    } else if (objects[j]) {
		str = "%^BOLD%^%^GREEN%^" + str + "*%^RESET%^";
		adjust = 26;
	    } else {
		adjust = 0;
	    }
	    j += rows;
	    if (j >= sz) {
		dirlist += str + "\n";
		break;
	    }
	    dirlist += (str + "                                                    ")
	    [0 .. (max + adjust)];
	}
    }
    message(dirlist);
}

/**
 * @brief dest destructs an object in wizard's inventory or environ
 */
static void cmd_dest(object user, string cmd, string str){
    mixed thing;
    int i;

    if(!str || !strlen(str)){
	message("Usage: dest <object>\n");
	return;
    }
    /** @todo add in check for $num functionality */
    if (sscanf(str, "$%d", i) && (thing = parse_obj(str))){
	message("You destruct : "+thing->query_short()+".\n"); /** @todo may have to make query_short more elaborate */
	destruct_object(thing);
	return;
    }

    /** check inventory */
    if(thing = user->query_body()->present(str)){
	message("You destruct : "+thing->query_short()+".\n");
	user->query_body()->query_environment()->message(user->query_Name()+" dests: "+thing->query_short()+"\n", ({user->query_body()}));
	destruct_object(thing);
	return;
    }

    /** check environment */
    if(thing = user->query_body()->query_environment()->present(str)){
	message("You destruct : "+thing->query_short()+".\n");
	user->query_body()->query_environment()->message(user->query_Name()+" dests: "+thing->query_short()+"\n", ({user->query_body()}));
	destruct_object(thing);
	return;
    }
    message("No "+str +" to dest.\n");
}

/**
 * @brief actions finds actions performable by an object
 */
static void cmd_actions(object user, string cmd, string str){
    mixed thing;
    mapping coms;
    string *vals;
    int i, sz;

    if(!str || !strlen(str)){
	message("Usage: actions <object>\n");
	return;
    }
    
    if(str == "here" && (thing = user->query_body()->query_environment())){
	 coms = thing->query_commands();
    if(!coms)
    {
        message("No commands here.\n");
        return;
    }
    vals = map_values(coms);
    message(thing->query_short() + " has commands:\n");
    for(i=0, sz=sizeof(vals);i < sz; i++)
    {
        if(vals[i] && coms[vals[i]])
            message("Com: "+vals[i]+" -> "+coms[vals[i]]+"\n");
    }
	return;
    }
    /** @todo add in check for $num functionality */
    if (sscanf(str, "$%d", i) && (thing = parse_obj(str))){
    coms = thing->query_commands();
    if(!coms)
    {
        message(thing->query_short() + " has no commands.\n");
        return;
    }
    vals = map_values(coms);
    message(thing->query_short() + " has commands:\n");
    for(i=0, sz=sizeof(vals);i < sz; i++)
    {
       if(vals[i] && coms[vals[i]])
            message("Com: "+vals[i]+" -> "+coms[vals[i]]+"\n");
    }
	return;
    }

    /** check inventory */
    if(thing = user->query_body()->present(str)){
    coms = thing->query_commands();
    if(!coms)
    {
        message(thing->query_short() + " has no commands.\n");
        return;
    }
    vals = map_values(coms);
    message(thing->query_short() + " has commands:\n");
    for(i=0, sz=sizeof(vals);i < sz; i++)
    {
        if(vals[i] && coms[vals[i]])
            message("Com: "+vals[i]+" -> "+coms[vals[i]]+"\n");
    }
	return;
    }

    /** check environment */
    if(thing = user->query_body()->query_environment()->present(str)){
	 coms = thing->query_commands();
    if(!coms)
    {
        message(thing->query_short() + " has no commands.\n");
        return;
    }
    vals = map_values(coms);
    message(thing->query_short() + " has commands:\n");
    for(i=0, sz=sizeof(vals);i < sz; i++)
    {
        if(vals[i] && coms[vals[i]])
            message("Com: "+vals[i]+" -> "+coms[vals[i]]+"\n");
    }
	return;
    }
    
    message("No "+str +" to lookup actions on.\n");
}

/** 
* @todo more: pager for a file, make wiztool input_to_obj?
*/

/**
* @todo home: returns you to your workroom
*/

/**
 * @brief goto: trans you to the named room, or player
 */
static void cmd_goto(object user, string cmd, string str){
    mixed thing;
    int i;

    if(!str || !strlen(str)){
	message("Usage: goto <object/room>\n");
	return;
    }
    /** @todo add in check for $num functionality */
    if (sscanf(str, "$%d", i) && (thing = fetch(i))){
	if(thing->is_room()){/**< suitable destination */
	    user->query_body()->move(thing, "", 1);
	    message("You move to " + thing->query_short() + ".\n");
	    return;
	}
    }else{
	thing = find_player(thing);
	if(thing && thing->is_player()){
	    user->query_body()->move(thing->query_environment(), "", 1);
	    message("You move to " + thing->query_Name() + "'s room.\n");
	    thing->message(this_player()->query_Name() + " appears in your room.\n");
	    /** @todo add message to others */
	    return;
	}
	/** @todo add in instance of file */

    }
    message("Not a suitable destination.\n");/** @todo may have to make query_short more elaborate */
    return;
    message("No "+str +" to dest.\n");
}

/**
* @todo invis: turns you invis, requires the coding of a query_vision
*/

/**
* @todo force: forces a body to do a command
*/

/**
* @todo nuke: destroys a given player/wiz
*/

/**
* @todo wall: realm echo
*/

/**
 * @brief helper functions
 */
static mixed get_value(string str)
{
    mixed result;
    object obj;
    string name, parsed;

    parsed = str;
    name = USR_DIR + "/" + query_owner() + "/_code";
    obj = find_object(name);
    if (obj) {
	destruct_object(obj);
    }
    if (!parsed) {
	return nil;
    }

    parsed = "return "+parsed;
    str = "# include <float.h>\n# include <limits.h>\n" +
    "# include <status.h>\n# include <trace.h>\n" +
    "# include <type.h>\n" + str + "\n" +
    "mixed exec(object user) {\n" +
    "    mixed a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z;\n\n" +
    "    " + parsed + "\n}\n";
    str = catch(obj = compile_object(name, str),
      result = obj->exec(user));
    if (str) {
	message("Error: " + str + ".\n");
    }
    return result;
}


/**
 * @brief helper call
 * @retval return array with ({ mixed value, TYPE })
 * @todo trouble materializing mappings and such
 */ 
mixed parse_call(string str){
    string parsed;
    mixed ret;
    int i;
    float x;

    str = strip(str);
    str = strip(str, '"');
    if(str == "me")return ({ user->query_body(), T_OBJECT });

    if(str == "here")return ({ user->query_body()->query_environment(), T_OBJECT });

    if(sscanf(str, "$%d", i))return ({ fetch(i), T_OBJECT });

    if(find_player(str))return ({ find_player(str), T_OBJECT });

    if(sscanf(str, "#%d", i) || sscanf(str, "/%s", parsed)){/**< treat as file, unless nil */
	return ({ find_object(str), T_OBJECT });
    }

    ret = get_value(str); 
    return ({ ret, typeof(ret) });
}

/**
 * @brief cmd_call wiz can call an object or himself and execute a functionality
 * @todo Fix this up
 */
static void cmd_call(object user, string cmd, string str){
    string id, function, args, *calls;
    mixed ret, obj, obj2;

    if(!str || !strlen(str) || !sizeof(calls = explode(str, ";"))){
	message("Usage: call <object $/here/name of player/me>;function(;argument)\n");
	return;
    }
    /** @todo strip leading and ending spaces */
    /* calls populated by ; exploded array */
    switch(sizeof(calls)){
    case 1:/* just have one object to deal with, just store it in history */
	store(parse_call(calls[0])[0]);/* need more elaborate? */
	return;
    case 2:/* we have form call obj;function */
	/* load up call[0] */
	obj = parse_call(calls[0]);
	if(obj[1] != T_OBJECT){
	    message("Invalid object.\n");
	    return;
	}

	if(catch(function = (string)strip(strip(calls[1]), '"'))){
	    message("Function not string.\n");
	    return;
	}	
	if(!function_object(function, (object)obj[0])){
	    message("No function: \""+function+"\" in object.\n");
	    return;
	}
	str = catch(ret = call_other((object)obj[0], function));
	if(str){
	    message("Error: "+str);
	    return;
	}
	if(ret)
	    store(ret);

	return;

    case 3:/* we have form obj;function;arg */
	/* load up call[0] */
	obj = parse_call(calls[0]);
	if(obj[1] != T_OBJECT){
	    message("Invalid object.\n");
	    return;
	}

	if(catch(function = (string)strip(strip(calls[1]), '"'))){
	    message("Function not string.\n");
	    return;
	}	

	if(!function_object(function, (object)obj[0])){
	    message("No function: \""+function+"\"in object.\n");
	    return;
	}
	obj2 = parse_call(calls[2]);
	switch(obj2[1]){
	case T_OBJECT:
	    str = catch(ret = call_other((object)obj[0], function, (object)obj2[0]));
	    break;
	case T_INT:
	    str = catch(ret = call_other((object)obj[0], function, (int)obj2[0]));
	    break;
	case T_FLOAT:
	    str = catch(ret = call_other((object)obj[0], function, (float)obj2[0]));
	    break;
	case T_STRING:
	    str = catch(ret = call_other((object)obj[0], function, (string)obj2[0]));
	    break;
	case T_ARRAY:
	    str = catch(ret = call_other((object)obj[0], function, obj2[0]));
	    break;
	case T_MAPPING:
	    str = catch(ret = call_other((object)obj[0], function, (mapping)obj2[0]));
	    break;
	default:
	    str = catch(ret = call_other((object)obj[0], function, obj2[0]));

	}
	if(str){
	    message("Error: "+str);
	    return;
	}
	if(ret)
	    store(ret);

	return;
    }
    message("Usage: call <object $/here/name of player/me>;function(;argument)\n");
}

/**
 * @brief tail: prints the last bit of a file
 */
static void cmd_tail(object user, string cmd, string str){
    string *lines;
    int i, sz;

    lines = explode(read_file(str), "\n");
    sz = sizeof(lines);
    if(sz <= TAIL_MAX){
	message(implode(lines, "\n"));
    }
    i = sz-TAIL_MAX-1;
    message(implode(lines[i..], "\n") + "\n");
}

/**
 * @brief head: prints the first bit of a file
 */
static void cmd_head(object user, string cmd, string str){
    string *lines;
    int i, sz;

    lines = explode(read_file(str), "\n");
    sz = sizeof(lines);
    if(sz <= TAIL_MAX){
	message(implode(lines, "\n"));
    }
    i = TAIL_MAX-1;
    message(implode(lines[..i], "\n") + "\n");
}
/**
 * @brief gauge: calculates the given ticks/time of the given command
 */
static void cmd_gauge(object user, string cmd, string str){
    int ticks, temp;
    float time, temp2;


    time = (float)millitime()[0] + millitime()[1];      /* time at start */
    ticks = status()[ST_TICKS]; /* ticks at beginning */

    user->receive_message(str);

    temp = status()[ST_TICKS]; /* for accurate resolution */
    temp2 = (float)millitime()[0] + millitime()[1];
    ticks -= temp;
    time = temp2 - time;
    message("\n\n<command "+str+">\nTime:  "+time+"\nTicks:  "+ticks+"\n");
}
/**
 * @brief scan inventory for objects
 */
static void cmd_scan(object user, string cmd, string str, varargs int recurse){
    object env;
    object *contents;
    int i, sz, deep;

    if(str && strlen(str) >= 2 && str[..1] == "-d"){
	deep = 1; /* trigger deep scan */
    }

    if(!recurse){
	env = user->query_body()->query_environment();
	recurse = 0;
    } else {
	env = user;
    }

    if (!env) {
	return;
    }
    contents = env->query_inventory() ;
    /* There has to be contents since the player is there. */
    message(env->query_short()+"\n");/* add tabs? */
    for (i=0,sz=sizeof(contents);i<sz;i++) {
	store(contents[i]);
	if(deep && contents[i]->query_inventory() && sizeof(contents[i]->query_inventory()))
	    cmd_scan(contents[i], cmd, str, ++recurse);

    }
}

/**
 * @brief cmd_od_report report on object
 */
static void cmd_od_report(object user, string cmd, string str)
{
    int    i, hmax;
    mixed  obj;
    string report;

    if(!find_object(OBJECTD))
	compile_object(OBJECTD);

    hmax = sizeof(query_history());

    i = -1;
    if(!str || (sscanf(str, "$%d%s", i, str) == 2 &&
    (i < 0 || i >= hmax || str != ""))) {
	message("Usage: " + cmd + " <obj> | $<ident>\n");
	return;
    }

    if (i >= 0) {
	obj = object_name(fetch(i));
	if(typeof(obj) != T_OBJECT && typeof(obj) != T_STRING) {
	    message("Not an object.\n");
	    return;
	}
    } else if (sscanf(str, "$%s", str)) {
	obj = ident(str);
	if (!obj) {
	    message("Unknown: $ident.\n");
	    return;
	}
    } else if (sscanf(str, "#%*d")) {
	obj = str;
    } else if (sscanf(str, "%*d")) {
	obj = str;
    } else {
	obj = DRIVER->normalize_path(str, query_directory(), query_owner());
    }

    str = catch(report = OBJECTD->report_on_object(obj));
    if(str) {
	str += "\n";
    } else if (!report) {
	str = "Nil report from Object Manager!\n";
    } else {
	str = report;
    }

    message(str);
}

/**
 * @brief cmd_full_rebuild recompile auto
 * @todo more calls to objectd
 */
static void cmd_full_rebuild(object user, string cmd, string str) {
    if(str && !strlen(str)) {
	user->message("Usage: " + cmd + "\n");
	return;
    }

    if(!access(user->query_name(), "/", FULL_ACCESS)) {
	user->message("Currently only those with full administrative access "
	  + "may do a full rebuild.\n");
	return;
    }

    user->message("Recompiling auto object...\n");

    catch {
	OBJECTD->recompile_auto_object(user);
    } : {/*
	  if(ERRORD->last_compile_errors()) {
	    user->message("===Compile errors:\n" + ERRORD->last_compile_errors());
	    user->message("---\n");
	  }

	  if(ERRORD->last_runtime_error()) {
	    if(sscanf(ERRORD->last_runtime_error(),
		      "%*sFailed to compile%*s") == 2) {
	      return;
	    }*/
	/*
	      user->message("===Runtime error: '" + ERRORD->last_runtime_error()
			    + "'.\n");*/
	user->message("there was an error\n");
	/*}
    /*
	if(ERRORD->last_stack_trace()) {
	  user->message("===Stack trace: '" + ERRORD->last_stack_trace()
			+ "'.\n");
	  user->message("---\n");
	}*/

	return;
    }

    user->message("Done.\n");
}

