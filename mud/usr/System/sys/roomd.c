/**
 *  @brief This daemon handles room management and creation
 *  at startup it will load a few rooms and prepare for launch
 *  This should also manage the removal of rooms without a purpose or a pointer to them
 * @todo divy rooms up into zones, etc.
 */

object *rooms;      /**< handles on rooms */
object start_room;  /**< start room */
object meat_locker; /**< meat locker */
object labyrinth;   /**< where people go when they die */

void connect_rooms(object room1, object room2, string via1, varargs string via2, int oneway);

/**
 * @brief Add a room to the register.
 * @param room Object of room to be added.
 * @retval Object room added.
 */
object add_room(object room){
    if(!rooms)
	rooms = ({});

    if(!room->is_room())
	error("Object is not a room!");
    
    if(sizeof(rooms & ({ room })))
    return room;

    rooms += ({ room });
    return room;
}

void create(varargs int clone){
    rooms = ({});

    /** load our room object */
    if(!find_object(ROOM)) compile_object(ROOM);

    /** create a start room */
    start_room = clone_object(ROOM);
    start_room->set_short("Start room");
    start_room->set_long("This is the ubiquitous start room.\n");

    /** create a meat_locker */
    meat_locker = clone_object(ROOM);
    meat_locker->set_short("This is the meat locker");
    meat_locker->set_long("This is where inactive bodies go to play.\n");

    /** create a labyrinth */
    labyrinth = clone_object(ROOM);
    labyrinth->set_short("%^GREEN%^Labyrinth%^RESET%^");
    labyrinth->set_long (wrap(
      "All around you, your movements echo throughout the vast expanses of the labyrinth. \
Decaying vines pock the area, concealing exits and perceived exits.  You have a difficult \
journey ahead of you.\n", 72));
    labyrinth->set_entrance(wrap("Your lungs draw breath, albeit in an air that differs \
from your mortal existence.  You are now in the labyrinth.  If you intend to assume your \
previous life, you must somehow find your way out of this place...\n", 72));
    connect_rooms(labyrinth, start_room, "live", "", 1);
}

/**
 * @brief return the default room.
 */
object query_start_room(){
    return start_room;
}

/**
 * @brief return the meat locker.
 */
object query_meat_locker(){
    return meat_locker;
}

/**
 * @brief return the labyrinth.
 */
object query_labyrinth(){
    return labyrinth;
}

/**
 * @brief return all rooms
 */
object *query_rooms(){
    return rooms;
}

/**
 * @brief connect room1 with room2 with exit name via.
 * @param room1 Object of room 1.
 * @param room2 Object of room 2.
 * @param via String of exit name.
 * @param oneway integer flag if the exit is one way.
 * @todo review
 */
void connect_rooms(object room1, object room2, string via, varargs string via2, int oneway){
    if(!room1->is_room() || !room2->is_room())
        error("Object not a room");

    if(!via2 || !strlen(via2)){/**< add in a flipped direction */
        via2 = flip_dir(via);
    }

    room1->add_exit(via, room2);
    if(!oneway)
        room2->add_exit(via2, room1);
    /** @todo exit registrar ?? */
}

/**
 * @brief dig_room Creates another room and adds exit from current room to it.
 * @param via Direction to dig.
 * @param via2 Direction back to current room from new room.
 * @param oneway Flag if the room is oneway exit.
 */
object dig_room(string via, varargs string via2, int oneway){
    object room; 

    room = clone_object(ROOM);

    connect_rooms(this_player()->query_environment(), room, via, via2, oneway);
    return room;
}


