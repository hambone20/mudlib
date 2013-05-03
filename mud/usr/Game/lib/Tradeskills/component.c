/**
 * This defines a typical tradeskill 
 * component.                        
 */
#include <game/Tradeskills/weaponsmithing.h>

inherit OBJECT;

#define RESET "%^RESET%^"

string _tradeskill;  /**< the tradeskill this component is used with */
int    _bonus;       /**< the relative benefit to using this component */
int    _difficulty;  /**< difficulty in using this component */
int    _reuse;       /**< set how many times this item can be reused */
string _subset;      /**< subset */
string _alloy;       /**< alloy if applicable */
string _color;       /**< color this item presents */

void create(varargs int clone){}

int is_component(){ return 1; }

void set_tradeskill(string val){ _tradeskill = val; }
string query_tradeskill()      { return _tradeskill; }

void set_color(string val){ _color = val; }
string query_color()      { return ( (_color) ? _color : "" ); }

void set_alloy(string val){ _alloy = val; }
string query_alloy()      { return _alloy; }

void set_bonus(int val){ _bonus = val; }
int query_bonus()      { return _bonus; }

/** override */
string query_short(){
    return query_color() + ::query_short() + RESET;
}

void set_difficulty(int val){ _difficulty = val; }
int query_difficulty()      { return _difficulty; }

void set_reuse(int val){ _reuse = val; }
int query_reuse()      { return _reuse; }

void set_subset(string val){ _subset = val; }
string query_subset()      { return _subset; }

/** this determines if the component is used up */
void use_component(){
    if(--_reuse < 0){/* done */
	destruct_object(this_object());
    }
}
