inherit WEAPON;

void create(varargs int clone){
    set_short ("%^RED%^Citali, the Swordbreaker%^RESET%^") ;
    set_long (wrap("Citali is a medium-sized dagger designed to be in the offhand. \
It is a twisted thing, with twines lacing its entire length.  It looks as though it was \
tortured at the forge.  Its many hooks and crevices snag weapons and break them like dry \
twigs.\n",72)) ;
    set_id ( ({ "citali", "Citali", "swordbreaker", "dagger" }) ) ;
    set_type ("cut");
    set_accuracy (5);
    set_quality(100);
    set_damage ( ({ 2, 2 }) );
    set_verbs_me (({ "strike", "slash" }));
    set_verbs_other (({ "strikes", "slashes" }));
    set_offhand();
    set_intercept_func("sword_break");
    set_speed(5);
    set_weight(10);
    set_bulk(10);
}

int sword_break(object owner, object attacker, object weapon){

    if(random(100) < 15){/* successful intercept */
	string oname, aname, wname;
	oname = owner->query_Name();
	aname = attacker->query_Name();
	wname = weapon->query_weapon_name();
	owner->message("%^RED%^Citali catches "+aname+"'s %^RESET%^"+wname+"%^RED%^ in its tines.%^RESET%^\n");
	attacker->message("%^RED%^"+oname+"'s strange weapon catches your %^RESET%^"+wname+"%^RED%^ in its tines.%^RESET%^\n");
	attacker->query_environment()->message(oname+"'s strange weapon intercepts "+
	  aname+"'s attack.\n",
	({ owner, attacker })
	);

	weapon->handle_break(attacker, this_object());
	return 1;
    }
}

