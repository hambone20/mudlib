/* A corpse */

#include <config.h>
#define DECAY_TIME 20

inherit CONTAINER ;
/* create into room, move inventory to it, kill person, start decay */
private int state;
private string name;

void create(varargs int clone){
    ::create(1);
    set_short ("a corpse") ;
    set_long ("The corpse of a fallen.\n");
    set_weight(100);
    /*add_command("eat", "eat");*/
    set_id ( ({ "corpse", "a coprse" }) ) ;
    call_out("decay", DECAY_TIME);
    state = 0; /* 0 fresh, 1 decayed, 2 rank, gone */
}

void set_name(string str){
    name = capitalize(str);
    set_short(name+"'s corpse");
    id += ({ lowercase(str)+"'s corpse" });
}

void decay(){
    object *stuff;
    int sz;
    switch(++state){
    case 1: set_short(((name) ? name : "Someone")+"'s decaying corpse");
	break;
    case 2: set_short(((name) ? name : "Someone")+"'s rotting corpse");
	break;
    default:this_object()->query_environment()->message("The corpse rots away.\n");
	stuff = this_object()->query_inventory() ;
	if (stuff && (sz=sizeof(stuff))>0) {
	    while(--sz>=0){
		stuff[sz]->move(this_object()->query_environment());
	    }
	}
	destruct_object(this_object());
	return;
    }
    call_out("decay", DECAY_TIME);
}

/*int eat(string str){
if(str && str=="corpse"){
    write("You eat the corpse!\n");
    say(TPN+" eats the corpse!\n");
    call_out("destruct",0);
}
return 1;
}*/
