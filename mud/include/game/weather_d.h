/**
 * Weather Daemon Includes
 */

# define WEATHERD "/usr/Common/sys/weatherd" /**< our daemon */
 
# define NUM_MONTHS 12 /**< number of months in our world */

# define DATE_PRINT ({ "weekday", " ", "month", " ", "day", " ", "hour", ":", "minute", ":", "second", " ", "year" }) /**< use this to step through mapping */

# define MONTHS ([ "Jan" : "Ettinmoor", "Feb" : "Bittertort", "Mar" : "Sylvanmarch", "Apr" : "Caberdoor", "May" : "Seermoon", "Jun" : "Wolfrise", "Jul" : "Balestreak", "Aug" : "Highmarch", "Sep" : "Fadegrip", "Oct" : "Ghastmarch", "Nov" : "Nighmarch", "Dec" : "Avendoor" ])
# define BEGINNING_OF_TIME 0 /**< this is the 0 second for our mud */
# define GAME_TIME_RAW (ctime(((time()<<2)-BEGINNING_OF_TIME))) /**< return raw string, we mask with our months */
# define YEAR_OFFSET -1990 /**< year gets adjusted by this */

/**
 *     @brief get_date Returns a mapping with the game date.
 *     @retval  mapping with month, year, weekday, day, hour, minute, second values.
 *     @todo Clean up.
 */
mapping get_date(){
	mapping date;
	
	date = ([]);
	
	if(sscanf(GAME_TIME_RAW, "%s %s %d %d:%d:%d %d", date["weekday"], date["month"], date["day"], 
              date["hour"], date["minute"], date["second"], date["year"]) != 7){/**< received malformed game_time */
		error("Weather daemon problem\n");
	}
	/** do our adjustments */
	date["month"] = MONTHS[date["month"]];
	date["year"] = date["year"] + YEAR_OFFSET;
	return date;
}
