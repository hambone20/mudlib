/* A limb */

#define DECAY_TIME 20

inherit CONTAINER;
inherit WEAPON;

int state;
string name;
int handle;

void create(varargs int clone) {
    set_short ("a limb") ;
    set_long ("The limb of a fallen.\n");
    set_id ( ({ "limb", "a limb" }) ) ;
    handle = call_out("decay", DECAY_TIME);
    state = 0; /* 0 fresh, 1 decayed, 2 rank, gone */
}

int is_limb(){ return 1; }

void set_limb(string vname, string part){
    name = capitalize(vname);
    set_short(name+"'s "+part);
    set_long(name+"'s "+part+", they'd appreciate if you returned it.\n");
    id += ({ lowercase(name)+"'s "+part, part });
}

void decay(){
    object *stuff;
    int sz;
    switch(++state){
    case 1:
	set_short(name+"'s decaying limb");
	break;
    case 2:
	set_short(name+"'s rotting limb");
	break;
    default:
	this_object()->query_environment()->message("The limb rots away.\n");
	stuff = this_object()->query_inventory() ;
	if (stuff && (sz=sizeof(stuff))>0) {
	    while(--sz>=0){
		stuff[sz]->move(this_object()->query_environment());
	    }
	}
	destruct_object(this_object());
	return;
    }
    handle = call_out("decay", DECAY_TIME);
}

void stop_decay(){
    remove_call_out(handle);
}

