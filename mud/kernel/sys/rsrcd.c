# include <kernel/kernel.h>
# include <kernel/rsrc.h>
# include <kernel/objreg.h>
# include <type.h>

# define CO_OBJ		0	/* callout object */
# define CO_HANDLE	1	/* handle in object */
# define CO_RELHANDLE	2	/* release handle */
# define CO_PREV	3	/* previous callout */
# define CO_NEXT	4	/* next callout */


mapping resources;		/* registered resources */
mapping owners;			/* resource owners */
mapping olimits;		/* resource limit per owner */
int downtime;			/* shutdown time */
mapping suspended;		/* suspended callouts */
mixed *first_suspended;		/* first suspended callout */
mixed *last_suspended;		/* last suspended callout */
object suspender;		/* object that suspended callouts */
int suspend;			/* callouts suspended */
object manager;			/* suspension manager */

/*
 * NAME:	create()
 * DESCRIPTION:	initialize resource mappings
 */
static void create()
{
    /* initial resources */
    resources = ([
      "callouts" :	({ -1,  0,    0 }),
      "objects" :	({ -1,  0,    0 }),
      "events" :	({ -1,  0,    0 }),
      "stack" :		({ -1,  0,    0 }),
      "ticks" :		({ -1,  0,    0 }),
      "tick usage" :	({ -1, 10, 3600 }),
      "filequota" :	({ -1,  0,    0 }),
      "editors" :	({ -1,  0,    0 }),
      "create stack" :	({ -1,  0,    0 }),
      "create ticks" :	({ -1,  0,    0 }),
    ]);

    owners = ([ ]);		/* no resource owners yet */
    olimits = ([ ]);
}

/*
 * NAME:	add_owner()
 * DESCRIPTION:	add a new resource owner
 */
void add_owner(string owner)
{
    if (KERNEL() && !owners[owner]) {
	object obj;

	rlimits (-1; -1) {
	    obj = clone_object(RSRCOBJ);
	    obj->set_resources(map_indices(resources) &
		({ "stack", "ticks", "tick usage" }) );
	    catch {
		owners[owner] = obj;
		obj->set_owner(owner);
		if (resources["objects"]) {
		    owners["System"]->rsrc_incr("objects", nil, 1,
			resources["objects"], TRUE);
		}
		olimits[owner] = ({ -1, -1, 0 });
	    } : {
		destruct_object(obj);
	    }
	}
	if (!obj) {
	    error("Too many resource owners");
	}
    }
}

/*
 * NAME:	remove_owner()
 * DESCRIPTION:	remove a resource owner
 */
void remove_owner(string owner)
{
    object obj;
    string *names;
    mixed **rsrcs, *rsrc;
    int i;

    if (previous_program() == API_RSRC && (obj=owners[owner])) {
	names = map_indices(resources);
	rsrcs = map_values(resources);
	for (i = sizeof(names); --i >= 0; ) {
	    rsrc = obj->rsrc_get(names[i], rsrcs[i]);
	    if (rsrc[RSRC_DECAY] == 0 && (int) rsrc[RSRC_USAGE] != 0) {
		error("Removing owner with non-zero resources");
	    }
	}

	rlimits (-1; -1) {
	    destruct_object(obj);
	    olimits[owner] = nil;
	}
    }
}

/*
 * NAME:	query_owners()
 * DESCRIPTION:	return a list of resource owners
 */
string *query_owners()
{
    if (previous_program() == API_RSRC) {
	return map_indices(owners);
    }
}


/*
 * NAME:	set_rsrc()
 * DESCRIPTION:	set the maximum, decay percentage and decay period of a
 *		resource
 */
void set_rsrc(string name, int max, int decay, int period)
{
    if (KERNEL()) {
	mixed *rsrc;

	rsrc = resources[name];
	if (rsrc) {
	    /*
	     * existing resource
	     */
	    if ((rsrc[GRSRC_DECAY] == 0) != (decay == 0)) {
		error("Cannot change resource decay");
	    }
	    rlimits (-1; -1) {
		rsrc[GRSRC_MAX] = max;
		rsrc[GRSRC_DECAY] = decay;
		rsrc[GRSRC_PERIOD] = period;
	    }
	} else {
	    /* new resource */
	    switch(name) {
	    case "callouts":
	    case "objects":
	    case "events":
	    case "stack":
	    case "ticks":
	    case "filequota":
	    case "editors":
	    case "create stack":
	    case "create ticks":
		if (decay != 0) {
		    error("Invalid decay for predefined resource");
		}
		break;
	    }
	    resources[name] = ({ max, decay, period });
	}
    }
}

/*
 * NAME:	remove_rsrc()
 * DESCRIPTION:	remove a resource
 */
void remove_rsrc(string name)
{
    int *rsrc, i;
    object *objects;

    if (previous_program() == API_RSRC && (rsrc=resources[name])) {
	objects = map_values(owners);
	i = sizeof(objects);
	rlimits (-1; -1) {
	    while (i != 0) {
		objects[--i]->remove_rsrc(name);
	    }
	    resources[name] = nil;
	}
    }
}

/*
 * NAME:	query_rsrc()
 * DESCRIPTION:	get usage and limits of a resource
 */
mixed *query_rsrc(string name)
{
    if (previous_program() == API_RSRC) {
	mixed *rsrc, usage;
	object *objects;
	int i;

	rsrc = resources[name];

	if (!rsrc) {
	    return ({ 0, -1, 0, 0, 0 });
	}

	rsrc = rsrc[..];
	objects = map_values(owners);
	usage = (rsrc[GRSRC_DECAY] == 0) ? 0 : 0.0;
	for (i = sizeof(objects); --i >= 0; ) {
	    usage += objects[i]->rsrc_get(name, resources[name])[RSRC_USAGE];
	}

	return ({ usage, rsrc[GRSRC_MAX], 0 }) +
	       rsrc[GRSRC_DECAY .. GRSRC_PERIOD];
    }
}

/*
 * NAME:	query_resources()
 * DESCRIPTION:	return a list of resources
 */
string *query_resources()
{
    if (previous_program() == API_RSRC) {
	return map_indices(resources);
    }
}


/*
 * NAME:	rsrc_set_limit()
 * DESCRIPTION:	set individual resource limit
 */
void rsrc_set_limit(string owner, string name, int max)
{
    if (previous_program() == API_RSRC) {
	object obj;

	if (!(obj=owners[owner])) {
	    error("No such resource owner: " + owner);
	}

	if (!resources[name]) {
	    error("No such resource");
	}

	obj->rsrc_set_limit(name, max, resources[name][GRSRC_DECAY]);
    }
}

/*
 * NAME:	rsrc_get()
 * DESCRIPTION:	get individual resource usage
 */
mixed *rsrc_get(string owner, string name)
{
    if (KERNEL()) {
	object obj;

	if (!(obj=owners[owner])) {
	    error("No such resource owner: " + owner);
	}

	if (!resources[name]) {
	    return ({ 0, -1, 0, 0, 0 });
	}

	return obj->rsrc_get(name, resources[name]);
    }
}

/*
 * NAME:	rsrc_incr()
 * DESCRIPTION:	increment or decrement a resource, returning TRUE if succeeded,
 *		FALSE if failed
 */
int rsrc_incr(string owner, string name, mixed index, int incr,
	      varargs int force)
{
    if (KERNEL()) {
	object obj;

	if (!(obj=owners[owner])) {
	    error("No such resource owner: " + owner);
	}

	if (!resources[name]) {
	    /* successful no-op */
	    return TRUE;
	}

	return obj->rsrc_incr(name, index, incr, resources[name], force);
    }
}

/*
 * NAME:	call_limits()
 * DESCRIPTION:	handle stack and tick limits for _F_call_limited
 */
mixed *call_limits(mixed *previous, string owner, int stack, int ticks)
{
    if (previous_program() == AUTO) {
	int maxstack, maxticks, time, *limits;

	limits = olimits[owner];

	/* determine available stack */
	maxstack = limits[LIM_MAX_STACK];
	if (maxstack < 0) {
	    if (resources["stack"]) {
		maxstack = resources["stack"][GRSRC_MAX];
	    } else {
		maxstack = -1;
	    }
	}
	if (maxstack > stack && stack >= 0) {
	    maxstack = stack;
	}
	if (maxstack >= 0) {
	    maxstack++;
	}

	/* determine available ticks */
	maxticks = limits[LIM_MAX_TICKS];
	if (maxticks < 0) {
	    if (resources["ticks"]) {
		maxticks = resources["ticks"][GRSRC_MAX];
	    } else {
		maxticks = -1;
	    }
	} else {
	    int *usage;

	    if (resources["tick usage"]) {
		usage = resources["tick usage"];
		if ((time=time()) - limits[LIM_MAX_TIME] >= usage[GRSRC_PERIOD]) {
		    /* decay ticks */
		    owners[owner]->decay_ticks(limits, time, usage);
		    maxticks = limits[LIM_MAX_TICKS];
		}
	    }
	}
	if (maxticks > ticks - 25 && ticks >= 0) {
	    maxticks = ticks - 25;
	    if (maxticks <= 0) {
		maxticks = 1;
	    }
	}

	return ({ previous, owner, maxstack, maxticks, ticks });
    }
}

/*
 * NAME:	set_rlimits()
 * DESCRIPTION:	set limits for call_limited
 */
void set_rlimits(string owner, int *limits)
{
    if (previous_program() == RSRCOBJ) {
	olimits[owner] = limits;
    }
}

/*
 * NAME:	update_ticks()
 * DESCRIPTION:	update ticks after execution
 */
int update_ticks(mixed *limits, int ticks)
{
    if (KERNEL()) {
	if (limits[LIM_MAXTICKS] > 0 &&
	    (!limits[LIM_NEXT] ||
	     limits[LIM_OWNER] != limits[LIM_NEXT][LIM_OWNER])) {
	    ticks = limits[LIM_MAXTICKS] - ticks;
	    if (ticks < 0) {
		return -1;
	    }
	    if (resources["tick usage"]) {
		owners[limits[LIM_OWNER]]->update_ticks(
		    ticks, resources["tick usage"]);
	    }
	    ticks = (limits[LIM_NEXT] && limits[LIM_TICKS] >= 0) ?
		     limits[LIM_NEXT][LIM_MAXTICKS] -= ticks : -1;
	}
	return ticks;
    }
}


/*
 * NAME:	set_suspension_manager()
 * DESCRIPTION:	set an external manager to handle suspended callouts
 */
void set_suspension_manager(object obj)
{
    if (SYSTEM()) {
	if (suspend != 0) {
	    error("Cannot change suspension manager with suspended callouts");
	}
	manager = obj;
    }
}

/*
 * NAME:	suspend_callouts()
 * DESCRIPTION:	suspend all callouts
 */
void suspend_callouts()
{
    if (SYSTEM() && suspend >= 0) {
	rlimits (-1; -1) {
	    if (manager) {
		manager->suspend_callouts();
	    } else {
		mixed *callout;

		if (suspend > 0) {
		    callout = first_suspended;
		    do {
			if (callout[CO_RELHANDLE] != 0) {
			    remove_call_out(callout[CO_RELHANDLE]);
			    callout[CO_RELHANDLE] = 0;
			}
			callout = callout[CO_NEXT];
		    } while (callout);
		} else {
		    suspended = ([ ]);
		}
	    }
	    suspender = previous_object();
	    suspend = -1;
	}
    }
}

/*
 * NAME:	release_callouts()
 * DESCRIPTION:	release suspended callouts
 */
void release_callouts()
{
    if (SYSTEM() && suspend < 0) {
	rlimits (-1; -1) {
	    suspender = nil;
	    suspend = 1;
	    if (manager) {
		manager->release_callouts();
	    } else if (first_suspended) {
		mixed *callout;

		callout = first_suspended;
		do {
		    if (callout[CO_RELHANDLE] == 0) {
			callout[CO_RELHANDLE] = call_out("release", 0);
		    }
		    callout = callout[CO_NEXT];
		} while (callout);
	    } else {
		suspended = nil;
		suspend = 0;
	    }
	}
    }
}


/*
 * NAME:	suspended()
 * DESCRIPTION:	return TRUE if callouts are suspended, otherwise return FALSE
 */
int suspended(object obj)
{
    return (suspend != 0 && obj != suspender);
}

/*
 * NAME:	suspend()
 * DESCRIPTION:	suspend a callout
 */
void suspend(mixed tls, object obj, int handle)
{
    if (previous_program() == AUTO) {
	if (manager) {
	    manager->suspend(obj, handle);
	} else {
	    mixed *callout;

	    callout = ({ obj, handle, 0, last_suspended, nil });
	    if (suspend > 0) {
		callout[CO_RELHANDLE] = call_out("release", 0);
	    }
	    if (last_suspended) {
		last_suspended[CO_NEXT] = callout;
	    } else {
		first_suspended = callout;
	    }
	    last_suspended = callout;
	    if (suspended[obj]) {
		suspended[obj][handle] = callout;
	    } else {
		suspended[obj] = ([ handle : callout ]);
	    }
	}
    }
}

/*
 * NAME:	remove_callout()
 * DESCRIPTION:	remove callout from list of suspended calls
 */
int remove_callout(mixed tls, object obj, int handle)
{
    mapping callouts;
    mixed *callout;

    if (previous_program() == AUTO && obj != this_object() && suspend != 0) {
	if (manager) {
	    return manager->remove_callout(obj, handle);
	} else if ((callouts=suspended[obj]) && (callout=callouts[handle])) {
	    if (callout != first_suspended) {
		callout[CO_PREV][CO_NEXT] = callout[CO_NEXT];
	    } else {
		first_suspended = callout[CO_NEXT];
	    }
	    if (callout != last_suspended) {
		if (callout[CO_RELHANDLE] != 0) {
		    remove_call_out(last_suspended[CO_RELHANDLE]);
		    last_suspended[CO_RELHANDLE] = callout[CO_RELHANDLE];
		}
		callout[CO_NEXT][CO_PREV] = callout[CO_PREV];
	    } else {
		if (callout[CO_RELHANDLE] != 0) {
		    remove_call_out(callout[CO_RELHANDLE]);
		}
		last_suspended = callout[CO_PREV];
	    }
	    callouts[handle] = nil;
	    return TRUE;	/* delayed call */
	}
    }
    return FALSE;
}

/*
 * NAME:	remove_callouts()
 * DESCRIPTION:	remove callouts from an object about to be destructed
 */
void remove_callouts(object obj)
{
    if (previous_program() == AUTO && suspend != 0) {
	if (manager) {
	    manager->remove_callouts(obj);
	} else if (suspended[obj]) {
	    mixed **callouts, *callout;
	    int i;

	    callouts = map_values(suspended[obj]);
	    for (i = sizeof(callouts); --i >= 0; ) {
		callout = callouts[i];
		if (callout != first_suspended) {
		    callout[CO_PREV][CO_NEXT] = callout[CO_NEXT];
		} else {
		    first_suspended = callout[CO_NEXT];
		}
		if (callout != last_suspended) {
		    if (callout[CO_RELHANDLE] != 0) {
			remove_call_out(last_suspended[CO_RELHANDLE]);
			last_suspended[CO_RELHANDLE] = callout[CO_RELHANDLE];
		    }
		    callout[CO_NEXT][CO_PREV] = callout[CO_PREV];
		} else {
		    if (callout[CO_RELHANDLE] != 0) {
			remove_call_out(callout[CO_RELHANDLE]);
		    }
		    last_suspended = callout[CO_PREV];
		}
	    }
	    suspended[obj] = nil;
	}
    }
}

/*
 * NAME:	release_callout()
 * DESCRIPTION:	let the suspension manager release a callout
 */
void release_callout(object obj, int handle)
{
    if (previous_object() == manager) {
	if (obj) {
	    rlimits(-1; -1) {
		obj->_F_release(handle);
	    }
	} else {
	    suspend = 0;
	}
    }
}

/*
 * NAME:	release()
 * DESCRIPTION:	release a callout
 */
static void release()
{
    mixed *callout;
    object obj;
    int handle;

    callout = first_suspended;
    obj = callout[CO_OBJ];
    handle = callout[CO_HANDLE];
    if ((first_suspended=callout[CO_NEXT])) {
	first_suspended[CO_PREV] = nil;
	suspended[obj][handle] = nil;
    } else {
	last_suspended = nil;
	suspended = nil;
	suspend = 0;
    }
    obj->_F_release(handle);
}


/*
 * NAME:	initd()
 * DESCRIPTION:	perform local system initialization
 */
object initd()
{
    if (previous_program() == DRIVER) {
	return compile_object(USR_DIR + "/System/initd");
    }
}

/*
 * NAME:	prepare_reboot()
 * DESCRIPTION:	prepare for a reboot
 */
void prepare_reboot()
{
    if (previous_program() == DRIVER) {
	downtime = time();
    }
}

/*
 * NAME:	reboot()
 * DESCRIPTION:	recover from a reboot
 */
void reboot()
{
    if (previous_program() == DRIVER) {
	object *objects;
	int i;

	objects = OBJREGD->remove_editors();
	if (resources["editors"]) {
	    for (i = sizeof(objects); --i >= 0; ) {
		owners[objects[i]->query_owner()]->rsrc_incr(
		    "editors", objects[i], -1, resources["editors"], TRUE);
	    }
	}

	downtime = time() - downtime;
	objects = map_values(owners);
	for (i = sizeof(objects); --i >= 0; ) {
	    objects[i]->reboot(downtime);
	}
    }
}
