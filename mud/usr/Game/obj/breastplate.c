inherit ARMOR;

void create(varargs int clone){
    set_short("%^BLUE%^indigo breastplate%^RESET%^");
    set_long(wrap("This is an indigo breastplate.\n", 72));
    set_id( ({ "breastplate", "indigo breastplate" }));
    set_type("breastplate");
    set_ac(5);
    set_weight(40);
    set_bulk(60);
}
