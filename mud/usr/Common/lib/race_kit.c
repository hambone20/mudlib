/**
 * This is inherited into a player body, and it contains information as to the player's
 * race and gender
 */

#include <game/race_kit.h>
#define STAGES ({"What gender do you wish to be? (male/female)\n",\
                 "What race do you wish to be?\n", \
                 "Keep these stats?\n" })
string gender, race; /**< contain race and gender */
int stage; /**< what stage of input we're at */

void set_gender(string str){
	gender = str;
}

string query_gender(){
	return (gender) ? gender : (gender = GENDERS[2]);
}

void set_race(string str){
	race = str;
}
string query_race(){
	return (race) ? race : "raceless";
}

/**
 * @brief input_to function that takes over input
 * @todo change this over to input_object structure?
 */
object input_to(string str){/* this function receives the initial quest for building a body */
	switch(stage){
	case 0: /* gender */
		if(!str || !sizeof(({ "male", "female" }) & ({ str }))){
			this_object()->message("Male or female?\n");
			return this_object();
		}
		/* valid gender */
		gender = str;
		stage = 1; /* race */
		this_object()->message(STAGES[stage]);
		return this_object();
        case 1: /* race */
		if(!str || !sizeof(RACES & ({ str }))){
			this_object()->message(implode(RACES, " ")+"\n");
			return this_object();
		}
		stage = 2;
		this_object()->message("Your race set to "+str+"\n");
		race = str;
		this_object()->roll_stats(); /* roll stats up with race */
		this_object()->input("score");/* TODO: redesign calls? */
		this_object()->message("\n" + STAGES[stage]);
		
		/* pass on to something else? */
		return this_object();
	case 2: /* we're checking stats */
		if(!str || str == "y" || str == "yes"){
			this_object()->message("Good.\n");
			stage = 3;
			return nil;
		}
		this_object()->roll_stats(); /* roll stats up with race */
		this_object()->input("score");
		this_object()->message("\n" + STAGES[stage]);
		return this_object();
		
	}
	return nil;
}

void init_input(){/* initialize racial/gender setting */
	stage = 0; /* stage 0 = gender */
	this_object()->message(STAGES[stage]);
}

object input_done(){
	this_object()->move(ROOMD->query_start_room(), "");
	return nil;/* get out of input to object code */
}