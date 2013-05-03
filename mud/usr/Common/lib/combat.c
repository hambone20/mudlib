/************************************
 *           combat.c               *
 * Any combative souls must inherit *
 * this                             *
 * NOTE: subscribe to event of      *
 * target dying                     *
 ************************************/

/*********************************************************************
 * The combat round is very complex.  First off, we need somethng    *
 * that doesn't drain too much CPU.  This is handled by making sure  *
 * we do something once and only once.  Each weapon has an inherent  *
 * speed.  This speed is the length of the call_out till this weapon *
 * swings again.  Heart beats have nothing to do with combat weaponry*
 * in terms of swings.  Each swing, before getting a chance to land, *
 * filters through some intercepts on the target player, namely a    *
 * shield.  NOTE: each weapon must have it's call_out handle stored. *
 * Below is detailed this process:                                   *
 * COMBAT ROUND:                                                     *
 * -we check for combatant 1's lock, if not, we assume range         *
 * -combatant 1 issues a target on combatant 2                       *
 * -combatant 1                                                      *
 * NOTE: the engage commands and others like it actually do the lock *
 *       aspect not this code                                        *
 *********************************************************************/
#include <game/body.h>

private object *locked_with;	/* objects whom this object is locked into melee with */
private object *angered;	/* objects whom this object has angered, namely with range */
private int block_attack;	/* handles stuns */
private mapping weapon_handles;	/* these are the call_out handles for weapons */

/* hit stack */
#define _ATTACKER      stack[0]
#define _ATTACKED      stack[1]
#define _WEAPON        stack[2]
#define _ATTACKER_NAME stack[3]
#define _TARGET_NAME   stack[4]
#define _ATTACKER_POS  stack[5]
#define _ATTACKED_POS  stack[6]
#define _PART          stack[7]
#define _STR           stack[8]

/* TO-HIT */
#define TO_HIT random(100) > (_ATTACKED->ac_of(_PART) * 3) + ( (_ATTACKED->query_state() & S_PRONE) ? -10 : (_ATTACKED->query_stat("dexterity")) * 3) - _STR - _WEAPON->query_accuracy()

/* messages masks for ease of editing */
#define MY_HIT 	"You " + verbage[0] + " " + _TARGET_NAME + "'s " + _PART + " with your " + weapon_short + " and " + dam_mess[0] + "\n"

#define  O_HIT 	_ATTACKER_NAME + " " + verbage[1] + " " + _TARGET_NAME + "'s " + _PART + " with " + _ATTACKER_POS + " " + weapon_short + " and " + dam_mess[1] + "\n"

#define  T_HIT 	_ATTACKER_NAME + " " + verbage[1] + " your " + _PART + " with " + _ATTACKER_POS + " " + weapon_short + " and " + dam_mess[1] + "\n"

#define MY_MISS "You attack " + _TARGET_NAME + "'s " + _PART + " with your " + weapon_short + " and miss!\n"

#define  O_MISS _ATTACKER_NAME + " attacks " + _TARGET_NAME + "'s " + _PART + " with " + _ATTACKER_POS + " " + weapon_short + " and misses.\n"

#define  T_MISS _ATTACKER_NAME + " attacks your " + _PART + " with " + _ATTACKER_POS + " " + weapon_short + " and misses you!\n"

/***PROTOS***/
void vector_weapons ();
/***PROTOS***/

void pacify(){
	locked_with = ({});
	angered = ({});
}

void
create ()
{				/* NOTE: this must be called */
  pacify();
  block_attack = 0;
  weapon_handles = ([]);
}

int query_is_attackable ()
{
  return 1;
}

void
cease_weapons ()
{				/* stops all weapon call_outs */
  int *values, i;
  values = map_values (weapon_handles);
  i = sizeof (values);

  /* remove all call_outs */
  while (--i >= 0)
    {
      remove_call_out (values[i]);
    }
  weapon_handles = ([]);
}

void
cease_weapon (object weapon)
{				/* stop the swinging of a particular weapon */
  if (weapon_handles[weapon])
    {
      remove_call_out (weapon_handles[weapon]);
      weapon_handles[weapon] = nil;
    }
  if (sizeof (locked_with) && !map_sizeof (weapon_handles))
    {
      call_out ("vector_weapons", 0);
    }
}
int
lock_with (object enemy)
{
  locked_with += (
		   {
		   enemy});
  if (member_array (enemy, angered) == -1)
    {
      angered += (
		   {
		   enemy}
      );
    }
  return (this_object ()->set_state (S_LOCK) && enemy->set_state (S_LOCK));
}

object
query_target ()
{
  int i, x;
  object room;
  i = sizeof (locked_with);
  room = this_object ()->query_environment ();
  for (x = 0; x < i; x++)
    {
      if (room->object_present (locked_with[x])
	  /* && (locked_with[x]->query_state() & S_LOCK) */
	)
	{
	  return locked_with[x];
	}
    }
  locked_with = (
		  {
		  }
  );
  return nil;
}


/******************
 * the miss       *
 ******************/
void
do_miss (mixed * stack)
{
  string weapon_short;
  weapon_short = _WEAPON->query_weapon_name ();
  _ATTACKER->message (MY_MISS + "\n");
  _ATTACKED->message (T_MISS);
  _ATTACKER->query_environment ()->message (O_MISS, (
						      {
						      _ATTACKER, _ATTACKED}));
}

/******************
 * the actual hit *
 ******************/
void
do_hit (mixed * stack)
{
  string weapon_short, *verbage, *dam_mess;
  int damage;
  weapon_short = _WEAPON->query_weapon_name ();
  verbage = _WEAPON->query_verbage ();
  damage
    =
    _WEAPON->query_damage () + (_STR * 2) - random (_ATTACKED->ac_of (_PART));
  if (damage < 1)
    damage = 1;
  switch (damage)
    {
    case 1..3:
      dam_mess = (
		   {
		   "hit wimpishly.", "hits wimpishly."}
      );
      break;
    case 4..5:
      dam_mess = (
		   {
		   "score a graze.", "scores a graze."}
      );
      break;
    case 6..12:
      dam_mess = (
		   {
		   "strike surely.", "strikes surely."}
      );
      break;
    case 13..20:
      dam_mess = (
		   {
		   "hit softly.", "hits softly."}
      );
      break;
    case 21..40:
      dam_mess = (
		   {
		   "hit solidly.", "hits solidly."}
      );
      break;
    case 41..60:
      dam_mess = (
		   {
		   "hit critically!", "hits critically!"}
      );
      break;
    default:
      dam_mess = (
		   {
		   "lay waste!", "lays waste!"}
      );
    }
  _ATTACKER->message (MY_HIT + "\n");
  _ATTACKED->message (T_HIT);
  _ATTACKER->query_environment ()->message (O_HIT, (
						     {
						     _ATTACKER, _ATTACKED}
					    ));
  _ATTACKED->damage_part (_PART, _ATTACKER, damage);

/*
	catch{
		_ATTACKED->attack(_ATTACKER);
		if(_ATTACKER->query_environment()->object_present(_ATTACKED)){
			call_other( weapon, weapon->query_hit_func(), attacker, attacked );
		}
	}
	/* possibly add in armor specials */
}

int
swing (object weapon)
{				/* each swing of a weapon */
  object target;
  locked_with -= (
		   {
		   nil});
  if (!locked_with || !sizeof (locked_with))
    {				/* no pending enemy */
      cease_weapons ();
      this_object ()->message ("The combat has ended.\n");
      return 0;
    }
  if (!(target = query_target ()))
    {				/* no target */
      cease_weapons ();
      this_object ()->message ("Your enemy has fled.\n");
      return 0;
    }
  if (!target->intercept (this_object (), weapon))
    {
      string body_part;
      int strength;
      mixed *stack;
      stack = allocate (9);
      _ATTACKED = target;
      if (!(_PART = _ATTACKED->target ()))
	return 0;

      /* load up stack */
      _WEAPON = weapon;
      _ATTACKER_NAME = this_object ()->query_Name ();
      _TARGET_NAME = target->query_Name ();
      _ATTACKER_POS = "his";	/*this_object()->query_possessive(); */
      _ATTACKED_POS = "his";	/* target->query_possessive(); */
      _ATTACKER = this_object ();
      _STR = this_object ()->query_stat ("strength");

      /* determine */
      if (TO_HIT)
	{			/* we have a hit */
	  do_hit (stack);
	}
      else
	{			/* miss */
	  do_miss (stack);
	}
    }
  if (weapon->is_broke ())
    {
      weapon_handles[weapon] = nil;
      return 1;
    }

  {
	 /***SPEED***/
    int speed;
    speed = weapon->query_speed ();
    if (!speed)
      speed = random (3) + 1;
    if (this_object ()->query_primary () != weapon)
      speed += random (3) + 2;
    weapon_handles[weapon] = call_out ("swing", speed, weapon);
  }
	 /***END SPEED***/
  return 1;
}

int
query_attacking (object target)
{
  return member_array (target, locked_with) > -1;
}

void
vector_weapons ()
{				/* start a swingin' */
  object *weapons;
  int i;
  if (!sizeof (locked_with))
    return;
  cease_weapons ();
  weapons = this_object ()->query_weapons ();
  if (weapons && (i = sizeof (weapons)))
    {				/* armed combat */
      int speed;
      object primary;
      primary = this_object ()->query_primary ();
      while (i--)
	{
	  speed = 1;
	  if (primary != weapons[i])
	    {
	      speed += random (3) + 1;
	    }
	  weapon_handles[weapons[i]] = call_out ("swing", speed, weapons[i]);
	}
    }
  else
    {				/* unarmed combat */
      if (!this_object ()->query_arms ())
	{
	  this_object ()->message ("Arm yourself!\n");
	  return;
	}
      weapon_handles
	[this_object ()] = call_out ("swing", random (3) + 1, this_object ());
    }
}
int
attack (object target)
{
  int i;
  i = member_array (target, locked_with);
  if (i == 0)
    return 2;
  if (i < 0)
    {				/* add him */
      if (sizeof (locked_with))
	{
	  locked_with += (
			   {
			   locked_with[0]}
	  );
	  locked_with[0] = target;
	}
      else
	{
	  locked_with = (
			  {
			  target}
	  );
	}

      /* so he gets precedance */
      catch (subscribe_event (target, "death"));
      if (!target->query_attacking (this_object ()))
	target->attack (this_object ());
      if (!map_sizeof (weapon_handles))
	vector_weapons ();
      return 1;
    }
  else
    {
      object temp;

      /* guess he's a member, move to front */
      temp = locked_with[0];
      locked_with[0] = locked_with[i];
      locked_with[i] = temp;
    }
  return 2;
}

void
stop_attacking (object obj)
{
  int i;
  i = member_array (obj, locked_with);
  if (i == -1)
    return;
  locked_with -= (
		   {
		   locked_with[i]}
  );
  if (!sizeof (locked_with))
    block_attack = 0;		/* we receive our actions back */

  /* may release lock state if empty */
}

void
evt_death (object obj, object dead)
{
  stop_attacking (dead);
  catch (unsubscribe_event (dead, "death"));
}
