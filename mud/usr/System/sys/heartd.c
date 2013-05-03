/**
 *  heart beat daemon       
 *  /usr/Common/heartd.c    
 *  issues event wide pulse 
 *  api is for a on creation, a body subscribes to HEARTD and have
 *  a function in it that is evt_heart_beat 
 */

#define HB "heart_beat"
#define HB_LENGTH 3

int paused;   /**< whether the beat is paused */

/**
 *  @brief vector_heart_beat Pulse and revector
 *  @todo Overhaul, add Defines
 */
void vector_heart_beat(){
    if(paused) return;

    event(HB); /**< propogate event */
    call_out("vector_heart_beat", HB_LENGTH); /**< vector */
}

/**
 *  @brief Init heart beat daemon
 */
void create(varargs int clone){
    add_event(HB);
    paused = 0;
    vector_heart_beat();
}

int query_paused(){ return !!paused; }
void set_paused(int val){ paused = val; }

/**
 *  @brief query_sentients get beings who have heartbeats
 */
object *query_sentients(){
    return query_subscribed_event(HB);
}

/**
 * @brief Controls who can subscribe to heartbeat.
 * @todo check security?
 */
int allow_subscribe(object obj, string name){
    /*if(!sscanf(object_name(obj), "%*s#%*s"))return 0;/* only cloned objects need heartbeats? */
    return 1;/**< @todo cull out non body issues? */
}


