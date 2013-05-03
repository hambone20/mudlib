/*****************************
 * This is object is a stack *
 * of coins. It can be moved *
 * around and processed into *
 * abstraction by players.   *
 *****************************/
inherit OBJECT;

#include <game/money.h>
#include <type.h>

private int    _num_coins;  /* number of coins this stack represents */
private string _cointype;   /* type of coins this stack represents */
private string creator;

int is_coins(){ return 1; }

void create(varargs int clone){/* defaults */
	_num_coins = 1;
	_cointype = COIN_NAMES[0];
	set_id( ({ "coins", "coin", _cointype, _cointype+" coin", _cointype+" coins" }) );
	set_weight(_num_coins);
    creator = previous_program();
    #ifdef TRACKCOINS
    find_object(WORLDBANKD)->create_coin(this_object(), creator); /* add to registry */
    #endif
}

void set_cointype(string val){
	if(member_array(val, COIN_NAMES) == -1)
		error("Incorrect coin type.\n");

	_cointype = val;
	set_id( ({ "coins", "coin", _cointype, _cointype+" coin", _cointype+" coins" }) );
}

string query_cointype(){ return _cointype; }

void set_num_coins(int val){
	_num_coins = ( (val > 0) ? val : 1 );
	set_weight(_num_coins);
}

int query_num_coins(){ return _num_coins; }

string query_short(){/* display coins in proper manner */
	switch(_num_coins){/* pile it up */
	case 1:
		return "one "+_cointype+" coin";
	case 2..3:
		return "few "+_cointype+" coins";
	case 4..8:
		return "handful of "+_cointype+" coins";
	default:
		return "many "+_cointype+" coins";
	}
	return nil;
}

string query_long(){
	string str;

	str = wrap(_num_coins+" "+_cointype+" coins.\n", 72);
	return str;
}

/* sans checks */
atomic void slide(mixed dest){
	if(!dest)
		error("No destination.\n");

	if(typeof(dest) == T_STRING){
		dest = find_object(dest);
	}
	if(!dest)
		error("No destination.\n");

	environment = dest;
}

/* coalesce the coins about */
atomic void move(mixed dest, varargs int slide){
	if(!dest)
		error("No destination.\n");

	if(typeof(dest) == T_STRING){
		/* Load the dest if needed, then convert it to an object. */
		dest = find_object(dest);
	}
	if(!dest)
		error("No destination.\n");

	if(dest->has_purse()){
		dest->credit(_cointype, _num_coins, slide);
		/*if(environment->has_purse*/
		call_out("destruct", 0);
		return;
	}
	/* Is our current environment willing to let us go? */
	if(environment)
		environment->release_object(this_object(), slide);

	{/***VC***/
	object cur_coins;
	/* check for stacking */
	if((cur_coins = dest->present(_cointype+" coin")) && cur_coins->is_coins()){
		int i;
		i = cur_coins->query_num_coins();
		cur_coins->set_num_coins(i + _num_coins);
		if(!slide)
			dest->incr_weight(_num_coins);

		call_out("destruct", 0);
		return;
	}
	}/***END VC***/
	/* Is the new environment willing to take us?  */
	dest->receive_object(this_object(), slide);

	environment = dest; /* done */
}

/***COIN MOVE***/
atomic void coin_move(object dest, int number, varargs int slide){
	if(number < 1)
		error("Take how many coins?\n");

	if(number > _num_coins)
		error("There are not that many coins present.\n");

	if(_num_coins == number){/* all coins */
		move(dest, slide);
		return;
	}

	{/***SPLIT***/
	object new_stack;
	new_stack = clone_object(COIN);
	new_stack->set_cointype(_cointype);
	new_stack->set_num_coins(number);
	new_stack->move(dest, slide);
	}/***END SPLIT***/

	set_num_coins(_num_coins - number);
	environment->decr_weight(number);
}

