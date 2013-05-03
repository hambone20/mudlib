/**
 *                     wealth.c                           
 *  This is for those people that delight in moneys       
 *  @todo need bulk? error printing with $                                        
 *  @author Hymael                                               
 */
#include <game/money.h>

mapping wealth;    /**< le purse */

int has_purse(){ return 1; } /* we know this object wants to hold cash in a purse */

string query_wealth(){/**< get a printable inventory of wealth */
    string str;
    int i, sz;

    str = "Wealth:\n\n";
    if(!wealth) wealth = ([]);

    for(i=0,sz = sizeof(COIN_NAMES); i<sz; i++){
	str += pad(capitalize(COIN_NAMES[i])+":", 15)+((wealth[COIN_NAMES[i]])?wealth[COIN_NAMES[i]]:0)+"\n";
    }

    return str;
}

/* with atomic we just do in a catch and print any messages */
/** @todo may make these accept arrays of coins */
atomic void credit(string coin_type, int amount, varargs int slide){
    if(!wealth) wealth = ([]);

    if(member_array(coin_type, COIN_NAMES) == -1)
	error("That is not money.\n");

    if(amount < 1)
	error("Positive money please.\n");

    wealth[coin_type] = ((wealth[coin_type] != nil) ? wealth[coin_type] : 0) + amount;

    /* increment weight */
    if(!slide)
	this_object()->incr_weight(amount);
}

atomic void debit(string coin_type, int amount, varargs int slide){
    if(!wealth) wealth = ([]);

    if(member_array(coin_type, COIN_NAMES) == -1)
	error("That is not money.\n");

    if(amount < 1)
	error("Positive money please.\n");

    if(wealth[coin_type] == nil)
	error("Not enough money.\n");

    wealth[coin_type] -= amount;

    if(wealth[coin_type] < 0)
	error("Not enough money.\n");

    /* decrement weight */
    if(!slide)
	this_object()->decr_weight(amount);
}

/**
 * this moves from a purse 
 * to destination          
 * NOTE: with no amount we
 * assume all              
 */
atomic void wealth_move(object dest, string type, varargs int amount, int slide){
    if(!amount){
	if(!wealth || !wealth[type])
	    error("You haven't any "+type+" coins.\n");

	amount = wealth[type];
    }
    if(amount < 1)
	error("Illegal to deal in negative moneys.\n");

    debit(type, amount, slide);
    {/***VC***/
	object new_stack;
	if(!find_object(COIN)) compile_object(COIN);
	new_stack = clone_object(COIN);
	new_stack->set_cointype(type);
	new_stack->set_num_coins(amount);
	new_stack->move(dest, slide);
	if(slide)
	    dest->incr_weight(amount);
    }/***END VC***/
}
