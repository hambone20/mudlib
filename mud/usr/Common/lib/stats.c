/**
 *                     stats.c                            
 *  @author Hymael                                               
 */
#define ATTRIBUTES ({ "strength", "constitution", "dexterity", "intelligence", "wisdom", "charisma" })
#define ROLL_STAT  ( random(6) + random(6) + random(6) + 3 )

mapping stats;           		/**< our stats */
static mapping stats_boost;     /**< temporary stat boosts */

void roll_stats(){/**< this will randomly populate stats */
    int i;
    if(!stats) stats = ([]);
    i = sizeof(ATTRIBUTES);
    while(--i >= 0){
	stats[ATTRIBUTES[i]] = ROLL_STAT;
    }
}

void create(varargs int clone){
	stats = ([]);
	stats_boost = ([]);
	roll_stats();
}

void set_stat(string stat, int val){
    if(!stats) stats = ([]);
    if(member_array(stat, ATTRIBUTES) < 0)return;
    stats[stat] = val;
}

int query_stat(string stat){
    int i;
    if(!stats) stats = ([]);
    if(!stats_boost) stats_boost = ([]);
    i = (stats_boost[stat]) ? stats_boost[stat] : 0;
    i += (stats[stat]) ? stats[stat] : 0;
    return i;
}

mapping query_stats(){/**< full mapping */
    mapping ret;
    int i;
    if(!stats) stats = ([]);
    if(!stats_boost) stats_boost = ([]);
    ret = ([]);

    i = sizeof(ATTRIBUTES);
    while(i--){
	ret[ATTRIBUTES[i]] = query_stat(ATTRIBUTES[i]);
    }
    return ret;
}

void boost_stat(string stat, int val){/* may add a timer */
    if(!stats_boost) stats_boost = ([]);
    if(!stats_boost[stat]) stats_boost[stat] = 0;
    stats_boost[stat] += val;
}
