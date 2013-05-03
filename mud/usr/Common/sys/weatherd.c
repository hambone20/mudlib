# include <game/weather_d.h>

/* TODO: add cyclical "chinese" style years?
   TODO: add descriptors to each month
   TODO: randomized weather returns
   TODO: heartbeat tie in
 */
 
string format_time(mapping time){/* return formatted string, TODO: add spaces to single digit times */
	string *format, ret;
	int i, sz;
	
	format = DATE_PRINT;
	ret = "";
	for(i=0, sz=sizeof(format); i<sz; i++){
		if(time[format[i]] || time[format[i]] == 0){
			ret += (string)time[format[i]];
		}else{
			ret += format[i];/* character */
		}
	}
	return ret;
}

