/** ansi_parser.c
 * @author Tacitus @ LPUniversity
 * @date 17-JUNE-06
 */

# define ANSI(p) ( ESC + "[" + (p) + "m" )
# define ESC "\033" /**< escape character */

string *unsafe_ansi;

string strip_unsafeAnsi(string arg)
{
    int i;

    if(!unsafe_ansi)
	unsafe_ansi = ({ "HOMEC", "CLEAR", "RESETC", "SAVEC", "UNFR", "FRBOTTOM",
	  "FRTOP", "ENDTERM", "INITTERM", "WINDOW", "STATUS", "CLEAR", "ER_SOL",
	  "ER_EOL", "ER_LINE", "ER_DOWN", "ER_UP" });


    for(i = 0; i < sizeof(unsafe_ansi); i++)
    {
	arg = replace_string(arg, "%^" + unsafe_ansi[i] + "%^", "");
    }

    return arg;
}

string parse_pinkfish(varargs string msg, int flag)
{
    mapping data ;
    string *words;
    int size;

    data = ([ "RESET":ANSI("0;37;40"), "BOLD":ANSI(1), "FLASH":ANSI(5),/** @todo: add beep */
      "BLACK":ANSI(30), "RED":ANSI(31), "GREEN":ANSI(32),
      "YELLOW": ANSI(33), "BLUE": ANSI(34), "CYAN":ANSI(36),
      "MAGENTA":ANSI(35), "WHITE": ANSI(37),
      "B_RED":ANSI(41), "B_GREEN":ANSI(42), "B_ORANGE":ANSI(43),
      "B_YELLOW": ANSI(43), "B_BLUE":ANSI(44),
      "B_CYAN":ANSI(46), "B_BLACK":ANSI(40), "B_WHITE": ANSI(47),
      "B_MAGENTA":ANSI(45), "STATUS":"", "WINDOW":""/*, "INITTERM":ESC("[H")+ESC("[2J"),
    "ENDTERM":"", "FRTOP" : ESC("[2;25r"), "FRBOTTOM" : ESC("[1;24r"),
    "UNFR" : ESC("[r"), "SAVEC" : ESC("[s"), "RESTC" : ESC("[u"),
    "HOMEC" : ESC("[H"), "CLEAR" : ESC("[2J"), "ER_SOL" : ESC("[1k"),
    "ER_EOL" : ESC("[K"), "ER_LINE" : ESC("[2K"), "ER_DOWN" : ESC("[J"),
    "ER_UP" : ESC("[1J")*/
    ]);
    words = explode(msg + "%^", "%^");
    size = sizeof(words);

    if(flag)
    {
	while(size--) if(data[words[size]]) words[size] = "";
    }
    else while(size--) if(data[words[size]]) words[size] = data[words[size]];


    msg = implode(words, "");

    return msg;
}

