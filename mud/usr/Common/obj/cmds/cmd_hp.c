/*
 * the hp command
 */
inherit BODY_COMMAND;

mixed cmd_hp(string cmd, string args, object actor){
#ifdef ASTARIA_CLONE
    actor->message("%^BOLD%^%^CYAN%^hp "+actor->query_health()+" / "+actor->query_max_health()+"%^RESET%^\n");
#else
    actor->message(actor->query_diagram());
#endif
    return 1;
}

mixed help(string cmd, object actor){
    return "The hp command prints your current health.\n";
}
