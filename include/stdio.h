/* stdio.h - definintions and constants for standard I/O functions */


/* Prototypes for formatted input functions */

extern	int	_doscan(char *,int *, int (*)(void),
			int (*)(char), int, int);
extern	int	sscanf(char *, char *, int);
extern	int	fscanf(int, char *, int);
#define	scanf(fmt, args)	fscanf(CONSOLE, fmt, args)


/* Definintion of standard input/ouput/error used with shell commands */

#define	stdin	((proctab[currpid]).prdesc[0])
#define	stdout	((proctab[currpid]).prdesc[1])
#define	stderr	((proctab[currpid]).prdesc[2])


/* Prototypes for formatted output functions */

extern	int	fprintf(int, char *, ...);
extern	int	printf(const char *, ...);
extern	int	sprintf(char *, char *, ...);


/* Prototypes for character input and output functions */

extern	int	fgetc(int);
extern	char	*fgets(char *, int, int);
extern	int	fputc(int, int);
extern	int	fputs(char *, int);
extern	int	putchar(int c);
extern	int 	getchar(void);
