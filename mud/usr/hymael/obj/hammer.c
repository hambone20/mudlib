/* an anvil */

inherit "/usr/Game/lib/Tradeskills/component";

void create(int clone) {
    set_tradeskill("weaponsmithing");
    set_short ("a weaponsmith's hammer") ;
    set_long (wrap("This is a hammer.\n",72)) ;
    set_id ( ({ "hammer" }) );
    set_subset("hammer");
}

