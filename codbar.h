/*********************************************************************
 *                                                                   *
 * CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
 *                                                                   *
 * codbar.h  Declaracao de Constantes, Variaveis e Rotinas Globais   *
 *                                                                   *
 *********************************************************************
 * Este include file contem algumas constantes e todas as            *
 * prototipacoes das funcoes publicas. Ele deve ser incluido em cada *
 * um dos modulos do sistema CODBAR.                                 *
 *                                                                   *
 * By: Perez/@rivelaro, General Software, Petropolis 0 Graus         *
 *********************************************************************
 *       (C) Copyright General Software Tecnologia 1994              *
 *********************************************************************
 * 26 Jun 1994 - DP primeira versao                                  *
 *********************************************************************/


/*********************************************************************
 * Constants                                                         *
 *********************************************************************/

/* Some details about this software */
#define NAME         "CODBAR"
#define DESCRIPTION  "Gerador Automatico de Codigo de Barras"
#define VERSION      "2.61"
#define REVISION     "Verao/2002"
#define AUTHOR       "Deluan Cotts Quint„o"

/* Nome dos arquivos do sistema */
#define DIR_DADOS    "CODBAR_DIR"
#define INIFILE      "codbar.ini"
#define CFGFILE      "codbar.cfg"
#define FONTFILE     "cbfonts.dat"

#define B_CMDFILE    "codbar."
#define B_LOGOFILE   "cblogo."
#define B_BANNERFILE "cbbanner."
#define B_CFGFILE    "cbconfig."

#define CMDFILE_ID   "CBP"
#define CFGFILE_ID   "CBF"

/* Constantes uteis */
#define OK           0

/* Data de referencia (Carta Circular 2926 de 24/7/2000 do BC */
/* Date2Int(07/10/1997)                                       */
#define DATA_REF     729747


/* Error return codes.  These are all less than zero. */
#define ERR_NOMEMORY   -1
#define ERR_INIT       -2
#define ERR_OPENFILE   -3
#define ERR_INCOMPLETE -4
#define ERR_NOCURRENCY -5
#define ERR_NOFIELD    -6
#define ERR_ACCESS     -7
#define ERR_NOBANK     -8


/*********************************************************************
 * Data Structures                                                   *
 *********************************************************************/

/* Nomeia os campos a serem capturados de uma boleta original. */
/* Estes campos sao definidos como FIELD.                      */
typedef enum _fields {
     F_Local_De_Pag_1 = 0,
     F_Local_De_Pag_2,
     F_Vencimento,
     F_Cedente,
     F_CGC_Cedente,
     F_Agencia,
     F_Cod_Cedente,
     F_Data_Doc,
     F_Numero_Doc,
     F_Especie_Doc,
     F_Aceite,
     F_Data_Proces,
     F_Carteira_N,
     F_Carteira_S,
     F_Nosso_Numero,
     F_Uso_Do_Banco,
     F_Especie_Moeda,
     F_Qtd_Moeda,
     F_Valor_Doc,
     F_Valor_Desconto,
     F_Data_Desconto,
     F_Comissao_Per,
     F_Instrucao_1,
     F_Instrucao_2,
     F_Instrucao_3,
     F_Instrucao_4,
     F_Instrucao_5,
     F_Instrucao_6,
     F_Sacado_1,
     F_Sacado_2,
     F_Sacado_3,
     F_Sacador_Aval,
     F_Unidade_Cedente,
     F_Parcela,
     F_Abatimento,
     F_Auxiliar,
     F_Codigo_Baixa,
     MAXFIELDS
} FIELDS;

/* Valores especiais para FREEFIELD.id */
typedef enum _ff_command {
     FF_END         = -1,    /* fim do campo livre */
     FF_FIXED       = -2,    /* campo de valor fixo */
     FF_DV10        = -3,    /* digito verificador 10 */
     FF_DV11        = -4,    /* digito verificador 11 */
     FF_JUL4        = -5,    /* formato DDDA, usado em FREEFIELD.fixed */
     FF_DD_ASBACE   = -6     /* duplo digito ASBACE */
} FF_COMMANDS;

/* Comandos que podem ser usados em COMMAND.cmd */
typedef enum _commands {
     C_STOPRUN      = -1,
     C_OFS          = -2,    /* Dinamico,  Form+Data */
     C_BEGINPROC    = -3,    /* Dinamico,  Form+Data */
     C_ENDPROC      = -4,    /* Dinamico,  Form+Data */
     C_EXECPROC     = -5,    /* Dinamico,  Form+Data */
     C_LOADFONT     = -6,    /* Estatico             */
     C_SELECTFONT   = -7,    /* Dinamico,  Form+Data */
     C_LOADMACRO    = -8,    /* Estatico             */
     C_EXECMACRO    = -9,    /* Dinamico,  Form      */
     C_RESETMARGINS = -10,   /* Dinamico,  Form+Data */
     C_TOPMARGIN    = -11,   /* Dinamico,  Form+Data */

     C_PRINTCALC  = -20      /* Dinamico,  Data */
} COMMANDS;

/* Par de coordenadas XY */
typedef struct _command {
       short cmd;
       short x, y;
       char parm[20];
} COMMAND;

/* Define um campo de capturacao */
typedef struct _field {
        char  name[18];        /* Nome da variavel no arquivo.ini */
        short pos_i, pos_f;    /* Posicoes inicial e final dentro da boleta */
        char *value;           /* Onde ficara armazenado o valor capturado */
} FIELD;

/* Esta estrutura mantem os dados de um elemento para construcao */
/* do campo livre para o banco atual.                            */
typedef struct _freefield {
        short id;    /* ver FF_FIXED */
        short ofs;
        short len;
        long  fixed; /* valor fixo, caso id == FF_FIXED */
} FREEFIELD;

/* Dados de um banco (contidos em CFGFILE) */
typedef struct _bank {
        short code;
        char  name[40];
        char  obs[40];
        FREEFIELD ff[9];
} BANK;

/* Especifica especie da moeda */
typedef struct _currency {
        char  *id;
        short code;
} CURRENCY;


/*********************************************************************
 * Global Data                                                       *
 *********************************************************************/

#define f_value(i)  (cbCaptFields[(int)i].value)

extern FIELD         cbCaptFields[MAXFIELDS];        /* cbCapt.c */
extern char         *cbCaptPrnConfig;
extern char         *cbCaptNewForm;
extern int           cbCaptFormLen;

extern char         *cbIniPrinter;                   /* cbIni.c */
extern int           cbIniCurrency;
extern int           cbIniNoFrame;
extern char         *cbIniDir;

extern BANK          cbCfgBank;                      /* cbCfg.c */

extern char          cbI25Start[];                   /* cbI25.c */
extern char          cbI25Stop[];

extern FILE         *cbPrnFile;                      /* cbPrint.c */

extern char          cbPcodeFile[];                  /* cbPcode.c */

/*********************************************************************
 * Public Functions                                                  *
 *********************************************************************/

int    cbCalcInit      OF((void));             /* cbCalc.c */
char  *cbCalcNumeric   OF((void));
char  *cbCalcBarCode   OF((void));
int    cbCalcCurrency  OF((char *));
char  *trim            OF((char *));

int    cbCaptChar      OF((int));              /* cbCapt.c */

char  *cbIniPath       OF((char *));           /* cbIni.c */
int    cbIniInit       OF((void));

int    cbCfgLoadBank   OF((int, int));         /* cbCfg.c */

char  *cbI25Code       OF((char *));           /* cbI25.c */
char  *cbI25Expand     OF((char *, int));

int    cbPcodeLoad     OF((void));             /* cbPCode.c */
int    cbPcodeExec     OF((int, int));

int    cbPrintInit     OF((void));             /* cbPrint.c */
int    cbPrintBegin    OF((void));
int    cbPrintFrame    OF((void));
int    cbPrintData     OF((void));
int    cbPrintEnd      OF((void));
int    cbPrintTerm     OF((void));

/*********************************************************************
 * Library Declarations                                              *
 *********************************************************************/

/* getopt.c prototypes */

extern int   getopt    OF((int, char **, char *));
extern int   optind;
extern char *optarg;
extern int   opterr;


