#include <stdio.h>
#include <string.h>
#include <graphics.h>
/*-----------------------------------------------------------------*/
#define _start "1010"
#define _end   "11101"
/*-----------------------------------------------------------------*/
void draw_string (int x, int y, char *txt);
int x_n2d( float x );
int y_n2d( float y );
/*-----------------------------------------------------------------*/
char bin[10][6] =
     {
     "00110",
     "10001",
     "01001",
     "11000",
     "00101",
     "10100",
     "01100",
     "00011",
     "10010",
     "01010"
     };
/*-----------------------------------------------------------------*/
int monta_bar (char *str, char *bits)
{
       int i,j,pos,one,sec;


       draw_string( x_n2d(20.0), y_n2d(150.0), str );

       i=0;
       pos=0;
       while( i < strlen(str) )
          {
          one = str[i]   - 48;
          sec = str[i+1] - 48;

          j=0;
          while( j < 5 )
             {
	          bits[pos] = bin[one][j];
	          pos++;
             bits[pos] = bin[sec][j];
             pos++;
	          j++;
	          }
	          i+=2;
	       }
	    bits[pos]=0;

       return(0);
}
/*-----------------------------------------------------------------*/
int print_i25 (char *bits)
{

       int len;
       int t,
           j;
       static char expand[4096];
       int i;

       len = strlen( bits );

       i = 0;
       expand[0] = 0;
       strcat( expand , _start );
       while( i < len )
          {
          if ( bits[i] == '1' )
             {
             if (i%2==0)
                strcat( expand, "111" );
             else
                strcat( expand, "000" );
	          }
	       else
	          {
	          if (i%2==0)
		          strcat( expand, "1" );
	          else
		          strcat( expand, "0" );
	          }
	          i++;
	       }

       strcat( expand, _end );
       len = strlen (expand);
       i = 8 - (len%8);

       for ( j=0;j<i;j++ )
           strcat( expand, "0" );

       len = strlen(expand);

       for (t=0;t<1;t++)
	        {
           for (j=0; j<len; j++)
               {
               if (expand[j]=='1')
                  {
                  setcolor(1);
                  rectangle( x_n2d(j+30.0), y_n2d(60.0-t),
                             x_n2d(j+30.0), y_n2d(10.0-t));
                  }
               }
           }

       return (0);
}

