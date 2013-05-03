/* diamond */

inherit "/usr/Game/lib/Tradeskills/component";

create(int clone){
    set_tradeskill("weaponsmithing");
    set_short("diamond");
    set_long("This is diamond, very difficult but very strong.\n");
    set_alloy("diamond");
    set_bonus(20);
    set_difficulty(20);
    set_subset("alloy");
    set_color("%^BOLD%^%^CYAN%^");
    set_id( ({ "diamond" }) );
}
