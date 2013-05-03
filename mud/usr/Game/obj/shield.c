/* test subject */

inherit ARMOR;

void create(varargs int clone){
    set_short("%^BOLD%^a coruscant shield%^RESET%^");
    set_long("This shield shines brightly, illuminating its owner with its brightness.\n");
    set_id( ({ "shield", "coruscant shield" }));
    set_type("shield");
    set_ac(10);
    set_weight(10);
    set_bulk(20);
}

