 /** additions to auto
  * @todo clean up and place somewhere else?
  */
 /**
  *   @brief return the index of the element in the array, or -1
  *   @param elt The element to search for.
  *   @param arr The array to search in.
  */
 static int member_array(mixed elt, mixed *arr){
     int i, sz;

     sz = sizeof(arr);
     for(i=0; i<sz; i++){
         if(arr[i] == elt)
             return i;
     }
     return -1;
 }

 /**
  *   @brief pad takes a mixed arg, converts to string, and pads it to the desired
  *          length. Left-justify is the default: if the justify arg is 1, it will
  *          be right-justified.
  *   @param str The string to pad.
  *   @param size Size to pad to.
  *   @param justify Optional argument to justify it rightly.
  */
 static string pad(string str, int size, varargs int justify){
     int i, len;

     len = strlen(str);
     if(len >= size)
        return str[0..size - 1];
     len = size - len;
     for(i=0; i<len; i++){
         if(justify)
         str = " "+str;
         else
            str += " ";
     }
     return str;
 }

/**
 *   @brief Get the inverse of the given direction.
 *   @param dir The string of the direction to flip.
 */
static string flip_dir(string dir){
    if(member_array(dir, A_DIRS) > -1)
        return DIRS[dir];

    return "";
}

/**
 *   @brief capitalize Capitalize the given string.
 *   @param str String to capitalize
 *   @todo Strip so it works with ansi?
 */
 static string capitalize(string str){
    if(!str)
        return str;

    if(str[0] >= 'a' && str[0] <= 'z')
        str[0] -= 'a' - 'A';

    return str;
 }

/**
 *   @brief lowercase Lowercase the given string.
 *   @param str String to lowercase.
 *   @todo Strip so it works with ansi?
 */
 static string lowercase(string str){
    int x;
    if(!str)
        return str;

    x = strlen(str);

    while(--x >= 0){
        if(str[x] >= 'A' && str[x] <= 'Z')
            str[x] += 'a' - 'A';
    }
    return str;
 }

/**
 *   @brief wrap Wrap the given string to a length of len.
 *   @param str String to wrap.
 *   @param len Length to wrap to, defaults to 72.
 *   @todo Strip so it works with ansi?
 */
 static string wrap(string str, int len){
    string ret;
    int i, slen;
    
    if(len < 1)
        len = 72;

    ret="";
    /** start at len and search back for space, place \n
     *  and then clip and paste
     */
    while(strlen(str) > len){
        for(i = len - 1; i > 0 && str[i] != ' '; i--);  /**< i should be a space or 0 */
        
        if(i == 0)i = len - 1;
        
        ret += str[..i] + "\n";
        str = str[i + 1..];
    }
    if(str != "")ret += str;
    
    return ret;
 }


/**
 *   @brief find_player Find a player by name.
 *   @param str String of player's name to find.
 *   @retval object player or nil
 */
static object find_player(string str){
    object *users;
    int i;

    str = lowercase(str);
    users = users();
    if(!(i = sizeof(users)))
        return nil;

    while(i--){
        if(users[i]->query_user()->query_name() == str)
            return users[i]->query_user()->query_body();
    }
    return nil;
}

/**
 *   @brief this_player Delegates to BRAIND and finds this player.
 */ 
static object this_player(){
	return find_object(BRAIND)->this_player();
}
 
/**
 *   @brief replace_string Find pattern in given string and replace with the given string.
 *   @param str String to do replace in.
 *   @param pattern String to replace.
 *   @param replace String to put in pattern's place.
 */
static string replace_string (string str, string pattern, string replace){
    int plen, slen, cursor;
    string ret;

    if (!str)
        return "";

    plen = strlen(pattern);
    if (!pattern || !plen)
        return str;

    slen = strlen (str);

    cursor = -1;
    ret = "";
    while ((slen - (++cursor + plen)) >= 0 && cursor <= (slen - 1)){
        if (str[cursor..cursor + plen - 1] == pattern){/**<found in string */
            ret += str[..cursor - 1] + replace;
            str = str[cursor + plen..];
            cursor = -1;
            slen = strlen (str);
        }
    }
    if (slen != 0)
    ret += str;
    return ret;
}

/**
 *   @brief strip Strips the given string of leading and trailing spaces, replacing 
 *                them with an optional string or a space.
 *   @param str String to strip.
 *   @param stripit Optional replacement for stripped.
 *   @retval stripped string.
 *   @todo Strip so it works with ansi?
 */
static string strip(string str, varargs int stripit){
	int i, sz, slice_start, slice_end;
	
	if(!stripit)
		stripit = ' ';
	
	slice_start=0;
	sz = strlen(str);
	slice_end=sz-1;
	for(sz = strlen(str), i=0;i < sz;i++){/**< leading stripit */
		if(str[i] == stripit) slice_start++;
		else break;
	}	
	
	for(i=sz;--i >= 0;){/**< ending stripit */
			if(str[i] == stripit) slice_end--;
			else break;
	}	
	return str[slice_start..slice_end];
}

/**
 *   @brief diminishing_returns Accepts a float valule and a float scale, then returns a diminshing function.
 *   @author http://lostsouls.org/grimoire_diminishing_returns
 *   @param val Float used to set value.
 *   @param scale Float used to set scale.
 *   @retval float of diminished return.
 *   @todo Document further.
 */
float diminishing_returns(float val, float scale) {
    float mult, trinum;
	
	if(val < 0.0)
        return -diminishing_returns(-val, scale);
    mult = val / scale;
    trinum = (sqrt(8.0 * mult + 1.0) - 1.0) / 2.0;
    return trinum * scale;
}

/**
 *   @brief article Returns article of given string.
 *   @param str String to prepend an article to.
 *   @retval string with prepended article.
 *   @todo Strip ansi, add in proper noun checking?
 */
string article(string str){
	return (sizeof(VOWELS & ({str[0]}))) ? "an" : "a";
}
