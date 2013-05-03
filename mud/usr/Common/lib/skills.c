/**
 *                     skills.c                           
 *  Eventually this will contain formulae for skill       
 *  decay, and a more abstract output                     
 *  @author Hymael                                               
 */
/* need a header for skills, create? */
#define NOT_SKILLED 0

mapping skills;           	   /**< our skills */
static mapping mod_skills;     /**< current skill mods */

void delta_skill(string skill, varargs int val){
    if(!skills) skills = ([]);
    if(!skills[skill]) skills[skill] = NOT_SKILLED;

    if(!val) val = 1;

    skills[skill] += val;
}

void remove_mod(string skill, int val, string message){
    mod_skills[skill] += val;
    if(message)
        this_object()->message(message);
}

/** temp mods for skills api: duration is in seconds and off message */
void mod_skill(string skill, int val, int duration, varargs string message){
    if(!mod_skills) mod_skills = ([]);

    if(duration < 1) return;

    if(!mod_skills[skill]){/* a mod doesn't exist yet */
	mod_skills[skill] = 0;
    }
    mod_skills[skill] += val;
    val = val * -1; /* flip sign */
    call_out("remove_mod", duration, skill, val, message);
}

int query_skill(string skill){
    int ret;

    if(!skills) skills = ([]);
    if(!mod_skills) mod_skills = ([]);

    ret = 0;

    if(!skills[skill]){
	ret = NOT_SKILLED;
    }else{
	ret = skills[skill];
    }
    if(mod_skills[skill]) ret += mod_skills[skill];

    return ret;
}

string query_skills(){/**< dumb text output of skill list */
    string str, *indies;
    int i, x;

    if(!skills){
	skills = ([]);
	return "No skills.\n";
    }
    if(!mod_skills)
	mod_skills = ([]);

    indies = map_indices(skills);
    if(mod_skills){
	indies = indies | map_indices(mod_skills);
    }

    i = sizeof(indies);
    if(i < 1)
	return "No skills.\n";

    str = "Skills\n\n";
    for(x=0; x<i; x++){
	str += pad(capitalize(indies[x]), 15)+": "+query_skill(indies[x])+"\n";
    }
    return str;
}

