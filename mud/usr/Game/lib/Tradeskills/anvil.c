/**
 * This is an anvil for the weapon-smithing tradeskill               
 * @todo make object stubs for inserting in here
 * generally bring it in line with new mudlib
 */
#include <game/Tradeskills/weaponsmithing.h>
# define OBJ THING /**< rudimentary object */
/** things are put into this and "combined" */
inherit container CONTAINER;

#define REUSE_TIME 20
#define HEAT_TIME  60

#define _DIF 0
#define _BON 1
#define _WT  2

/** formulae */
#define TP this_player() /**< temporary hack */
#define TPN TP->query_Name()
#define write(x) TP->message(x)
#define say(x) query_environment()->message((x), ({TP}))

#define FAIL_SHAPE         ( random(10) < values[_DIF] ) /**< checked in shape */
#define SHAPE_BEYOND_SKILL ( (values[_DIF] -= TP->query_skill("shaping") + TP->query_stat(W_SMITH_PA)) > 12 )
#define TRIVIAL            ( values[_DIF] <= 0 )
#define SKILL_UP           ( random(10) > values[_DIF] )
#define REPAIR_FAIL        ( random(10) < 1 )
#define FAB_BEYOND_SKILL   ( (values[_DIF] -= TP->query_skill("shaping") + TP->query_stat(W_SMITH_PA)) > 12 )
#define FAB_FAIL           ( (TPN != "Hymael") && (random(10) < values[_DIF]) )
#define CALC_SPEED         ( (int)(floor(8.0 - log((float)values[_BON]) + 2.0)) )
#define CALC_DAMAGE        ( (int)(ceil(log((float)values[_BON] - 5.0) + 10.0)) )

/** the attributes set below will determine how good this particular anvil is */
int  _grade;       /**< the relative quality of this anvil */
static int  _heat;        /**< is anvil ready */

/** NOTE: create defines commands that are necessary */
void create(varargs int clone){
    set_bulk(200);
    set_weight(500);
    set_capacity(500);
    set_volume(500);
    set_short("anvil");
    set_long("An anvil.\n");
    set_id( ({ "anvil" }));


    add_command("heat",      "heat");
    add_command("shape",     "shape");
    add_command("fabricate", "fabricate");
    add_command("repair",    "repair");
    container::create();
}

void set_grade(int val){ _grade = val; }
int query_grade()      { return _grade; }

/** get the process started */
mixed heat(string str){
    if(str != "anvil") return "Heat what?\n";

    if(_heat > time())
	return "The anvil is already heated.\n";

    if(_heat + REUSE_TIME > time())
	return "The anvil has been used recently and needs cooling.\n";

    _heat = time() + HEAT_TIME; /**< heated for time */
    write("You operate the bellows to the anvil and bring it to a working heat.\n");
    say(TPN+" operates the bellows and brings the anvil to a working heat.\n");
    return 1;
}

/** returns oil found */
object find_oil(){
    object *stuff;
    int i;
    stuff = this_object()->query_inventory();
    if((i = sizeof(stuff)) < 1)
	return nil;

    while(i--){
	if(stuff[i]->query_subset() != C_OIL) continue;
	return stuff[i];
    }
    return nil;
}

int *tally_values(object *stuff){
    int i, difficulty, bonus, wt;
    i = sizeof(stuff);
    while(i--){
	difficulty += stuff[i]->query_difficulty();
	bonus      += stuff[i]->query_bonus();
	if(stuff[i]->query_subset() == C_HAMMER ||
	  stuff[i]->query_subset() == C_OIL)
	    continue;

	wt         += stuff[i]->query_weight();
    }
    return ({ difficulty, bonus, wt });
}
/** @todo screen receive object for only weaponsmithing componenets */

/** shape something */
mixed shape(string str){
    mixed component;
    string shape;
    object hammer, oil;
    int *values;

    if(!str) return "What do you wish to shape?\n";

    if(_heat < time()){
	return "You must heat the anvil first.\n";
    }

    if(sscanf(str, "%s into %s", component, shape) != 2){
	return "Shape what into what?\n";
    }

    if(!(hammer = TP->present(C_HAMMER)) || !hammer->is_component())
	return "You need a hammer.\n";

    if(!(component = this_object()->present(component)))
	return "You need the component inside the anvil.\n";

    if(!component->is_component() || component->query_subset() != C_ALLOY)
	return "That is not an alloy to be shaped.\n";

    /** check for valid shape */
    /** find oil */
    if(!(oil = find_oil()))
	return "You need oil to shape something.\n";

    values = tally_values( ({ oil, component, hammer }) );

    if(SHAPE_BEYOND_SKILL)
	return "Shaping this is beyond your skill.\n";

    values[_BON] += _grade; /**< grade of anvil helps */

    if(FAIL_SHAPE){/**< botch */
        object pieceofshit;
        string alloy;
        alloy = component->query_alloy();
        write("You pound out the alloy and draw it into a twisted semblance of a "+shape+".\n");
        say(TPN+" pounds at the anvil and after a time throws aside the project.\n");
        pieceofshit = THINGD->get_clone(OBJ);
        pieceofshit->set_id( ({ "junk", shape }) );
        pieceofshit->set_short("A malformed "+alloy+" "+shape);
        pieceofshit->set_long("This is a malformed "+shape+".\n");
        pieceofshit->set_weight(component->query_weight());
        pieceofshit->move(TP);
    }else{
        object drawnshape;
        string alloy;
        alloy = component->query_alloy();
        write("You draw out the alloy expertly and shape it into a "+shape+".\n");
        say(TPN+" expertly draws the alloy into a "+shape+".\n");
        /** @todo may functionize this */
        drawnshape = THINGD->get_clone(COMPONENT);
        drawnshape->set_id( ({ shape, alloy, alloy+" "+shape, "shape" }) );
        drawnshape->set_short(article(alloy)+" "+alloy+" "+shape);
        drawnshape->set_alloy(alloy);
        drawnshape->set_color(component->query_color());
        drawnshape->set_subset(shape);
        drawnshape->set_tradeskill("weaponsmithing");
        drawnshape->set_difficulty(values[_DIF]);
        drawnshape->set_bonus(values[_BON]);
        drawnshape->set_long("This is a crafted "+shape+" made from "+alloy+".\n");
        drawnshape->set_weight(values[_WT]);
        drawnshape->move(TP);
    }
    /** think of skilling up this player */
    if(TRIVIAL){
	write("Making that item has become a trivial task for you.\n");
    }else if(SKILL_UP){
	write("You've become better at shaping.\n");
	TP->delta_skill("shaping");
    }
    /** to shape we need oil and the thing which we want to shape, plus a hammer 
     * use up components 
     */
    oil->use_component();
    component->use_component();
    return 1;
}

mixed repair(string str){
    object weapon, oil, hammer;

    if(!str)
	return "What item do you wish to repair?\n";

    if(!(hammer = TP->present(C_HAMMER)) || !hammer->is_component())
	return "You need a hammer.\n";

    if(_heat < time())
	return "You must heat the anvil first.\n";

    if(!(weapon = this_object()->present(str)))
	return "Cannot find "+str+" to fix.\n";

    if(!weapon->is_weapon())
	return "That isn't a weapon.\n";

    if(!weapon->is_broke())
	return "That thing is already in fine shape.\n";

    if(!(oil = find_oil()))
	return "You need oil to repair something.\n";

    if(REPAIR_FAIL){
	str = weapon->query_short();
	write("You fail to repair the "+str+".\n");
	say(TPN+" fails in his efforts to repair the "+str+".\n");
    }else{
	weapon->fix_break();
	str = weapon->query_short();
	write("You expertly repair the "+str+".\n");
	say(TPN+" expertly fixes the "+str+".\n");
    }
    /** @todo skill up? */
    return 1;
}

string query_short(){
    return ::query_short()+((_heat > time()) ? " %^B_RED%^%^BOLD%^[heated]%^RESET%^" : "");
}

mapping check_ingredients(string *ingreds){
    object *stuff;
    mapping ret;
    int i, cur;
    string sub;
    ret = ([]);
    stuff = this_object()->query_inventory();
    i = sizeof(stuff);
    while(i--){
        if((cur = member_array((sub = stuff[i]->query_subset()), ingreds)) > -1){
            ret[sub] = stuff[i];
            ingreds[cur] = nil;
            ingreds -= ({ nil });
            if(sizeof(ingreds) < 1)
            return ret;
        }
    }
    return nil;
}

/** this is the final product */
mixed fabricate(string str){
    string shape;
    object hammer, oil, *ostuff;
    mapping stuff;
    int *values, i;

    if(!str) return "What do you want to fabricate?\n";

    if(_heat < time()){
	return "You must heat the anvil first.\n";
    }

    if(!(hammer = TP->present(C_HAMMER)) || !hammer->is_component())
	return "You need a hammer.\n";

    if(!W_SMITH_REC[str])
	return "That is not a valid weapon.\n";

    /** check for ingredients */
    stuff = check_ingredients(W_SMITH_REC[str]);

    if(!stuff)
	return "Your missing components to fabricate that.\n";

    /** find oil */
    if(!(oil = find_oil()))
	return "You need oil to fabricate something.\n";

    ostuff = map_values(stuff);
    values = tally_values( ostuff + ({ oil, hammer }) );

    if(FAB_BEYOND_SKILL)
	return "Fabricating this is beyond your skill.\n";

    values[_BON] += _grade; /**< grade of anvil helps */

    if(FAB_FAIL){/**< botch */
        object pieceofshit;
        write("You fabricate something that has a twisted semblance of a "+str+".\n");
        say(TPN+" pounds at the anvil and after a time throws aside the project.\n");
        pieceofshit = THINGD->get_clone(OBJ);
        pieceofshit->set_id( ({ "junk", str }) );
        pieceofshit->set_short("A malformed "+str);
        pieceofshit->set_long("This is a malformed "+str+".\n");
        pieceofshit->set_weight(values[_WT]);
        pieceofshit->move(TP);
    }else{/** build weapon */
        object weapon;
        string alloy, color;
        int speed;
        int damage;
        alloy = stuff[W_SMITH_REC[str][0]]->query_alloy();/* should be blade */
        color = stuff[W_SMITH_REC[str][0]]->query_color();
        write("You piece together the objects into a fine "+str+".\n");
        say(TPN+" expertly pieces together a "+str+".\n");
        /* debug */
        write("DEBUG - bonus: "+values[_BON]+" difficulty: "+values[_DIF]+"\n");
        /* debug */
        weapon = THINGD->get_clone("/usr/Common/obj/weapon");
        weapon->set_id( ({ str, alloy+" "+str }) );
        weapon->set_short(color+alloy+" "+str+"%^RESET%^");
        weapon->set_quality(values[_BON]+TP->query_skill("weaponsmithing"));

        speed = CALC_SPEED;
        write("speed: "+speed+"\n");

        weapon->set_speed(speed);
        weapon->set_type("cut");/* to be made after header */
        weapon->set_accuracy(values[_BON]);

        damage = ( (values[_BON] > 5) ? CALC_DAMAGE : 1 );
        write("damage: "+damage+"\n");

        weapon->set_damage( ({ random(damage), damage }) );
        weapon->set_long("This is a crafted "+str+" with a blade of "+alloy+".\n");
        weapon->set_weight(values[_WT]);
        weapon->move(TP);
    }
    /** think of skilling up this player */
    if(TRIVIAL){
	write("Making that item has become a trivial task for you.\n");
    }else if(SKILL_UP){
	write("You've become better at weapon smithing.\n");
	TP->delta_skill("shaping");
    }

    /** use up components */
    oil->use_component();
    i = sizeof(ostuff);
    while(i--){
	ostuff[i]->use_component();
    }
    return 1;
}
