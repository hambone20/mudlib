/**
 * channeld
 * this daemon handles all channel functionality, it may talk with logd
 * to make more useful reporting of errors and events
 */
#include <kernel/user.h>
#include <type.h>
#define CHANS ([ "wiz" : ({}), "myan" : ({}) ])

mapping backlogs, channels; /**< a mapping of the backlogs of channels, and tuned users */
/** @todo people tuned? or add an inherited for user.c */

void create(){/** inits */
    backlogs = CHANS;
    channels = CHANS;
}

/**
 * @brief tune Function to tune an actor to a channel.
 * @param channel String represting channel to tune.
 * @param actor Object to tune to channel.
 * @retval int (0->cannot tune, -1->tuned out, 1->tuned in)
 * @todo security
 */
int tune(string channel, object actor){
    int i;

    if(!channel || (i = member_array(channel, map_indices(channels))) == -1 || !actor->is_body()){
        LOGD->log("Channel improper.\n", "channel");
        return 0;
    }

    /** @todo check if previous_object() can tune channel */
    if(channel == "wiz" && !actor->query_user()->query_wiztool()){
        return 0;
    }

    /** @todo check if previous_object() is tuned? */


    /** flip tune */
    if(member_array(actor, channels[channel]) > -1){
        channels[channel] -= ({ actor });
        return -1;/** tuned out */
    }

    channels[channel] += ({ actor });
    return 1;/**< tuned in */
}

/**
 *  @brief show Show users tuned to a channel.
 *  @param channel String representing channel.
 *  @param actor Object of actor to show.
 *  @retval string that's @em printable
 */
string show(string channel, object actor){
    int i;
    object player, *indies;
    string ret;

    /** check if previous_object() can tune channel */
    if(channel == "wiz" && !actor->query_user()->query_wiztool()){/** @todo be sure it's a wiz, do something here to make this functional */
        return "You can't check this line.\n";
    }

    if(member_array(channel, map_indices(channels)) == -1){
        return "Invalid channel id.\n";
    }

    if(!channels[channel]){
        return "No one tuned into channel: " + channel + ".\n";
    }

    indies = channels[channel];
    ret = "Channel <" + channel + "> is currently listened to by:\n";
    for(i = sizeof(indies); --i >= 0; ){
        ret += indies[i]->query_Name() + " ";
    }

    return ret + "\n";
}


/** @todo valid tunes */

/**
 * @brief broadcast Used to broadcast a message to channel
 * @param channel String of the channel to broadcast on.
 * @param message String of message to broadcast.
 * @param actor Object broadcasting message.
 * @param except @em Optional argument to remove listeners.
 * @todo make except an array?
 */
void broadcast(string channel, string message, object actor, varargs object except){
    int emote, i;
    object *listeners;

    if(!channels[channel] || !message || !strlen(message))
        return;

    if(member_array(actor, channels[channel]) == -1){/**< not tuned in */
        return;
    }

    /** do emotes? */
    if(message[0] == ':'){
        emote = 1;
        message = message[1..];
    }

    message = (emote) ? "[" + channel + "] " + actor->query_Name() + " " + message + "\n" :
    actor->query_Name() + " [" + channel + "] " + message + "\n";

    listeners = channels[channel] - ({ except });
    for(i = sizeof(listeners); i--; ){
        listeners[i]->message(message);
    }

    /** add to backlog */
    backlogs[channel] += ({ message });
    if(sizeof(backlogs[channel]) > 10)
	backlogs[channel] = backlogs[channel][1..];
}

/**
 * @brief get_channels get available channels
 * @retval mapping of channels
 * @todo Cull this based on caller?
 */
mapping get_channels(){
    return channels;
}

/**
 *  @brief cmd_channel this is called when a command is issued, to see if it matches a tuned in channel.
 *  @param chan String of the channel.
 *  @param mess String of the message.
 *  @param actor Object doing the action.
 *  @retval returns 1 if found, 0 if not
 *  @todo check security
 */
int cmd_channel(string chan, string mess, object actor){
    if(!chan || !strlen(chan)) return 0;

    if(channels[chan] && sizeof(channels[chan] & ({ actor }))){/**< valid channel */
        broadcast(chan, mess, actor);
        return 1;
    }

    return 0;
}

/**
 *  @brief tuneout function used to log someone out of channels
 *  @param player @em Optional argument to logout given.
 *  @retval string of channels logged out
 *  @todo verify
 */
string *tuneout(varargs object player){
    mixed *keys, ret;
    int i;

    keys = map_indices(channels);
    if(!player)
	player = previous_object();

    if(!player->is_body())return ({});

    ret = ({});
    for(i = sizeof(keys); i--; ){
        if(sizeof(channels[keys[i]] & ({player}))){/**< player tuned to channel */
            channels[keys[i]] -= ({player});
            ret += ({keys[i]});
        }
    }
    return ret;
}

/** @todo tune in at log in */


/**
 * @brief get_backlogs reports backlogs
 * @retval mapping of the backlogs
 * @todo too intensive on memory?
 */
mapping get_backlogs(){
    return backlogs;
}

