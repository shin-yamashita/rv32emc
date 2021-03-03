/*
	"consio.h"
*/

#include <curses.h>

#define BSP 0x7f    /* back space code  */
#define DEL 0x08    /* delete char code */
#define EOL '\n'    /* end of line char */
#define EOLKEY 0xd

#define CLS             "\033[2J"
#define ELEOL           "\033[0K"

#define ATR_OFF         "\033[0m"
#define ATR_BOARD       "\033[1m"
#define ATR_UNDER_LINE  "\033[4m"
#define ATR_UNDER_BOARD "\033[1;4m"
#define ATR_REVERSE     "\033[7m"

extern int LPP; // line per page
extern int CPL; // colmn per line

enum KeySeq {K_Home, K_Insert, K_Delete, K_End, K_PgUp, K_PgDn, K_Up, K_Down, K_Right, K_Left, };

char inchr(void);
void cls(void);
void el(void);
void ovfmark(void);
void point(int x, int y);
int chkrdy(void);
void initerm(void);
int get_wsize(void);
void deinitrm(void);
int esc_seq(char c);

