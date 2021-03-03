/*
	"monlib.h"
*/
#ifndef TRUE
#define TRUE    (1==1)
#define FALSE   (!TRUE)
#endif
#ifndef ON
#define ON      TRUE
#define OFF     FALSE
#endif

/*------ Function prototypes ---------------------------------------------------*/
int IsComment(char *s);
int IsBlank(char *s);
int IsPsc(char *s);
int GetArgs(char *str, char *argv[]);
int AlmostSame(char *token, char *refstr);
char *Rejectnl(char *s);
char *RejectComment(char *s);
char *RejectSpace(char *s);
char *Gettoken(char *s, char *dlmn);
void Printbin(int d, int n);


