/*
   room.c
   Any object which is meant to contain players should inherit room.c.
   It defines exits for rooms which players can move through. It also
   has an altered short and long that shows those exits, and the long
   shows the contents of the room as well. There is also a room_tell()
   lfun which you can call to send a message to all players in that
   room.
   Begun by Mobydick, 5-25-94.
*/

inherit CONTAINER;

/** bits */
#define OUTSIDE   1 /**< denotes outside */
#define HAS_COVER 2 /**< denotes whether there is cover around */
/* must add light and such */

        mapping exits;   /**< rooms exits */
private int _range;      /**< range encounters are in this room */
private int _bits;       /**< bit field */
private string entrance; /* an entrance message to be echoed after someone enters */

/*int catch_tell(string str);*/

/** bland defaults */
void create(varargs int clone){
	set_short("Nondescript Room");
	set_long("Bland, tasteless, lifeless, etc.");
	ROOMD->add_room(this_object());
}

void set_entrance(string str){ entrance = str; }

int is_room(){ return 1; }

void set_outside(){ _bits |= OUTSIDE; }

int query_outside(){ return _bits & OUTSIDE; }

void set_cover(int val){
	if(val) _bits |= HAS_COVER;
	else _bits &= ~HAS_COVER;
}

int query_cover(){ return _bits & HAS_COVER; }

void set_range(int val){ _range = val; }

int query_range()      { return _range; }

void set_exits(mapping ex){
    exits = ex;
}

void add_exit(string direction, mixed file){
	if(!exits)
		exits = ([]);

    exits[direction] = file; /*+= ([ direction : file ]);*/
}

void remove_exit(string direction){
	exits[direction] = nil;
}

mapping query_exits(){
    return exits[..];
}

object query_exit(string direction){
    return (!exits || !exits[direction]) ? nil : exits[direction];
}

/** You may not pick up your surroundings. Basic theorem in topology. */
int prevent_get(){
    return 1;
}

string query_long(varargs int brief, object actor){/* optional actor */
    string value;
    object *inventory;
    int sz;

	if(!actor)actor = previous_object();
	/* Start with whatever desc the room coder supplied. Give the short
       or the long desc, according to the brief argument. */
    if(brief)
    	value = short_desc+"\n";
    else
    	value = short_desc+"\n\n"+long_desc;

	/* Add in any exits there may be. */
	value += "%^BOLD%^\n\t";
    if(!exits || !(sz = map_sizeof(exits))){
        value += "There are no obvious exits.\n" ;
    }else{
		string *indies;
		indies = map_indices(exits);

        switch(sz){
		case 1:
	    	value += "The only obvious exit is "+indies[0];
	    	break;
	    case 2:
	    	value += "Obvious exits are "+indies[0]+" and "+indies[1];
	    	break;
	    default:
			value += "Obvious exits are "+implode(indies[0..sz-2], ", ")+", and "+indies[sz-1];
		}
	    value += ".\n";
    }
    value += "\n%^RESET%^";
	/* List the contents of the room. */
    inventory = query_inventory();
    if(inventory && (sz = sizeof(inventory))){
		string name;
		int i;

        for(i=0; i<sz; i++){
       		/* Make sure there isn't an empty item in the inventory somehow. */
	    	if(!inventory[i])
	    		continue;

       		/* Don't include our own body in the list. */
	    	if(inventory[i] == actor)
	    		continue;

	    	name = inventory[i]->query_short();
	    	if(name){
	    		value += "   "+name+"\n";
        	}
        }
    }
    return value;
}

/** room_tell. analagous to catch_tell, catches a message and passes
   it on to every living object in the room except excluded ones. */
void message(varargs string str, object *excluded){
    int i, sz;
    object *inventory;

    /*if(!catch_tell(str))
    	return;*/

    inventory = query_inventory();
    if(!inventory || !(sz=sizeof(inventory)))
    	return;

    for(i=0; i<sz; i++){
        if(!inventory[i]->is_body())/*query_living())*/
        	continue;

        if(excluded && member_array(inventory[i], excluded) > -1)
        	continue;

        inventory[i]->message(str);
    }
}

void echo_entrance(object player){
	if(player->query_environment() != this_object())return;
	if(entrance)player->message(entrance);
}

atomic void receive_object(object ob, varargs int slide){
	if(ob->is_body() && entrance){
		call_out("echo_entrance", 3, ob); 
	}
	::receive_object(ob, slide);
}



