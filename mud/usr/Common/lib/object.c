/*
   object.c
   An inheritable which defines the functions needed by all objects
   which play a part in the MUD environment. It handles descriptions,
   environment handling, moving, and so forth.
   Any object which might be handled by a player or monster, or appears
   in a room on the MUD, or will be moved from one place to another, or
   defines a player command, or any such thing, should inherit this.
*/

#include <type.h>

inherit WEIGHT;

static string short_desc;          /**< description of the object in inventories */
static string long_desc;           /**< description of the object when looked at */
static object environment;         /**< object that contains this object */
static string *id;                 /**< an array of string that name this object */
static mapping commands;           /**< command words defined by this object */
private static mapping temp_var;   /**< this will contain temporary variables */
mapping vars;            	       /**< very against this but somethings it is necessary to
									  have a dynamic set of variables that hang around */


int is_object(){
    return 1; 
}

mapping query_commands()/* mainly for wizzes */
{
    return commands;
}

/***PROPS***/
void set_temp(string var, mixed val){
    if(!temp_var)
	temp_var = ([]);

    temp_var[var] = val;
}

mixed query_temp(string var){
    if(!temp_var)
	return nil;

    return temp_var[var];
}

void delete_temp(string var){
    if(!temp_var)
	return;

    temp_var[var] = nil;
}

void set(string var, mixed val){
    if(!vars)
	vars = ([]);

    vars[var] = val;
}

mixed query(string var){
    if(!vars)
	return nil;

    return vars[var];
}

void delete(string var){
    if(!vars)
	return;

    vars[var] = nil;
}
/***END PROPS***/

void set_short(string str){
    short_desc = str;
}

string query_short(){
    return short_desc;
}

void set_long(string str){
    long_desc = str;
}

string query_long(){
    return long_desc;
}

/* query_environment will return the object's environment. */
object query_environment(){
    return environment;
}

void set_id(string *newid){
    id = newid;
}

string *query_id(){
    return id;
}

int id(string str){
    if(!id || !sizeof(id))
	return 0;

    return (sizeof(id & ({ str })) );
}

/* atomic move */
atomic void move(mixed dest){
    if(!dest)
	error("No destination.\n");

    if(typeof(dest) == T_STRING){
	/* Load the dest if needed, then convert it to an object. */
	if(!(dest = find_object(dest)))
	    dest = compile_object(dest);
    }
    if(!dest)
	error("No destination.\n");

    /* Is our current environment willing to let us go? */
    if(environment){
	environment->release_object(this_object());
    }

    /* Is the new environment willing to take us?  */
    dest->receive_object(this_object());

    environment = dest; /* done */
}

/* sans checks */
atomic void slide(mixed dest){
    if(!dest)
	error("No destination.\n");

    if(typeof(dest) == T_STRING){
	if(!(dest = find_object(dest)))
	    dest = compile_object(dest);
    }
    if(!dest)
	error("No destination.\n");

    environment = dest;
}
/* not sure on implementation yet */
void add_command(string word, string function){
    if(!commands)
	commands = ([ ]);

    commands[word] = function;
}

PER_ACTION{
    if(!commands || !commands[verb])
	return nil;

    if(arg)
	return call_other(this_object(), commands[verb], arg);

    return call_other(this_object(), commands[verb]);
}

/* destruct: remove from inventories */
atomic void destruct(){

    if(this_object()->is_container() == TRUE) /* may have to take care of inventory, call up the chain */
    {
        this_object()->container_clear(); /* calling container destruct */
    }
    if(environment)
	environment->release_object(this_object());
}

