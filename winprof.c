/*
            Title: Windows-like profile Facility
           Author: Jack Botner, BOTNER at TOROLAB2
                   Deluan Perez, DELUAN at RIO40Graus
          Version: 2.00
     Date Written: Feb. 26, 1988
     Last Updated: July 1, 1993, the Now-or-never Winter!
       Processors: Borland C++ 3.1 (#define dos), Unix SysV
   Classification: Internal Use Only
        Reference: Microsoft Windows Software Development Kit Programmer's
                   Reference, section 4.8: Windows Initialization File
                   Functions

These functions implement a simple database system in the form of a flat
file the same as that available in Microsoft Windows for their WIN.INI
file. The file is characterized as follows:

1. Comment lines
   These are any line with a semicolon in column 1.

2. Major sections
   This is a section preceeded by a line containing the major key in
   square brackets, i.e. "[ports]". It must begin in column 1.

3. Detail entries
   This is one or more lines after a major section line, characterized
   by a minor key, an equal sign, and an optional value, i.e.
   "com1:=1200,n,8,1". The value before the equal sign is the minor
   key. The value after the equal sign is optional and may be anything
   up to the end of the line.

Functions are provided to add/modify/delete and access the database.
There are no functions to access comment lines, but since the file is a
plain ascii flat file, it may be edited and modified by programs.

function synopsis:

  rc = dbinit( database-name );
  int rc;
  char *database-name;

    This function must be called before any other database function.
    database-name must contain the fully-qualified filename of the
    database. 0 is returned if OK, otherwise an error occurred (see
    below for return codes). Only one database may be initialized at a
    time. To access another database, use the dbterm function on the
    current database and dbinit on the new database.

    Note that the maximum size database file is around 30K.

                        -*-*-*-*-*-*-*-

  rc = dbgetstr( appname, keyname, defname, retstr, nsize )
  int rc;
  char *appname,
       *keyname,
       *defname,
       *retstr;
  int nsize;

    This function accesses a record in the database and returns the
    string assigned to the key. Pass the application name in appname
    and the key name in keyname. If the string is not found, the
    default value found in defname will be returned. If not used,
    defname should be a zero-length string. The returned string is
    moved to the address pointed to by retstr, to a maximum length
    specified by nsize. The function return value is the length of
    the string returned, otherwise 0.

    A list of all the keynames in the application may be obtained by
    passing a NULL pointer instead of keyname. In this case, all the
    keynames associated with the application name are enumerated and
    the returned string filled with the keys. Each key is null
    terminated, and a second null is placed at the end of the list.
    The return value is the length of the list up to, but not
    including, the final null.

                        -*-*-*-*-*-*-*-

  rc = dbgetint( appname, keyname, defvalue )
  int rc;
  char *appname,
       *keyname;
  int  defvalue;

    This function accesses a record in the database and returns the
    integer assigned to the key. Pass the application name in appname
    and the key name in keyname. If the string is not found, the
    default value found in defvalue will be returned.

                        -*-*-*-*-*-*-*-

  rc = dbwrite( appname, keyname, data-string )
  int rc;
  char *appname,
       *keyname,
       *data-string;

    This function inserts, replaces, or deletes a key record from the
    database. It also creates an application record if the application
    name is not found. Pass the application name in appname, and the record
    key in keyname. The data assigned to the key record should be passed
    in data-string. If data-string is a zero length string, the key record will
    be deleted, otherwise it will be inserted or replaced. 0 is returned if
    OK, otherwise an error occurred (see below for return codes).

                        -*-*-*-*-*-*-*-

  rc = dbterm()
  int rc;

    This function must be called after all database accesses are finished
    and before the program ends. The database is held in main storage and
    must be written to disk to be saved. 0 is returned if OK, otherwise an
    error occurred (see below for return codes).

                        -*-*-*-*-*-*-*-

Change history:

  01/19/95  Drs Perez & @riva's OBS1: When the file does not exists, we'll have
            a problem.
            Johnny:   What's the problem, Drs?
            Drs:      Well, Johnny, you know...
            Johnny:   And what will you do, Drs? - Johnny's scared.
            Solution: Well, Johnny, when the file doesn't exist, we create
                      a file with a blank line in it. Sometime in somewhere
                      we'll analize this problem a little more....

  06/26/94  Perez: findkey() now ignores spaces before '='.
            dbgetint() now sets dberror with -1 when key is not found.

  01/28/94  Deluan Dodo: Bug corrected in dbterm(). It now checks for a NULL
            profile.

  11/21/93  Deluan Perez: Bug corrected in dbinit().

  11/02/93  Deluan Perez: metacaracteres nas strings retornadas por dbgetstr
            sao processadas (i.e.: \r, \n, \x1b, ...)

  10/02/93  Deluan Perez: keylen would not find CR at last line.
            Now it also stops when it finds the EOD char (^Z).

  08/18/93  Deluan Perez: Bug corrected in strnicmp (tsk).

  07/01/93  Deluan Perez: Implemented multiple profile processing.
            Error codes from dbinit() are returned in the new dberror
            variable.

  04/29/93  Deluan Perez: Implemented my own strnicmp function,
            'cause Unix don't have such thing (not standard yet).

  02/18/93  Deluan Perez: Adapted to Unix. mymemcpy also needed.
            All '\r' was changed to '\n' (Unix newline char).

  04/28/88  Implemented my own memcpy function after dicovering that
            Microsoft C memcpy does not work for overlapping fields.

  04/20/88  Forgot to free memory after closing the database!

  04/19/88  Changed dbwrite to NOT delete the entry when the data_string
            value is NULL. This is more compatible with Windows, which never
            deletes a line.

  04/19/88  Error code 201 from dbwrite occurred when there was an attempt
            to update an entry where the keyname existed but the value in
            the database was null. deldata was fixed to remove the 201
            error and do nothing if the delete length is zero.

  04/14/88  WINPROF would not handle profiles with ^Z at the end. It would
            create new entries after the ^Z. dbinit was fixed to handle
            the ^Z correctly.

*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <values.h>

#include "winprof.h"            /* function prototype statements */

#ifdef dos
#  include <stdlib.h>
#else
char *malloc OF((int));
#endif

#ifndef size_t
#  define size_t int
#endif

#define MAX_STRING_LENGTH 256
#define EOD               26

#ifdef dos
#  define CR    '\r'
#else
#  define CR    '\n'
#endif


int dberror;

static unsigned msz;            /* memory size, bytes */
static char *mp = 0;            /* pointer to memory area */

static int  findapp   OF((char * ));
static int  findkey   OF((char *, char *));
static int  isrtdata  OF((PROFILE *, int, char *, int));
static int  deldata   OF((PROFILE *, int, int));
static void keylen    OF((char *, int *, int *));
static int  nextrec   OF((int));
static void mymemcpy  OF((char *, char *, int));
static void metaparse OF((char *s));

#ifndef dos

static int strnicmp(s1, s2, maxlen)
char *s1, *s2;
size_t maxlen;
{
        size_t i, ret = 0;

        for (i = 0; i < maxlen && *s1 && *s2 && !ret; i++, s1++, s2++)
            ret = toupper(*s1) - toupper(*s2);

        return(ret);
}

#endif

/*------------------------------------------------
dbinit: initialize the database

returns: 0 if OK
        -1 if invalid input parameter
        -2 if database already open
        -3 file could not be created
        -4 i/o error
        -5 if file too large
        -6 if not enough memory
        -7 read error
------------------------------------------------*/

PROFILE *dbinit( db_name )
  char *db_name;                /* fully-qualified file name */
{
  FILE *fp;
  char *wp, *tp;
  int  nn;
  long fsz;
  PROFILE *p;

  dberror = 0;
  if ((p = (PROFILE*)malloc(sizeof(PROFILE))) == NULL) { dberror = -6; return NULL; }
  /* if ( mp != (char *) 0 ) { dberror = -2; goto error; } */
  if ( !strlen( db_name ) ) { dberror = -1; goto error; }
  /* if ( strlen( dbname ) ) { dberror = -2; goto error; } */

  wp = db_name;
  fp = fopen( wp, "rb" );               /* open file for input */

  /* If file does not exist, create it */
  if ( fp == (FILE *) 0 )               /* file not found */
  {
    fp = fopen( wp, "wb" );
    if ( fp == (FILE *) 0 ) { dberror = -3; goto error; }
    fputs("\n", fp); /*PP - OBS: Vide OBS1:*/
    if ( fclose( fp ) == -1 ) { dberror = -3; goto error; }
    fp = fopen( wp, "rb" );             /* open file for input */
    if ( fp == (FILE *) 0 ) { dberror = -3; goto error; }
  }

  /* Get the file size */

  if (fseek( fp, 0L, 2 ) == -1 ) {     /* seek to end of file */
     dberror = -3;
     goto error;
  }
  fsz = ftell( fp );                    /* get file size */
  if ( fsz == -1L ) {
     dberror = -4;
     goto error;
  }
  if ( fsz > (long) ( MAXINT - ( MAXINT / 6 ) ) ) {
    fclose( fp );
    dberror = -5;
    goto error;
  }
  (void) fseek( fp, 0L, 0 );            /* seek to beginning */

  /* Get main storage to hold the file */

  msz = (unsigned) fsz + ( (unsigned) fsz / 6 );
  if ( msz < 4096 ) msz = 4096;
  mp = malloc( msz );
  if ( mp == (char *) 0 )
  {
    if ( fp != (FILE *) 0 ) fclose( fp );
    dberror = -6;
    goto error;
  }

  /* Read the file in */

  if ( fsz )
  {
    nn = fread( mp, (unsigned) fsz, 1, fp );
    fclose( fp );
    if ( nn != 1 )
    {
      free( mp );
      mp = (char *) 0;
      dberror = -7;
      goto error;
    }
  }

  /* Mark the end of file with a ^Z */

  if ( fsz )                    /* data exists */
  {
    tp = mp + (int) (fsz - 1L);     /* point to last byte */
    if ( *tp == EOD )           /* EOD already there */
      --fsz;                    /* remove it from length */
    else                        /* no EOD */
    {
      ++tp;                     /* insert EOD at end */
      *tp = EOD;
    }
  }
  else                          /* no data exists */
    *mp = EOD;

  strcpy( p->dbname, wp );         /* save the file name */
  p->dbsz = (int) fsz;
  p->change = 0;
  p->mp = mp;
  p->msz = msz;
  return p;

error:
  free(p);
  return NULL;
}

/*------------------------------------------------
dbgetstr: get a profile string

returns: length of string returned
         (0 if not found)
------------------------------------------------*/

int dbgetstr(p, appname, keyname, defname, retstr, nsize)
  PROFILE *p;
  char *appname,                /* pointer to application name string */
       *keyname,                /* pointer to key name string */
       *defname,                /* pointer to default string */
       *retstr;                 /* pointer to buffer for result */
  int nsize;                    /* maximum length of result string */
{
  int  nn, rc, l1, l2;
  char *wp, *tp;

  mp = p->mp;
  msz = p->msz;

  if ( mp == (char *) 0 ) return 0;

  *retstr = *(retstr+1) = '\0';

  if ( !p->dbsz ) goto notfound;           /* no data */
  if ( !strlen( appname ) || !nsize ) goto notfound;
  if ( keyname != (char *) 0 ) if ( !strlen( keyname ) ) goto notfound;
  rc = findapp( appname );
  if ( rc == -1 ) goto notfound;        /* no application entry */

  if ( keyname != (char *) 0 )          /* find specific key */
  {
    rc = findkey( (mp+rc), keyname );
    if ( rc == -1 ) goto notfound;      /* no key entry */

    wp = mp + rc;                       /* point to key record */
    keylen( wp, &l1, &l2 );             /* get key, data lengths */

    tp = wp + ( l1 + 1 );               /* bypass key */
    rc = 0;
    if ( l2 )
    {
      rc = ( l2 > nsize ) ? nsize : l2;
      (void) mymemcpy( retstr, tp, rc );
      retstr[rc] = '\0';
    }
  }
  else                                  /* enumerate all keys */
  {
    nn = rc;                            /* current offset in database */
    tp = retstr;                        /* pointer to caller's return area */
    rc = 0;                             /* length of returned data */

    while ( 1 )
    {
      nn = nextrec( nn );
      if ( nn == -1 ) break;            /* end of database */
      wp = mp + nn;                     /* point to record */
      if ( *wp == ';' ) continue;       /* bypass comment */
      if ( *wp == '[' ) break;          /* stop at next application */
      if ( *wp == ' ' ) continue;       /* skip blank lines */
      keylen( wp, &l1, &l2 );           /* get key lengths */
      if ( !l1 ) continue;              /* skip incomplete record */
      if ( ( rc + l1 + 2 ) >= nsize )   /* key name too long to return */
        continue;                       /* next one may be smaller! */
      (void) mymemcpy( tp, wp, l1 );      /* return the string */
      tp += l1;
      *tp = (char) '\0';                /* null terminated */
      ++tp;
      *tp = (char) '\0';                /* in case this is the last */
      rc += ( l1 + 1 );                 /* update length returned */
    }
  }
  goto endup;

notfound:                               /* no string */
  nn = strlen( defname );
  if ( !nn ) return 0;                  /* no default */
  rc = ( nn > nsize ) ? nsize : nn;
  (void) mymemcpy( retstr, defname, rc );
  retstr[rc] = '\0';

endup:
  metaparse(retstr);
  return rc;
}

/*------------------------------------------------
dbgetint: get a profile integer

returns: value of integer
         default value if not found
------------------------------------------------*/

int dbgetint(p, appname, keyname, defvalue)
  PROFILE *p;
  char *appname,                /* pointer to application name string */
       *keyname;                /* pointer to key name string */
  int  defvalue;                /* default value */
{
  int  nn, rc, l1, l2;
  char *wp, *tp, workarea[MAX_STRING_LENGTH];

  mp = p->mp;
  msz = p->msz;

  dberror = -1;

  if ( mp == (char *) 0 ) return defvalue;

  if ( !p->dbsz ) return defvalue;         /* no data */
  if ( !strlen( appname ) || !strlen( keyname ) ) return defvalue;

  rc = findapp( appname );
  if ( rc == -1 ) return defvalue;      /* no application entry */

  rc = findkey( (mp+rc), keyname );
  if ( rc == -1 ) return defvalue;      /* no key entry */

  wp = mp + rc;                         /* point to key record */
  keylen( wp, &l1, &l2 );               /* get key, data lengths */
  if ( !l2 ) return defvalue;           /* no data */

  tp = wp + ( l1 + 1 );                 /* point to data area in record */
  (void) mymemcpy( workarea, tp, l2 );    /* copy to workarea */
  workarea[l2] = '\0';                  /* make a string */

  if ( sscanf( workarea, "%d", &nn ) != 1 ) return defvalue;

  dberror = 0;
  return nn;
}

/*------------------------------------------------
dbwrite: insert/update a profile string

returns:     0 = OK
         other = error
------------------------------------------------*/

int dbwrite(p, appname, keyname, string)
  PROFILE *p;
  char *appname,                /* pointer to application name string */
       *keyname,                /* pointer to key name string */
       *string;                 /* pointer to string data */
{
  int  nn, rc, l1, l2, isrtpt, dele;
  char *wp;
  char data[MAX_STRING_LENGTH+1], temp[MAX_STRING_LENGTH+4];

  mp = p->mp;
  msz = p->msz;

  if ( mp == (char *) 0 ) return 0;

  if ( !strlen( appname ) || !strlen( keyname ) ) return 0;
  if (!(dele = (string == NULL)))
     strncpy( data, string, MAX_STRING_LENGTH );
  else data[0] = 0;

  rc = findapp( appname );
  if ( rc == -1 ) goto noapp;           /* no application entry */
  isrtpt = rc;

  rc = findkey( (mp+rc), keyname );
  if ( rc == -1 )                       /* no key entry */
  {
 /* if ( !strlen( data ) ) return 0; */ /* nothing to do */
    wp = mp + isrtpt;                   /* point to application record */
    while ( *wp != CR )               /* scan for end of app record */
    {
      ++wp;
      ++isrtpt;
    }
    while ( *wp == '\r' || *wp == '\n' ) /* scan for start of next record */
    {
      ++wp;
      ++isrtpt;                         /* get proper insert point */
    }
    goto nokey;
  }

  /* The application and key entries exist. Do an update. */

  wp = mp + rc;                         /* point to key record */
  keylen( wp, &l1, &l2 );               /* get key, data lengths */

  if ( strlen( data ) )                 /* replace data */
  {
    nn = rc + l1 + 1;                   /* where current data is */
    rc = deldata(p, nn, l2);            /* delete current data */
    if ( rc ) return rc;                /* error */
    rc = isrtdata(p, nn, data, strlen(data));
  }
  else                                  /* delete data */
  {
    if (dele) {
       nn = l1 + l2 + 3;                /* length of current data */
       rc = deldata(p, rc, nn);         /* delete current data */
    }
    rc = deldata(p, (rc + l1 + 1), l2); /* delete the value only */
  }
  goto end;

noapp:
#ifdef dos
  strcpy( temp, "\r\n[" );
  strcat( temp, appname );
  strcat( temp, "]\r\n" );
#else
  strcpy( temp, "\n[" );
  strcat( temp, appname );
  strcat( temp, "]\n" );
#endif
  rc = isrtdata(p, p->dbsz, temp, strlen(temp));
  if ( rc ) goto end;                   /* insert error */
  isrtpt = p->dbsz;

nokey:
  strcpy( temp, keyname );
  strcat( temp, "=" );
  strcat( temp, data );
#ifdef dos
  strcat( temp, "\r\n" );
#else
  strcat( temp, "\n" );
#endif
  rc = isrtdata(p, isrtpt, temp, strlen(temp));

end:
  if ( !rc ) p->change = 1;
  return rc;
}

/*------------------------------------------------
dbterm: terminate the database

returns: 0 if OK
        -1 if not initialized
        -2 if database does not exist
        -3 error opening file
        -4 error writing file
        -5 error closing file
------------------------------------------------*/

int dbterm(p)
  PROFILE *p;
{
  FILE *fp;
  int  nn, rc;

  if (p == NULL) return -1;

  if ((mp = p->mp) == NULL) return -1;

  if (!p->change)                        /* no change */
  {
    rc = 0;
    goto cleanup;
  }
  if ( !strlen(p->dbname))
  {
    rc = -1;
    goto cleanup;
  }
  fp = fopen(p->dbname, "rb");           /* open file for input */
  if ( fp == (FILE *) 0 )               /* file not found */
  {
    rc = -2;
    goto cleanup;
  }
  fclose( fp );

  fp = fopen(p->dbname, "wb");
  if ( fp == (FILE *) 0 )
  {
    rc = -3;
    goto cleanup;
  }
  nn = (p->dbsz) ? fwrite(p->mp, p->dbsz, 1, fp ) : 1;
  rc = fclose( fp );
  if ( nn != 1 )
  {
    rc = -4;
    goto cleanup;
  }
  if ( rc == -1 ) rc = -5;

cleanup:
  free(p->mp);                           /* release the memory */
  p->mp = NULL;
  p->dbname[0] = '\0';
  p->change = 0;
  free(p);

  return rc;
}

/*------------------------------------------------
findapp: scan the database for the application

returns: offset to record containing application
         (-1 if not found)
------------------------------------------------*/

static int findapp( appname )
  char *appname;                /* pointer to application name string */
{
  char *wp, *tp;
  int  nn, rc;

  wp = mp;
  while ( *wp != EOD )
  {
    if ( *wp == '[' )           /* record containing application name */
    {
      tp = wp + 1;
      nn = 0;                   /* get the length of the name */
      while ( *tp != ']' && *tp != CR )
      {
        ++nn;
        ++tp;
      }
      if ( nn == strlen( appname ) && !strnicmp( (wp+1), appname, nn ) )
      {
        rc = wp - mp;
        return rc;
      }
    }
    while( *wp != '\r' && *wp != '\n' ) ++wp;   /* find end of record */
    while( *wp == '\r' || *wp == '\n' ) ++wp;   /* find next record */
  }
  return -1;                    /* not found */
}

/*------------------------------------------------
findkey: scan the database for the key

returns: offset to record containing key
         (-1 if not found)
------------------------------------------------*/

static int findkey(ap, keyname)
char *ap,                     /* pointer to application entry */
     *keyname;                /* pointer to key name string */

{ char *wp, *tp;
  int  nn, rc, kl;

  if (*ap != '[') return 0;   /* not positioned correctly */
  wp = ap;
  while(*wp != '\r' && *wp != '\n') ++wp;   /* find end of record */
  while(*wp == '\r' || *wp == '\n') ++wp;   /* find next record */

  while (*wp != '[' && *wp != EOD) {   /* stop at next application or EOD */
        if (*wp != ' ' && *wp != ';') {    /* record containing key name */
           tp = wp;
           nn = 0;                   /* get the length of the key */
           while (*tp != '=' && *tp != CR) {
                 ++nn;
                 ++tp;
           }
           kl = strlen(keyname);
           if (/*nn == kl && */!strnicmp(wp, keyname, kl)) {
              rc = wp - mp;
              return rc;
          }
        }
        while(*wp != '\r' && *wp != '\n') ++wp;   /* find end of record */
        while(*wp == '\r' || *wp == '\n') ++wp;   /* find next record */
  }
  return -1;                    /* not found */
}

/*------------------------------------------------
isrtdata: insert some data into the database

returns:   0 = OK
        -301 = input parameter error
        -302 = input parameter error
        -303 = dbterm error
        -304 = dbinit error
------------------------------------------------*/

static int isrtdata(p, where, what, howlong)
  PROFILE *p;
  int where;                    /* offset to location to insert */
  char *what;                   /* pointer to data to insert */
  int howlong;                  /* amount of data to insert */
{
  char *wp, *tp;
  int  nn;

  if ( !howlong ) return -101;             /* programming error */
  if ( where > p->dbsz ) return -102;      /* programming error */

  if ( howlong > ( (int) p->msz - p->dbsz ) ) /* not enough space */
  {
    char temp[128];
    strcpy(temp, p->dbname);
    if (dbterm(p)) return -103;         /* error closing database */
    if ((p = dbinit(temp)) == NULL) return -104;  /* error opening database */
    mp = p->mp;
    msz = p->msz;
  }

  if ( where == howlong )               /* nothing to do */
  {
    p->dbsz += howlong;
    return 0;
  }

  /* Make room for the inserted data */

  wp = p->mp + where;                      /* insert point */
  nn = (p->dbsz + 1) - where;            /* amount to copy */
  tp = wp + howlong;                    /* where to put it */
  (void) mymemcpy( tp, wp, nn );

  (void) mymemcpy( wp, what, howlong );   /* insert the data */
  p->dbsz += howlong;
  return 0;
}

/*------------------------------------------------
deldata: delete some data from the database

returns:   0 = OK
        -202 = parameter error
        -203 = parameter error
------------------------------------------------*/

static int deldata(p, where, howlong)
  PROFILE *p;
  int where;                    /* offset to location to delete */
  int howlong;                  /* amount of data to delete */
{
  char *wp, *tp;
  int  nn;

  if ( !howlong ) return 0;             /* nothing to do */
  if ( where >= p->dbsz ) return -202;     /* programming error */
  if ( howlong > ( p->dbsz - where) ) return -203;

  wp = p->mp + where;                      /* where to delete the data */
  tp = wp + howlong;                    /* where the next data is */
  nn = ( p->dbsz + 1 ) - ( where + howlong );  /* how much data to move */

  (void) mymemcpy( wp, tp, nn );          /* delete the data */
  p->dbsz -= howlong;

  mp = p->mp;
  msz = p->msz;
  return 0;
}

/*------------------------------------------------
keylen: return 2 lengths associated with key record

returns: l1 = length of key (up to =)
         l2 = length of data ( after =)
------------------------------------------------*/

static void keylen( record, l1, l2 )
  char *record;                 /* pointer to key record */
  int  *l1, *l2;                /* pointers to return lengths */
{
  char *wp;
  int  c1, c2;

  c1 = c2 = 0;
  wp = record;

  /* Add up length of key before the = */

  while ( *wp != '=' && *wp != CR && *wp != EOD )   /* scan for = or c/r */
  {
    ++c1;
    ++wp;
  }

  if ( *wp != '=' )                     /* no data */
  {
    *l1 = c1;
    *l2 = c2;
    return;
  }

  /* Add up length of data after the = */

  ++wp;                                 /* bypass the = */
  while ( *wp != CR && *wp != EOD )     /* scan for c/r */
  {
    ++c2;
    ++wp;
  }

  *l1 = c1;
  *l2 = c2;
}

/*------------------------------------------------
nextrec: return offset to next record in database.

returns: offset to next record, or
         -1 if no record found.
------------------------------------------------*/

static int nextrec( offset )
  int  offset;                  /* offset to current record */
{
  char *wp;
  int  rc;

  wp = mp + offset;             /* point to current record */

  if ( *wp == EOD )
  {
    rc = -1;
    goto endup;
  }

  while( *wp != '\r' && *wp != '\n' ) ++wp;     /* find end of record */
  while( *wp == '\r' || *wp == '\n' ) ++wp;     /* find next record */

  if ( *wp == EOD )
  {
    rc = -1;
    goto endup;
  }

  rc = wp - mp;

endup:
  return rc;
}

/*-----------------------------------------------------------------
   This program implements the memcpy library function so that
   overlapping fields are handled correctly. This is not required
   for Lattice C, since its library function works correctly. It
   is required for Microsoft C, since its library function does
   not work correctly and messes up your data.
-----------------------------------------------------------------*/

static void mymemcpy( target, source, count )
  char *target;
  char *source;
  int  count;
{
  int  ii;

  if ( target == source ) return;
  if ( target > source )
  {
    target += count;
    source += count;
    for ( ii=0; ii<count; ++ii )
    {
      --source;
      --target;
      *target = *source;
    }
  }
  else
    for ( ii=0; ii<count; ++ii ) *target++ = *source++;

}

/*
 *  metaparse()
 *
 *  Funcao: Interpreta strings "C", convertendo caracteres de controle.
 *          ex: \r = CR, \n = LF, \\ = \, \x1b = ESC, ...
 *
 *  Desenvolvida por Deluan Perez para o Little "C" em Out/1992.
 *  Ampliada no "dia das bruxas" de 1993.
 */
static void metaparse(s)
char *s;
{
     char *t;
     char mod;

     for (t = s; *t; t++)
	 if (*t == '\\') {
	    t++;

            /* Processa caracters em hexadecimal */
            if (*t == 'x') {
               char num[3], i, *tt;

               tt = t++ - 1;
               i = 0;
               while (isxdigit(*t) && i < 2)
                     num[i++] = *t++;
               if (i) {
                  num[i] = 0;
                  sscanf(num, "%x", &i);
                  *tt = (char) i;
               }
               else *tt = 'x';
               strcpy(++tt, t);
               t = tt - 1;
            }
            else {
                 mod = 1;
                 switch (*t) {
                        case '0':  *(--t) = 0;   break;  /* NUL */
                        case 'a':  *(--t) = 7;   break;  /* BEL */
                        case 'b':  *(--t) = 8;   break;  /* BS  */
                        case 't':  *(--t) = 9;   break;  /* TAB */
                        case 'n':  *(--t) = 10;  break;  /* LF  */
                        case 'v':  *(--t) = 11;  break;  /* VT  */
                        case 'f':  *(--t) = 12;  break;  /* FF  */
                        case 'r':  *(--t) = 13;  break;  /* CR  */
                        case 'E':  *(--t) = 27;  break;  /* ESC */
                        case '\\': --t;          break;  /* \   */
                        default: mod = 0;
                 }
                 if (mod) strcpy(t+1, t+2);
            }
         }
}
