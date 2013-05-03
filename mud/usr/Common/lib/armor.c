/*************************
 *       armor.c         *
 *************************/

inherit OBJECT;

/* messages */
#define MY_BREAK aname + "'s blow has broken your " + thisname + "!\n"
#define  R_BREAK aname + "'s blow " + "breaks " + vname + "'s " + thisname + "!\n"
#define BR_BREAK "Your blow breaks " + vname + "'s " + thisname + "!\n"

static string type;              /**< what's type name of this armor? */
static string slot;              /**< what slot does this armor fit */
static int ac;                   /**< armor's inherent armor class */
static string equip_func;        /**< function called when armor is equipped */
static string unequip_func;      /**< function called when armor is unequipped */
static string hit_func;          /**< possibly called when player is hit */
static int quality;              /**< the quality of a weapon scaled 1-100 */
static int _broke;               /**< whether it is broke or not */

void create(varargs int clone){
    set_short("armor");
    ::create(clone);
}
/****************
 * sets/queries *
 ****************/
int is_armor() { return 1; }

int is_broke() { return _broke; } /* broken armors can't be used */

void set_type(string str){ type = str; }/* corresponds to a body part */

string query_type(){ return type; }

void set_slot(string str){ slot = str; }

string query_slot(){ return slot; }

void set_quality(int val){ quality = val; }

/* override short for [broken] tag */
string query_short(){
    return ::query_short() + ((_broke) ? " %^B_RED%^[broken]%^RESET%^" : "");
}

int query_quality()      { return quality; }

void set_ac(int val){ ac = val; } /* can be negative for cursed armors */

int query_ac(){ return ac; }

void set_equip_func(string fun)   { equip_func = fun;    }
void set_unequip_func(string fun) { unequip_func = fun;  }

string query_equip_func()         { return equip_func;   }
string query_unequip_func()       { return unequip_func; }

void set_hit_func(string fun) { hit_func = fun; }

string query_hit_func() { return hit_func; }

void set_id (string *newid) {/* assures we always have id armor */
    ::set_id (newid +({ "armor" }));
}

void set_break(){
    _broke = 1;
}

/* this is called on each blow to area to determine if armor breaks */
int handle_break(object breaker, object weapon, object breakee, varargs int damage){
    int chance;/* may put values like these in a header */
    chance = 75 + quality - (weapon->query_quality() + damage);/* comparative quality */
    if(random(100) > chance){
	string vname, aname, thisname;
	vname    = breakee->query_Name();
	aname    = breaker->query_Name();
	thisname = this_object()->query_short();
	this_object()->set_break();
	breakee->unequip_armor(this_object());
	breakee->message(MY_BREAK);
	breaker->message(BR_BREAK);
	breakee->query_environment()->message(R_BREAK, ({ breaker, breakee }) );
	return 1;
    }
    return 0;

}

int fix_break(){/* this corrects problems with armor */
    if(_broke){
	_broke = 0;
	return 1;
    }
    return 0;/* not broke */
}
