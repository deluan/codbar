#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <graphics.h>
#include <conio.h>
/*-----------------------------------------------------------------*/
float _MAX_X=640.0;    /* 640.0 */
float _MAX_Y=480.0;    /* 480.0 */
#define _QLT_X  640.0
#define _QLT_Y  480.0
/*-----------------------------------------------------------------*/
int monta_bar (char *str, char *bits);
int print_i25 (char *bits);
/*-----------------------------------------------------------------*/
int open_display  (void);
int close_display (void);
int x_n2d( float x );
int y_n2d( float y );
/*-----------------------------------------------------------------*/
int x_n2d (float x)
{
       x=(_MAX_X * x)/_QLT_X;

       return ((int)(x));
}
/*-----------------------------------------------------------------*/
int y_n2d (float y)
{
       y=(_MAX_Y * y)/_QLT_Y;
       y=_MAX_Y - y;

       return ((int)(y));
}
/*-----------------------------------------------------------------*/
int open_display ()
{
       int  rc,
            gdriver,
            gmode,
            errorcode;

       gdriver=DETECT;
       errorcode=registerbgidriver (EGAVGA_driver);

       if (errorcode < 0)
          {
          printf ("Erro: Display grafico nao encontrado\n");
          exit   (1);
          }
       else
          {
          rc=0;
          initgraph (&gdriver, &gmode, "");
          if (gdriver < 9)
             {
             _MAX_X=640.0;    /* 640.0 */
             _MAX_Y=350.0;    /* 480.0 */
             //printf ("Erro: Deve ser pelo menos um VGA\n");
             //exit   (1);
             }

          }

       /* limpa a tela */
       setfillstyle (1, 15);
       bar          (x_n2d(0.0), y_n2d(0.0), x_n2d(_QLT_X), y_n2d(_QLT_Y));
       setcolor     (0);

       return( rc );
}
/*-----------------------------------------------------------------*/
int close_display ()
{
       closegraph ();
       return (0);
}
/*-----------------------------------------------------------------*/
void draw_string (int x, int y, char *txt)
{
       setcolor  (0);
       outtextxy (x, y, txt);
}
/*-----------------------------------------------------------------*/
main (int argc, char **argv)
{
       static char bits[256];

       if (argc!=2)
          {
          printf ("BARGRA <codigo>\n");
          exit(1);
          }

       open_display  ();
       monta_bar     (argv[1], bits);
       print_i25     (bits);
       getch();
       close_display ();

       return(0);
}



