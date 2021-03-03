/*
    "consio.c"
    raw terminal interface
    1986/4/27
    1986/7/14 for OS9-68k
    1993/3/8	for NEWS
    2009/     for Linux
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
//#include <ulimit.h>
#include <sys/types.h>
#include <sys/time.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <curses.h>
#include <term.h>

#include "consio.h"

int LPP = 24;
int CPL = 80;

char inchr(void)
{
    char c;

    get_wsize();
    c = getchar();
    if (c == EOLKEY)
        c = EOL;

    return c;
}

void cls(void)
{
    fputs("\033[2J", stdout);
}

void el(void)
{
    fputs("\033[0K", stdout);
}

void ovfmark(void)
{
    printf("\033[7m>\033[0m");
}

void point(int x, int y)
{
    printf("\033[%d;%dH", y + 1, x + 1);
}

int chkrdy(void)
{				/* terminal ready : 0     */
    fd_set readfds;
    struct timeval tv;

    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO(&readfds);
    FD_SET(0, &readfds);
    if (select(1, &readfds, NULL, NULL, &tv) != 0) {
//        if (select(ulimit(4, 0), &readfds, NULL, NULL, &tv) != 0) {
       return !(FD_ISSET(0, &readfds));
    } else
        return 1;
}

#define STDIN	0
#define STDOUT	1

static struct termios ttystat;

void initerm(void)
{
    struct termios buff;

    tcgetattr(STDIN, &ttystat);
    tcgetattr(STDIN, &buff);

    buff.c_lflag &= ~ISIG;	/* no kbd interrupt     */
    buff.c_lflag &= ~ECHO;	/* no echo      */
    buff.c_lflag &= ~ICANON;	/* raw mode (no buffered)       */

    buff.c_iflag &= ~IXON;	/* no x-on      */
    buff.c_iflag &= ~IXOFF;	/* no x-off     */

    buff.c_cc[VMIN] = 1;
    buff.c_cc[VTIME] = 0;

    tcsetattr(STDIN, TCSADRAIN, &buff);

    get_wsize();
}

int get_wsize(void)
{
    register char *s;
    int sc_height, sc_width;
#ifdef TIOCGWINSZ
    struct winsize w;
#else
#ifdef WIOCGETD
    struct uwdata w;
#endif
#endif

#ifdef TIOCGWINSZ
    if (ioctl(2, TIOCGWINSZ, &w) == 0 && w.ws_row > 0)
        sc_height = w.ws_row;
    else
#else
#ifdef WIOCGETD
        if (ioctl(2, WIOCGETD, &w) == 0 && w.uw_height > 0)
            sc_height = w.uw_height / w.uw_vs;
        else
#endif
#endif
            if ((s = getenv("LINES")) != NULL)
                sc_height = atoi(s);
            else
                sc_height = tgetnum("li");

    if (sc_height <= 0)
        sc_height = 24;

#ifdef TIOCGWINSZ
    if (ioctl(2, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        sc_width = w.ws_col;
    else
#ifdef WIOCGETD
        if (ioctl(2, WIOCGETD, &w) == 0 && w.uw_width > 0)
            sc_width = w.uw_width / w.uw_hs;
        else
#endif
#endif
            if ((s = getenv("COLUMNS")) != NULL)
                sc_width = atoi(s);
            else
                sc_width = tgetnum("co");

    if (sc_width <= 0)
        sc_width = 80;

    if ((LPP == sc_height) && (CPL == sc_width - 1))
        return 0;
    else {
        LPP = sc_height;
        CPL = sc_width - 1;
        if ((LPP < 2) || (CPL < 2)) {
            LPP = 24;
            CPL = 80;
        }
        return 1;
    }
}

void deinitrm(void)
{
    tcsetattr(STDIN, TCSANOW, &ttystat);
}

int esc_seq(char c)
{
    int d = 0, rv = c;
    if(c == '\033'){    // ESC
        c = inchr();
        if(c == '['){   // CSI
            c = inchr();
            if(isdigit(c)){
                d = (c - '0');
                c = inchr();
                if(isdigit(c)){
                    d = d*10 + (c - '0');
                    c = inchr();
                }
                switch(d){
                case 1: rv = K_Home; break;
                case 2: rv = K_Insert; break;
                case 3: rv = K_Delete; break;
                case 4: rv = K_End; break;
                case 5: rv = K_PgUp; break;
                case 6: rv = K_PgDn; break;
                default: break;
                }
            }else{
                switch(c){
                case 'A': rv = K_Up; break;
                case 'B': rv = K_Down; break;
                case 'C': rv = K_Right; break;
                case 'D': rv = K_Left; break;
                default: break;
                }
            }
        }
    }
    return rv;
}



