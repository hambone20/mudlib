#include <config.h>

/* world bank, finance handler */

private object *coins; /* coin handles, too intensive? */

static void create(varargs int clone) 
{
    coins = ({ });
}

void create_coin(object coin, varargs string creator)
{
    coins += ({ coin });
    
}

void coin_change(object coin, string to, int amount)
{
    
}