/**
   container.c
   This file is to be inherited by any object that is capable of
   holding other objects. It handles inventory_related business,
   which is defined here rather than in the auto object.
   Do not confuse this container with a bag. Bags need to inherit this,
   but player.c and room.c (which also contain objects) also inherit
   this. There is no support for opening, closing, or locking these
   containers, since many containers do not desire these options.
   You may wish to write an inheritable bag.c if you plan on having
   many bags (or chests or other such) in your MUD.
   Begun by Mobydick, 5-25-94
   Hymael
   @todo may push get/put to different inheritable, error printing
*/

inherit OBJECT;


#define TP this_player()
#define TPN this_player()->query_Name()
#define write(x) TP->message(x)
#define say(x) TP->query_environment()->message((x), ({TP}))

private static object *inventory;

/***PROTOS***/
object present(string id);

static void create(varargs int clone){
    /* Hymael - adding in bag functionality */

    add_command("get",  "get_from");
    add_command("put",  "put_in");
    add_command("look", "look_in");

}

int is_container(){ return 1; }

/************************************
 * Hymael - coded in weight check   *
 * and atomic                       *
 * slide is sans weight calc        *
 ************************************/
atomic void receive_object(object ob, varargs int slide){
    if(!inventory)
	inventory = ({});

	if(ob->prevent_get())
		error("This object cannot be in inventory.");

    inventory += ({ ob });
    if(!slide)
	::receive_object(ob); /* this is the weight check */
}

atomic void release_object(object ob, varargs int slide){
    if(!inventory || !sizeof(({ ob }) & inventory))
	error("Not in container.\n");

    inventory -= ({ ob });
    if(!slide)
	::release_object(ob);
}


mixed look_in(string str){
    string bag;
    int i;

    if(!str || !sscanf(str, "in %s", bag) || !id(bag))
	return 0;

    /* let's look inside */
    catch(i = sizeof(inventory));
    say(TPN+" looks inside "+article(bag)+" "+bag+".\n");
    if(!i){
	write("Nothing inside of that.\n");
	return 1;
    }
    str = "Contents:\n";
    while(i--){
	str += "  "+inventory[i]->query_short()+"\n";
    }
    write(str);
    return 1;
}

/* sneaky but what the fuck */
atomic void _get(object item, object getter){
    if(environment != getter){/* normal movement */
	item->move(getter);
	return;
    }
    getter->receive_object(item, 1);
    release_object(item);
    item->slide(getter);
}

mixed get_from(string str){/* handle form get X from Y */
    string item, bag, type;
    object ob;
    int number;

    if(!str || sscanf(str, "%s from %s", item, bag) != 2 || !id(bag))/* not this bag */
	return 0;

    /***MONEY***/
    if(sscanf(item, "%d %s coin", number, type) == 2 || sscanf(item, "%d %s", number, type) == 2){
	string err;
	ob = present(type);
	if(!ob || !ob->is_coins())
	    return "You don't see that money.\n";

	err = catch(ob->coin_move(TP, number, (environment == TP)));
	if(err){
	    return err;
	}
	write("You get "+number+" "+type+" from "+article(bag)+" "+bag+".\n");
	say(TPN+" gets some money from "+article(bag)+" "+bag+".\n");
	return 1;
    }
    /***END MONEY***/

    if(item == "all"){
	int i;
	catch(i = sizeof(inventory));
	if(!i)
	    return "Nothing in "+article(bag)+" "+bag+".\n";

	while(i--){/* proceed to empty */
	    item = catch(_get(inventory[i], TP));
	}
	write("You remove everything from the "+bag+".\n");
	say(TPN+" gets everything from the "+bag+".\n");
	return 1;
    }
    ob = present(item);
    if(!ob)
	return "No "+item+" in "+article(bag)+" "+bag+".\n";

    /* ob exists in this bag */
    if((str = catch(_get(ob, TP)))){/* no move */
	write(str);
    } else { /* moved */
	str = ob->query_short();
	write("You take the "+str+" from "+article(bag)+" "+bag+".\n");
	say(TPN+" gets a "+str+" from "+article(bag)+" "+bag+".\n");
    }
    return 1;
}

/* sneaky but what the fuck */
atomic void _put(object item, object putter){
    if(environment != putter){/* normal movement */
	item->move(this_object());
	return;
    }
    receive_object(item);
    putter->release_object(item, 1);
    item->slide(this_object());
}



mixed put_in(string str){/* handle form put X in(to) Y */
    string item, bag, type;
    object ob;
    int number;

    if(!str || sscanf(str, "%s in %s", item, bag)!=2 || !id(bag))/* not this bag */
	return "Usage: put <item> in <bag>\n";

    /***MONEY***/
    if(sscanf(item, "%d %s coin", number, type) == 2 || sscanf(item, "%d %s", number, type) == 2){
	string err;

	err = catch(TP->wealth_move(this_object(), type, number, (environment == TP)));
	if(err){
	    return err;
	}
	write("You put "+number+" "+type+" into "+article(bag)+" "+bag+".\n");
	say(TPN+" puts some money into "+article(bag)+" "+bag+".\n");
	return 1;
    }
    /***END MONEY***/

    ob = TP->present(item);
    if(!ob)
	return "You don't possess "+article(item)+" "+item+".\n";

    if(ob == this_object())
	return "You cannot put an object inside itself.\n";

    /* ob exists in this bag */
    if((str = catch(_put(ob, TP)))){/* move fail */
	write(str);
    } else { /* moved */
	str = ob->query_short();
	write("You put "+str+" into "+article(bag)+ " "+bag+".\n");
	say(TPN+" puts a "+str+" into "+article(bag)+" "+bag+".\n");
    }
    return 1;
}

/* query_inventory() returns the inventory of this object. Right now
   I'm passing the array which is the Wrong Thing. I need to alter
   this to return a copy. */

object *query_inventory() {
    if(!inventory)
	inventory = ({});

    inventory -= ({ nil });
    return inventory[..];
}

/* The present function takes a string and returns the first object in
   the inventory of the object which takes that string as id.
   If the string passed is "me", then the body of this_user() will be
   returned if in the container. If the string is "here", then the
   container will be returned. There are no other magic words.
   Does not, as yet, grok the concept of "sword 2".
   It is more object-oriented to have this in container, where it can
   be overriden by a given container if that is desired, than to have
   it as a kfun which was the old LPC way.
*/

object present(string id){
    int sz, num;
    string hardid;

    if(!inventory || (sz = sizeof(inventory)) == 0)
	return nil;

    if(id == "me"){
	if(member_array(TP, inventory) > -1){
	    return TP;
	}
	return nil;
    }

    if(id && sscanf(id, "%s %d", hardid, num) == 2){/* we have sword 2 form */
	if(num > 0){
	    while(sz--){
		if(inventory[sz]->id(hardid) && --num == 0)
		    return inventory[sz];
	    }
	}
	return nil;
    }
    /* I'm not sure this is really correct, but it makes "look at here" work. */
    if (id == "here")
	return this_object();

    while(sz--){
	if(inventory[sz]->id(id))
	    return inventory[sz];
    }
    return nil;
}

int object_present (object ob) {
    if (!inventory || !sizeof(inventory))
	return 0;

    return (member_array(ob,inventory) != -1);
}

atomic void container_clear(){
    int i, sz;
    object env;
    if(inventory && sizeof(inventory))
    {
        env = this_object()->query_environment();
        for(i=0,sz = sizeof(inventory); i<sz; i++) /* move objects to environment */
        {
            inventory[i]->move(env);
        }
        inventory = nil;
    }
}

