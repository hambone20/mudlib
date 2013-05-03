/******************
 *    weapon.c    *
 ******************/

#define TYPES ({ "cut", "pierce", "blunt" })
/***********************************
 * eventually we need weapon types *
 * for what damage they deal       *
 ***********************************/

#define OFF_HAND 1 /**< bit for off hand */
#define TWO_HAND 2 /**< bit for two hand */
/* attributes NOTE: add ins for ranged and reach */

inherit OBJECT;

static string type;         			/**< of TYPES */
static int bit_att;                     /**< a bitfield of attributes */
static int *damage_matrix; 			    /**< handles damage output */
static int accuracy;        			/**< inherent accuracy, barring player attributes */
static string hit_func;     			/**< function called each hit to handle special */
static string intercept_func;           /**< function that intercepts blows */
static string wield_func;   			/**< function called when weapon is wielded */
static string unwield_func; 			/**< function called when weapon is unwielded */
static string *verbs_me, *verbs_other;  /**< "You verbs_me[num] X with your query_short()"*/
static int quality;                     /**< the quality of a weapon scaled 1-100 */
static int _broke;                      /**< whether it is broke or not */
static int _speed;                      /**< seconds for a swing */

static string weapon_name;              /**< work around */

int is_weapon(){
    /*if(this_object()->is_body()){
	    return 0;
    }*/
    return 1;
}

int is_broke(){ return _broke; }

void set_quality(int val){ quality = val; }

int query_quality()      { return quality; }

void set_speed(int val)  { _speed = val; }

int query_speed(){
    return _speed;
}

void set_weapon_name(string val){ weapon_name = val; }

string query_weapon_name(){
    if(weapon_name) return weapon_name;
    if(this_object()->is_body()) return "fists";
    return this_object()->query_short();
}

/* override short for [broken] tag */
string query_short(){
    return ::query_short() + ((_broke) ? " %^B_RED%^[broken]%^RESET%^" : "");
}

void set_two_handed(){ bit_att |= TWO_HAND; }

int query_two_handed(){ return bit_att & TWO_HAND; }

void set_type(string str){ type = str; }

string query_type(){
    if(!type) return "blunt";
    return type;
}

void set_offhand(){ bit_att |= OFF_HAND; }

int query_is_offhand(){ return bit_att & OFF_HAND; }

void set_accuracy(int val){ accuracy = val; }

int query_accuracy(){ return accuracy; }

void set_hit_func(string str){ hit_func = str; }

string query_hit_func(){ return hit_func; }/* must accept (object attacker, object attackee) */

void set_wield_func(string str){ wield_func = str; }

string query_wield_func(){ return wield_func; }

void set_unwield_func(string str){ unwield_func = str; }

string query_unwield_func(){ return unwield_func; }

void set_intercept_func(string str){ intercept_func = str; }

string query_intercept_func()      { return intercept_func; }

/***********************************************
 *          NOTE ON UN/WIELD FUNCTIONS         *
 * if the function returns 1, the message is   *
 * dealt with, if not, message still must be   *
 * done                                        *
 ***********************************************/

void set_verbs_me(string *str)   { verbs_me = str;    }
void set_verbs_other(string *str){ verbs_other = str; }

string *query_verbage(){/* returns 2 dimensional string with a random verb(me and other) */
    int i;
    if(verbs_me && (i = sizeof(verbs_me))){
	i = random(i);
	if(verbs_other && verbs_other[i]){
	    return ({ verbs_me[i], verbs_other[i] });
	}else{
	    return ({ verbs_me[i], verbs_me[i]+"s" });
	}
    }
    /* return defaults */
    return ({ "hit", "hits" });
    /*************************************************************************************************************************
     *  								FORM																				 *
     *	hitter - "You "+verbage[0]+" "+target_name+" with your "+weapon->query_short()+" and...                              *
     *	room -   attacker_name+" "+verbage[1]+" "+target_name+" with "+attacker_possessive+" "+weapon->query_short()+" and...*
     *	target - attacker_name+" "+verbage[1]+" you with "+attacker_possessive+" weapon->query_short()+" and...              *
     *************************************************************************************************************************/
}


void set_damage(int *dam){
    /*****************************
     * ideally we have this form *
     * ({ 2, 3 })                *
     * 0 is range, 1 is minimum  *
     *****************************/
    damage_matrix = dam;
}

int query_damage(){
    int dam;
    if(!damage_matrix)return random(2)+1;
    dam = random(damage_matrix[0]) + damage_matrix[1];
    return dam;
}

void set_id(string *newid){/* assures we always have id weapon */
    ::set_id (newid + ({ "weapon" }) );
}

/** this is called on each blow to determine if it breaks */
int handle_break(object owner, varargs object armor, int damage){
    int chance;/* may put values like these in a header */
    chance = 75 + quality - (armor ? armor->query_quality() : 0 + damage);/* comparative quality */
    if(random(100) > chance){
	string aname, vname, wname, poss, mess;
	aname = armor ? armor->query_short() : nil;
	vname = owner->query_cap_name();
	wname = this_object()->query_short();
	mess  = aname ? " on the "+aname:"";
	poss  = owner->query_possessive();
	_broke = 1;/* may make this a function */
	owner->unwield(this_object());

	owner->message("You've broken your "+wname+mess+".\n");
	owner->query_environment()->message(vname+" broke "+poss+" "+wname+".\n",
	({ owner }) );
	return 1;
    }
    return 0;

}

int fix_break(){/* this corrects problems with weapons */
    if(_broke){
	_broke = 0;
	return 1;
    }
    return 0;/* not broke */
}
