/**
 * the weight system                 
 * this handles all the logic with   
 * weights and bulks and capacities  
 * @author Hymael                           
 */
#include <game/weight.h>

int capacity;  /**< how much an object can hold in weight */
int   volume;  /**< how much an object can hold in bulk   */
int    p_cap;  /**< present weight held                   */
int p_volume;  /**< present bulk held                     */
int     bulk;  /**< the relative size of an object        */
int   weight;  /**< an object's weight                    */

void create(varargs int clone){/* demand for defaults */
    capacity = DEF_CAPACITY;
    volume   = DEF_BULK;
}

int has_capacity(){ return capacity; }

/* sets & queries */
void set_capacity(int val){ capacity = val; }
int query_capacity()      { return capacity; }

void set_volume(int val){ volume = val; }
int query_volume()      { return volume; }

atomic void set_bulk(int val){ 
    int change;
    object env;
    change = val - bulk;
    bulk = val; 
    if(change == 0){
	return;
    }
    env = this_object()->query_environment();
    if(!env)return;

    if(change > 0){
	env->incr_bulk(change);
    }else{
	env->decr_bulk((int)fabs((float)change));
    }   
}
int query_bulk()      { return bulk; }

atomic void set_weight(int val){ 
    int change;
    object env;
    change = val - weight;
    weight = val; 
    if(change == 0){
	return;
    }
    env = this_object()->query_environment();
    if(!env)return;

    if(change > 0){
	env->incr_weight(change);
    }else{
	env->decr_weight((int)fabs((float)change));
    }   
}

int query_weight()      { return weight; }

int query_p_cap()   { return p_cap; }

int query_p_volume(){ return p_volume; }

/* need an incr and decr weight for coins */
atomic void incr_weight(int val){
    p_cap += val;
    if(capacity && p_cap > capacity)
	error("Too heavy.\n");
}

atomic void incr_bulk(int val){
    p_volume += val;
    if(volume && p_volume > volume)
	error("Too bulky.\n");
}

atomic void decr_weight(int val){
    p_cap -= val;
    if(p_cap < 0)
	error("Weight error.\n");
}

atomic void decr_bulk(int val){
    p_volume -= val;
    if(p_volume < 0)
	error("Bulk error.\n");
}

/*************************
 * insert into container *
 *************************/
atomic void receive_object(object ob){/* determine if ob can be inserted */
    int i;
    if(this_object()->is_room())
	return;

    if(!this_object()->is_container())
	error("Not a container.\n");

    catch(i = ob->query_p_cap());
    incr_weight(ob->query_weight() + i);

    catch(i = ob->query_p_volume());
    incr_bulk(ob->query_bulk() + i);
}

/*************************
 * remove from container *
 *************************/
atomic void release_object(object ob){
    int i;
    if(this_object()->is_room())
	return;

    if(!this_object()->is_container())
	error("Not a container.\n");

    if(this_object()->object_present(ob))
	error("Not inside.\n");

    catch(i = ob->query_p_cap());
    decr_weight(ob->query_weight() + i);

    catch(i = ob->query_p_volume());
    decr_bulk(ob->query_bulk() + i);
}

