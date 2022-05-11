#ifndef _ONBOARD_DOTSTAR_H
#define _ONBOARD_DOTSTAR_H
typedef enum _DotstarColours 
{  
	DOT_BLACK,
        DOT_BLUE,
        DOT_CYAN,
        DOT_GREEN,
        DOT_YELLOW,
        DOT_RED,
        DOT_MAGENTA,
        DOT_WHITE
} eDotstarColours;

void doststar_show(int colour);
void dotstar_init(void);
#endif
