/** test input object */
object user; /* user object associated with this object */

int input(string str){
	if(!user)user = previous_object();

	user->message("User = "+user->query_Name()+".\n");
	return 0;
}

void init_input(){
	user = previous_object();
	user->message("Inited input object.\n");
}