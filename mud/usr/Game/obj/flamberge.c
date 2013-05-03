inherit WEAPON;

void create(varargs int clone){
    set_short ("%^RED%^a massive flamberge%^RESET%^") ;
    set_long (wrap("This is a massive flamberge.  \
You feel as though it could tear through sinew with ease.\n",72)) ;
    set_id ( ({ "flamberge", "massive flamberge" }) ) ;
    set_type ("cut");
    set_accuracy (2);
    set_damage ( ({ 190, 10 }) );
    set_verbs_me (({ "strike", "gut" }));
    set_verbs_other (({ "strikes", "guts" }));
    set_two_handed();
}
