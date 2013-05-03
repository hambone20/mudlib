/**
 * this defines what money is
 * and what our world values 
 */
/** values, wise to have 1 in this option */
#define COIN_VALUES ({ 10000,       1000,          100,    10,       1 })
/** names of the corresponding coins */
#define COIN_NAMES  ({ "platinum", "lapis lazuli", "gold", "silver", "copper" })
/** shops rip you off, selling and buying, charisma may be factored in */
#define SHOP_DISCOUNT * 3 / 4
#define CHAR_BOON * 9 / 10

/**
 * @brief exact_change kicks out money as change
 * this function will be used extensively by 
 * vendors to take a number value and change  
 * it into actual coins                       
 * @param value Value to be changed.           
 * @retval mapping of money changed.            
 */
mapping exact_change(int value){
	mapping ret;
	int i, sz, into;
	ret = ([]);
	if(value < 1) return nil;

	sz = sizeof(COIN_VALUES);
	/** now we filter through the value */
	for(i = 0;i < sz && value > 0; i++){
		if((into = value / COIN_VALUES[i])){
			ret[COIN_NAMES[i]] = into;
			value -= (COIN_VALUES[i] * into);
		}
	}
	return ret;
}
