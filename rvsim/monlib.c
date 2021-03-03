/*
	"monlib.c"
	mon utility functions.

*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "monlib.h"

/*----------------------------------------------------------------------*/

int IsComment(char *s)
{				/* Is Comment line ?      */
    return ((*s == '#') || (*s == '*') || IsBlank(s));
}

int IsBlank(char *s)
{				/* Is blank line ?        */
    while (isspace(*s) && *s)
	s++;
    return *s == 0;
}

int IsPsc(char *s)
{				/* Is Pseudo operation ?  */
    while (isspace(*s) && *s)
	s++;
    return *s == '.';
}

char *Rejectnl(char *s)
{				/* Reject new line character      */
    char *s1;

    s1 = s;
    while (*s)
	if (*s == '\n')
	    *s = '\0';
	else
	    s++;
    return s1;
}

char *RejectComment(char *s)
{				/* Reject Comment */
    char *s1;

    s1 = Rejectnl(s);
    while (*s)
	if (*s == '#') {
	    *s = '\0';
	    break;
	} else
	    s++;
    return s1;
}

char *RejectSpace(char *s)
{				/* Reject space character */
    char *s1, *s2, c;

    s2 = s1 = s;
    while ((c = *s++)) {
	if (!isspace(c)) {
	    *s1++ = c;
	}
    }
    *s1 = '\0';

    return s2;
}

void Printbin(int d, int n)
{
    int i;

    for (i = n - 1; i >= 0; i--) {
	printf("%d", (d >> i) & 1);
    }
}

/*
 * GetArgs() separates arguments from string "str[]", 
 *   and returns pointer to args "*argv[]" and no. of args "argc".
 *    Each args are separated with character ' ', ',' or ';'.
 */

int GetArgs(char *str, char *argv[])
{
    int argc;

    Gettoken(str, " \t\n;");

    argc = 0;
    while ((argv[argc] = Gettoken(NULL, " \t\n;")) != NULL)
	argc++;

    return argc;
}

static char *esc_chr = "abtnvfr\"\\";
static char *esc_cnv = "\a\b\t\n\v\f\r\"\\";

char *Gettoken(char *s, char *dlmn)
{
    static char *str;
    char *lstr, *s1, *s2, *s3;

    if (s != NULL) {
	str = s;
	lstr = s;
    } else {
	s1 = str;
//              while((isspace(*s1) ||/* (*s1 == ',') ||*/ (*s1 == ';')) && *s1)
	while (*s1 && strchr(dlmn, *s1))
	    s1++;

	if (*s1 == '\0')
	    return NULL;

	str = s1;
	if (*s1 == '"') {
	    str = s2 = ++s1;
	    while (*s1) {
		if (*s1 == '\\') {	// escape char conversion
		    if (*++s1 == '\0')
			break;
		    if ((s3 = strchr(esc_chr, *s1))) {
			*s2++ = esc_cnv[s3 - esc_chr];
		    }
		    if (*++s1 == '\0')
			break;
		} else {
		    if (*s1 == '"')
			break;
		    *s2++ = *s1++;
		}
	    }
	    *s2 = '\0';
	} else {
//                      while(!isspace(*s1) && *s1 
//                      && /*(*s1 != ',') && */ (*s1 != ';'))
	    while (*s1 && !strchr(dlmn, *s1))
		s1++;
	}
	lstr = str;
	if (*s1)
	    str = s1 + 1;
	else
	    str = s1;

	*s1 = '\0';
    }

    return lstr;
}


/*
 * AlmostSame() compares string value of token[] with refstr[], and
 *   returns TRUE if they are identical up to the first $ in refstr[].
 */

int AlmostSame(char *token, char *refstr)
{
    int almost = FALSE;

    while (*token) {
	if (*refstr == '$') {
	    almost = TRUE;
	} else if (*token != *refstr) {
	    return FALSE;
	} else
	    token++;
	refstr++;
    }

    return almost || (*refstr == '$') || (*refstr == '\0');
}
