
#ifndef WINPROF_H

#define WINPROF_H

/* PROFILE struct */

typedef struct {
    char dbname[128];        /* database name */
    int change;              /* change flag */
    int dbsz;                /* data base size, bytes */
    unsigned msz;            /* memory size, bytes */
    char *mp;                /* pointer to memory area */
} PROFILE;


/* error code flag */

extern int dberror;


/* Used to remove arguments in function prototypes for non-ANSI C */

#if defined(dos) || defined(__STDC__)
#  define OF(a) a
#else /* !dos */
#  define OF(a) ()
#endif /* ?dos */

#ifndef INT_MAX
#  define INT_MAX 32767
#endif

#ifndef MAX
#  define MAXINT INT_MAX
#endif

/* function prototype statements */

PROFILE *dbinit   OF((char *));
int      dbgetstr OF((PROFILE *, char *, char *, char *, char *, int));
int      dbgetint OF((PROFILE *, char *, char *, int));
int      dbwrite  OF((PROFILE *, char *, char *, char *));
int      dbterm   OF((PROFILE *));

#endif

