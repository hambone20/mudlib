/*
 * user data, colorsets and backlogs
 */
 
# define MAX_BACKLOG 10
 
mapping backlog; /* expansion of backlogs */
mapping colorsets; /* to hold colorsets */
int brief; /* brief look setting */

/* add onto say_backlog - Hymael
 * transfering to generic mapping of logs
 */

 
void set_brief(int val){ brief = val; }
int query_brief()      { return brief; }

void add_backlog(string logger, string str){
        if(!backlog)backlog=([]);
        if(!backlog[logger]) backlog[logger] = ({ });
        backlog[logger] += ({ str });
        if(sizeof(backlog[logger])>MAX_BACKLOG)
                backlog[logger]=backlog[logger][1..];
}

/* and a query - Hymael */

string *query_backlog(string str){
        string *ret;
        if(!backlog) backlog = ([]);
        if(!backlog[str]) ret = ({ });
        else ret = backlog[str];
    return ret[..];
}

/* called to set our color */
int set_color(string channel, string color){
        if(!colorsets) colorsets = ([]);
        colorsets[channel] = color;
        return 1;
}

/* query given channel */
string query_color(string channel){
        if(!colorsets) colorsets = ([]);
        if(member_array(channel, map_indices(colorsets))>-1){
                return colorsets[channel];
        }
        return "";
}

/* query all existing colors */
mapping query_colors(){
        if(!colorsets)colorsets=([]);
        return copy(colorsets);
}
