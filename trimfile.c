#include <stdio.h>
#include <stdlib.h>
#ifndef __TURBOC__
#  include <unistd.h>
#endif

FILE *filent, *filsai;

main(argc, argv)
int argc;
char **argv;
{
     int ch;
     long i, size;
     int  ini,fim;


       if (argc<5)
          {
          printf( "TRIMFILE file_in file_out bytes_beg bytes_end\n" );
          exit(1);
          }

       filent = fopen( argv[1], "rb" );
       filsai = fopen( argv[2], "wb" );

       ini = atoi( argv[3] );
       fim = atoi( argv[4] );

       if ( filent && filsai )
          {
          fseek( filent, 0, SEEK_END );
          size = ftell( filent );

          fseek( filent, 0, SEEK_SET );

          for(i=0; i<ini; i++)
             ch=fgetc( filent );

          for(i=ini; i<size-fim; i++)
             {
             ch=fgetc( filent );
             fputc( ch, filsai );
             }/*for*/
          }/*if*/

       fclose(filent);
       fclose(filsai);

       return 0;
}














