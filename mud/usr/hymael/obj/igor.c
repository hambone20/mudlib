inherit "/usr/Common/lib/attic/monster";

void create(int clone) {
::create(clone);
    set_short ("Igor") ;
    set_long ("Igor, the brain-toting wonder monster.\n") ;
    set_id ( ({ "igor" }) ) ;
    enable_commands();
    set_health(750);
    set_gender("male");
    set_damage(({ 40, 30 }));
    set_accuracy(50);
    set_verbs_me(({ "slash", "strike", "cut" }));
    set_verbs_other(({ "slashes", "strikes", "cuts" }));
        set_weapon_name("claws");
        set_follow();
    set_hit_func("tornado");
}

int tornado(object attacker, object victim){
        int dam;
        switch(random(100)){
        case 0..20:
                dam = random(65)+1;
                attacker->mesage("%^BOLD%^%^RED%^You fly into a tornado.%^RESET%^\n");
                victim->message("%^BOLD%^%^RED%^"+attacker->query_cap_name()+" flies into a vicious tornado, "+
                                "tearing into your stomach with crazed, animalistic abandon!%^RESET%^\n");
                attacker->query_environment()->message("%^BOLD%^%^RED%^"+attacker->query_cap_name()+" flies "+
                                "into a vicious tornado of motion, ripping apart "+victim->query_cap_name()
                                +"!%^RESET%^\n",
                                ({ attacker, victim }));
                victim->receive_damage(dam, attacker, this_object()->query_type());
                return 1;
        }
        return 0;
}

