/**********************************************************
 *                     health.c                           *
 * NOTE: receive_damage may default to handle splash      *
 *  -Hymael                                               *
 **********************************************************/
#include <game/body.h>

#define SPACE5 "     "
#define RESET  "%^RESET%^"
/* array defines for health */
#define _DAMAGE 0
#define _MAX_HP 1
#define _STATE  2
/* array defines for maligs */
#define M_PART  0
#define M_TIME  1
#define M_DAM   2
#define M_TYPE  3
#define M_MESS  4
#define M_GPART 5 /* the part that gave this malig */

/* color shit */
#define BLEED_COL "%^BOLD%^%^RED%^"

/* limb mess, truely a mess */
#define MY_LIMB   BLEED_COL + dname + " has severed your " + part + ".\n" + RESET
#define  O_LIMB   BLEED_COL + vname + " has lost " + vposs + " " + part + " to " + dname + "'s weapon.\n" + RESET
#define  D_LIMB   BLEED_COL + "You sever " + vname + "'s " + part + " with your blow.\n" + RESET
#define BLEED_M   BLEED_COL + "Your " + malignancies[i][M_GPART] + " " + malignancies[i][M_MESS] + ".\n" + RESET

/* formulae */
#define BLEED_DAM 10

mapping health;           /**< part: ({ damage, max, state of wounds }) */
static mapping parts;
mixed malignancies;       /**< this holds bleeding and such */
string *essential;        /**< essential body parts */


int query_has_health(){	return 1; }

void refresh_health(){
    int i, toughness, temp;
    string *indies;
    if(!health) health = ([]);

    malignancies = ({});
    indies = map_indices(parts);
    i = sizeof(indies);
    toughness = this_object()->query_stat("constitution");
    if(toughness < 1) toughness = 1;
    /* this may query other things */
    while(i--){
        temp = parts[indies[i]] * toughness;
        health[indies[i]] = ({ 0, temp, H_SUPERB });/* start out 0 damage */
    }
}

void init_health(){
    int i, toughness, temp;
    string *indies;

    parts = BODY_PARTS;
    health = ([]);
    essential = BP_ESS;
    refresh_health();
}

void die(){
	event("death", this_object());
	this_object()->cease_weapons();
	this_object()->message("%^BOLD%^%^BLACK%^You have died.%^RESET%^\n");
	this_object()->query_environment()->message(this_object()->query_Name()+" has died.\n", ({this_object()}));
	refresh_health();
	this_object()->pacify();

}

/************************
 * returns health state *
 ************************/
int scale_health(int cur_hp, string part){
    int i;
    i = health[part][_MAX_HP] / sizeof(H_SCALE);
    if(i < 1) i = 1;
    i = cur_hp / i;
    if(i < 0) i = 0;
    catch{
	i = H_SCALE[i];
    }/*:write("i = "+i+" cur_hp = "+cur_hp+"\n");*/
    return i;
}

int damage_part(string part, object dealer, int damage, varargs string type){
    int i;

    if(!part) part = essential[0];/* default to essential */
    if(damage < 0)
	return 0; /* damage you ninny */

    /* resist, damage type vs. armor type */

    health[part][_DAMAGE] += damage;
    i = health[part][_MAX_HP] - health[part][_DAMAGE];

    if(i < 0){
	if(member_array(part, essential) > -1){
	    call_out("die", 0);
	    return 2;
	}else if(health[part][_STATE] != H_LOST){/* lose it */
	    object limb, *stuff, room;
	    string vname, dname, vposs;
	    vname = this_object()->query_Name();
	    vposs = " ";/*this_object()->query_possessive();*/
	    dname = dealer->query_Name();
	    room  = this_object()->query_environment();

	    health[part][_STATE] = H_LOST;
	    /* vector damage to essential */
	    this_object()->add_malignancy(essential[0], -1, BLEED_DAM, "internal", "bleeds profusely", part);
	    /* damage vectored */
	    /* messages */
	    this_object()->message(MY_LIMB);
	    room->message(O_LIMB, ({ dealer, this_object() }));
	    dealer->message(D_LIMB);

	    if(!find_object(LIMB))
		compile_object(LIMB);

	    limb = clone_object(LIMB);
	    stuff = this_object()->disable_limb(part);
	    if(stuff){
		int sz;
		sz = sizeof(stuff);
		while(sz--){
		    stuff[sz]->move(limb);
		}
	    }
	    limb->set_limb(vname, part);
	    limb->move(room);
	}
    }

    if(health[part][_STATE] != H_LOST){
	health[part][_STATE] = scale_health(i, part); /* should be printable with H_PRINT */
    }
    /*if(health[part][_STATE] >= CRITICAL && dealer){/* critical hit code is invoked, in dealer */
    /*dealer->critical(this_object(), part, type, i);
}*/
    return 1;
}

/****************************
 * a malignancy is used to  *
 * vector out damage from a *
 * wound                    *
 ****************************/
void add_malignancy(string part, int time, int damage, string type, varargs string message, string gpart){
    /* the form for each malignancy is:
       body_part, time(in rounds, -1 is until healed),
       damage_per_tick, type, message(nil for none)
     */
    /*if(!malignancies) malignancies = ({});*/

    malignancies += ({ ({ part, time, damage, type, message ? message : nil, gpart ? gpart : part }) });
    /* messages fit form: Your [body part] <bleeds profusely>.
       ideally we don't want many of these going at once that give messages
       since that would get rather spammy
     */
}

void malignant(){/* tick by tick deal with malignancies */
    /* malignancies are removed whenever a the part raises above the critical state */
    int i;
    malignancies -= ({ nil });
    i = sizeof(malignancies);
    while(i--){
	if((health[malignancies[i][M_GPART]][_STATE] < CRITICAL && malignancies[i][M_TIME] < 0) ||
	(malignancies[i][M_TIME] > -1 && --malignancies[i][M_TIME] == -1)){/* this injury is removed */
	    malignancies[i] = nil;
	    continue;
	}

	if(malignancies[i][M_MESS]){/* add observers? */
	    this_object()->message(BLEED_M);
	}
	if(damage_part(malignancies[i][M_PART], nil, malignancies[i][M_DAM], malignancies[i][M_TYPE]) == 2) break; /* death hack */
    }
}
#if 0
/* debug */
string print_maligs(){
	int i;
	string ret;
	i = sizeof(malignancies);
	ret = "...";
	while(i-- >= 0){
ret += "part: "+(string)malignancies[0][M_PART];+" time: "+malignancies[i][M_TIME]+
      " damage: "+malignancies[i][M_DAM]+" mess: "+malignancies[i][M_MESS]+
      " gpart: "+malignancies[i][M_GPART]+"\n");
}
return ret;
}
#endif
void stabilize_part(string part){/* this is to remove any malignancies associated to a part being stabilized */
    int i;
    if(!malignancies) return;

    i = sizeof(malignancies);
    while(i--){
	if(malignancies[i][M_GPART] == part){
	    malignancies[i] = nil;
	}
    }
}
/*****************************
 * and its brother function  *
 * if part is omitted, heal  *
 * is assumed to be holistic *
 * which is very powerful    *
 *****************************/
int receive_healing(int healing, varargs string part){
    if(healing < 1 || (part && health[part][_STATE] == H_SUPERB)) return 0;

    if(part){/* heal body part */
	int i;
	if(health[part][_STATE] == H_LOST) return 0;

	if((health[part][_DAMAGE] -= healing) < 0){
	    health[part][_DAMAGE] = 0;
	    health[part][_STATE]  = H_SUPERB;
	    return 1;
	}
	i = health[part][_MAX_HP] - health[part][_DAMAGE];
	health[part][_STATE] = scale_health(i, part); /* should be printable with H_PRINT */
    }else{/* holistic healing */
	string *indies;
	int i;
	indies = map_indices(parts);
	i = sizeof(indies);
	while(i--){/* recursively heal */
	    receive_healing(healing, indies[i]);
	}
    }
    return 1;
}

/**************************************************************
 * this heals us up, may also add other per heart beat things *
 * this will eventually become race specific and such         *
 **************************************************************/
void do_tick(){/* needs plugging into a heartbeat */
    receive_healing(1);
    if(malignancies) malignant();
    /* do other things, like empty stomachs */
}

string query_color_part(string body_part){
    return H_COLOR[health[body_part][_STATE]];
}

string query_diagram(){
    string printme;
    printme = "\n";
    printme += SPACE5 + query_color_part("head") + "  _\n";
    printme += SPACE5 + " ( )\n" + RESET;
    printme += SPACE5 + query_color_part("right arm") + " /" + RESET;
    printme += query_color_part("torso") + "|" + RESET;
    printme += query_color_part("left arm") + "\\\n" + RESET;
    printme += SPACE5 + query_color_part("right arm") + "/" + RESET;
    printme += query_color_part("torso") + " |" + RESET;
    printme += query_color_part("left arm") + " \\\n" + RESET;
    printme += SPACE5 + query_color_part("right leg") + " /" + RESET;
    printme += query_color_part("left leg") + " \\\n" + RESET;
    printme += SPACE5 + query_color_part("right leg") + "/" + RESET;
    printme += query_color_part("left leg") + "   \\\n\n" + RESET;
    return printme;
}

string query_xa(){/* prints out current status */
    string ret, *stuff;
    int i, sz, temp;
    stuff = BP_ALL;
    sz = sizeof(stuff);
    ret = "\n";
    for(i=0; i<sz; i++){
	temp = health[stuff[i]][_STATE];
	if(temp == H_SUPERB) continue;

	if(temp == H_LOST){
	    ret += " " + capitalize(stuff[i])+" is lost.\n";
	}else{
	    ret += " " + capitalize(stuff[i])+" is "+
	    H_PRINT[temp]+"ly wounded.\n";
	}
    }
    return ret;
}

/****************************************
 * used by equip and wield to determine *
 * if a part is in shape to use an item *
 ****************************************/
int query_heft(string part){
    return health[part][_STATE] != H_LOST;
}

