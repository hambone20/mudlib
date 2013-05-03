/*
 *  messaged : does heavy lifting for all player messages
       'a'    Apply a() to the following element.
       'c'    Apply capitalize() to the following element.
	   'f'    Call the function defined by the given string
       'j'    Join together the following two elements without inserting
	      a space between them.
       'n'    Ignore this element, as if it were not present.  (Useful
	      in condition construction of messages.)
       'p'    Apply pluralize() to the following element.
       's'    Apply singularize() to the following element.
       'w'    Apply capitalize_words() to the following element.

 */
#include <type.h>

string *vowels;
private mapping outer_calls; /* this holds the calls that occur outside of inner arrays */
private mapping inner_calls; /* this holds the calls that occur inside nested array */
private int     *double_calls; /* functions like j, requiring the next two elements */
private int     *orig_calls; /* functions that like to be called with originator argument */

void create(){/* load up deferring array, pluralization structures, etc */
    vowels = VOWELS;
    outer_calls = ([ 
      0  : "comp_orig", /* compare to originator, return appropriate */
      'a': "a",
      'c': "capitalize",
      'f': "func",
      'j': "join",
      'n': "ignore",
      'p': "pluralize",
      's': "singularize",
      'w': "capitalize_words" /* throw in a function call with $? */
    ]);
    
    double_calls = ({ 'j', 'd' });
    orig_calls = ({ 0 });
    
    inner_calls = ([ 
          0: "comp_orig",
        'a': "a",
        'c': "get_race",
        'd': "print_dir",
        'e': "one",
        'i': "print_list",
        'l': "reflexive_pronoun",
        'm': "possessive_objective",
        'n': "literal_name",
        'o': "objective_pronoun",
        'p': "subjective_pronoun",
        'q': "possessive_pronoun",
        'r': "indef_possessive",
        's': "def_possessive2",
        't': "the",
        'u': "def_possessive1",
        'w': "which",
        'x': "sex",
        'z': "forme"
    ]);
}

/*string apply(string function, varargs   // apply given function */

/* useful string calls */

string get_race(mixed a, object originator, object viewer){ 
    if(function_object("query_race", a) == nil) return "object";
    return a->query_race();
}

string print_dir(mixed obj, string dir, object originator, object viewer)
{
    string ret;
    
    ret = "";
    
}

string ignore(mixed a, object originator, object viewer){ return ""; }

string func(mixed obj, mixed f, object originator, object viewer){ return call_other(obj, f); }/* 'f' */

string get_name(mixed obj){
    if(typeof(obj) == T_STRING){
	return (string)obj;
    }
    
    if(typeof(obj) == T_OBJECT){/* should know whether to properize the noun or not */
	if(obj->query_Name() == obj->query_short())return (string)obj->query_Name();
	
	return (string)obj->query_short();
    }
    error("Error in get_name: wrong type for obj.\n");
}

string join(mixed a, mixed b, object originator, object viewer){ /* check for strings */

    return a + b; 
}

string a(mixed obj, object originator, object viewer){
    string str;
    
    switch(typeof(obj)){
    case T_STRING:
    str = (string)obj;
    break;
    
    case T_OBJECT:
    str = (string)get_name(obj);
    break;
    
    default:
    error("Wrong type on function 'a'\n");
    }
    
    if(capitalize(str) == str)return str;
    
    return article(str)+" "+str;
}


string comp_orig(mixed a, varargs mixed viewer)
{
    string str;
    switch(typeof(a)){
    case T_STRING:
    str = (string)a;
    break;
    
    case T_OBJECT:
    if(viewer && (object)a == (object)viewer)str = "you";
    else str = get_name(a);
    break;
    
    default:
    error("Wrong type on function 'a'\n");
    }
    return str;
}

string pluralize(mixed obj, object originator, object viewer){/* accept object or string to pluralize */
    string str;
    
    str = get_name(obj);
    return str + "s";
}

string list_array(mixed *list, varargs string and){/* list out the given with and separating */
    return "";
    
}

string inner_parse(mixed *list, object originator, object viewer)
{
    int i, sz;
    string ret, verb;
    
    ret = "";
    for(i=0; i < sz; i++)/* run through tokens */
    {
    if(i != 0) /* do spacing */
    {
        ret += " ";
    }
    switch(typeof(list[i]))
    {
    case T_INT:
        /* must be a control code */
        if(inner_calls[list[i]] == nil)error("No control match");
        
        if(sizeof(orig_calls & ({ list[i] })) > 0)
            ret += call_other(this_object(), inner_calls[list[i]], originator, viewer);
        else if(sizeof(double_calls & ({ list[i] })) > 0)
            ret += call_other(this_object(), inner_calls[list[i]], list[++i], list[++i], originator, viewer);
        else
            ret += call_other(this_object(), inner_calls[list[i]], list[++i], originator, viewer);
        continue;
    case T_STRING:
        if(i == 0) /* we have a verb */
        {
            verb = (string)list[i]; /* do immediate call? */
        }
        else
        {
            ret += (string)list[i];
        }
        continue;
    case T_OBJECT:/* check for verb */
        ret += a(list[i], originator, viewer);
        continue;
    case T_ARRAY:/* check for verb */
        ret += inner_parse(list[i], originator, viewer);
        continue;
        }      
    }
    return capitalize(ret)+".\n";
}

string parse_message(mixed *list, object originator, object viewer)
{
    int i, sz;
    string ret;
    
    ret = "";
    if( (sz=sizeof(list)) == 0)return ret; /* no message */
    
    for(i=0; i<sz; i++)/* run through tokens */
    {
    if(i != 0) /* do spacing */
    {
        ret += " ";
    }
    switch(typeof(list[i]))
    {
    case T_INT:
        /*if(list[i] == 0)
        {
            ret += (string)a(originator);
            continue;
        }*/
        /* must be a control code */
        if(outer_calls[list[i]] == nil)error("No control match");
        
        if(sizeof(orig_calls & ({ list[i] })) > 0)
            ret += call_other(this_object(), outer_calls[list[i]], originator, viewer);
        else if(sizeof(double_calls & ({ list[i] })) > 0)
            ret += call_other(this_object(), outer_calls[list[i]], list[++i], list[++i], originator, viewer);
        else
            ret += call_other(this_object(), outer_calls[list[i]], list[++i], originator, viewer);
        continue;
    case T_STRING:
        ret += (string)list[i];
        continue;
    case T_OBJECT:
        ret += a(list[i], originator, viewer);
        continue;
    case T_ARRAY:
        ret += inner_parse(list[i], originator, viewer);
        continue;
        }      
    }
    return capitalize(ret)+".\n";
}

/*inner_parse*/

string emit(object actor, mixed *emitme){/* does all the heavy lifting of messaging */
    string control, me, we, they;/* gets changed as we step through based on control characters */
    int i, sz;
    
    me = they = we = "";/* /* will be our running concatenation */
    return "";
}

void message(varargs mixed *list, object *exclude, object *define, string filter)/* emit message, filter is a function */
{ /** @todo: add in check for vision and invis things */
    object originator, *audience;
    int i, sz;
    
    if(list == nil || (sz = sizeof(list)) == 0)return; /* no message */
    
    originator = this_player();
    audience = originator->query_environment()->query_inventory(); /** todo: make a function that gets living objects in container */
    
    if(exclude)audience -= ({ exclude });
    
    if(define)audience += ({ define });
    
    for(i=0, sz=sizeof(audience); i < sz; i++)
    {
        if(audience[i]->is_body())
        {
            if(filter && function_object(filter, originator) && call_other(originator, filter, audience[i]) == 0)continue; /* abort */
            
            /* should be good to transmit */
            audience[i]->transmit(list, originator);
        }
    }
    
}


