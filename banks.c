
#define ALL_BANKS

#ifdef ALL_BANKS
#   define  BCO_001
#   define  BCO_215
#   define  BCO_237
#   define  BCO_291
#   define  BCO_320
#   define  BCO_389
#   define  BCO_399
#   define  BCO_422
#   define  BCO_453
#   define  BCO_479
#endif

#ifdef CLIENT_CARIOCA
#   define  BCO_341_198
#   define  BCO_399_CNR
#endif

#ifdef CLIENT_DIMARCO
#   define  BCO_001
#   define  BCO_215
#   define  BCO_237
#   define  BCO_291
#   define  BCO_320
#   define  BCO_389
#   define  BCO_399
#   define  BCO_422
#   define  BCO_453
#   define  BCO_479
#endif

#ifdef CLIENT_HUMAITA
#   define  BCO_341
#   define  BCO_409_ESPECIAL
#endif

#ifdef CLIENT_FISILABOR
#   define  BCO_275
#endif


/* Para cada carteira de cada banco, devem ser definidos as        */
/* seguintes informacoes:                                          */
/*                                                                 */
/*  Codigo do banco: o codigo do banco...                          */
/*  Nome do banco:   ...                                           */
/*  Campo Livre:     Deve conter a descricao dos dados que formam  */
/*         o campo livre no codigo de barras, assim como sua       */
/*         formatacao. Esta informacao e fornecida pelo proprio    */
/*         banco.                                                  */

static BANK bancos[] = {
#ifdef BCO_237
       { 237, "Bradesco", "",
        {{F_Agencia,       0,    4},
         {F_Carteira_N,    0,    2},
         {F_Nosso_Numero,  0,   11},
         {F_Cod_Cedente,   0,    7},
         {FF_FIXED,        0,    1,   0L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_215
       { 215, "Banco America do Sul", "",
        {{F_Agencia,       0,    4},
         {F_Cod_Cedente,   0,    7},
         {F_Cod_Cedente,   7,    1},
         {F_Carteira_N,    0,    2},
         {F_Nosso_Numero,  0,    7},
         {FF_FIXED,        0,    1,   0L},
         {F_Nosso_Numero,  7,    1},
         {FF_FIXED,        0,    2,   0L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_275
       { 275, "Banco Real", "",
        {{F_Agencia,       0,    4},
         {F_Cod_Cedente,   0,    7},
         {F_Cod_Cedente,   7,    1},
         {F_Nosso_Numero,  0,   13},
         {FF_END}
        }
       },
#endif
#ifdef BCO_399
       { 399, "Bamerindus", "",
        {{F_Nosso_Numero,  0,   11},
         {F_Agencia,       0,    4},
         {F_Cod_Cedente,   0,    7},
         {FF_FIXED,        0,    2,   0L},
         {FF_FIXED,        0,    1,   1L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_399_CNR
       { 399, "Bamerindus", "CNR",
        {{F_Cod_Cedente,   0,    7},
         {F_Nosso_Numero,  0,   13,  -3L},
         {F_Vencimento,    0,    8,   FF_JUL4},
         {FF_FIXED,        0,    1,   2L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_341
       { 341, "Banco Itau", "",
        {{F_Carteira_N,    0,    3},
         {F_Nosso_Numero,  0,    8},
         {F_Nosso_Numero,  8,    1},
         {F_Agencia,       0,    4},
         {F_Cod_Cedente,   0,    5},
         {F_Cod_Cedente,   5,    1},
         {FF_FIXED,        0,    3,   0L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_341_198
       { 341, "Banco Itau", "Carteira 198",
        {{F_Carteira_N,    0,    3},
         {F_Nosso_Numero,  0,    8},
         {F_Numero_Doc,    0,    7},
         {F_Cod_Cedente,   0,    5},
         {FF_DV10,        20,   23},
         {FF_FIXED,        0,    1,   0L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_048
       {  48, "BEMGE", "",
        {{FF_FIXED,        0,    2,   0L},
         {FF_FIXED,        0,    7,   0L},
         {F_Cod_Cedente,   0,    6},
         {F_Agencia,       0,    4},
         {FF_FIXED,        0,    1,   1L},
         {FF_FIXED,        0,   -3,   48L},
         {FF_DD_ASBACE,   20,   22},
         {FF_END}
        }
       },
#endif
#ifdef BCO_409
       { 409, "UNIBANCO", "Cobranca Direta",
        {{FF_FIXED,        0,    1,   4L},
         {F_Vencimento,    6,    2},
         {F_Vencimento,    3,    2},
         {F_Vencimento,    0,    2},
         {F_Agencia,       0,    5},
         {F_Nosso_Numero,  0,   11},
         {FF_DV11,        33,   11},
         {FF_END}
        }
       },
#endif
#ifdef BCO_409_ESPECIAL
       { 409, "UNIBANCO", "Cobranca Especial",
        {{FF_FIXED,        0,    1,   5L},
         {F_Auxiliar,      0,    7},
         {FF_FIXED,        0,    2,   0L},
         {F_Nosso_Numero,  0,   15},
         {FF_FIXED,        0,    0,   0L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_001
       { 1, "Banco do Brasil", "",
        {{F_Nosso_Numero,  0,   11},
         {F_Agencia,       0,    4},
         {FF_FIXED,        0,    4,   0L},
         {F_Cod_Cedente,   0,    4},  /* Todas as posicoes, menos a ultima */
         {F_Carteira_N,    0,    2},
         {FF_END}
        }
       },
#endif
#ifdef BCO_231
       { 231, "Banco Boavista", "",
        {{FF_FIXED,        0,    1,   0L},
         {F_Agencia,       0,    4},
         {F_Cod_Cedente,   0,    8},
         {F_Nosso_Numero,  0,   12},
         {FF_END}
        }
       },
#endif
#ifdef BCO_422
       { 422, "Banco Safra", "",
        {{FF_FIXED,        0,    1,   7L},
         {F_Agencia,       0,    5},
         {F_Cod_Cedente,   0,    9},
         {F_Nosso_Numero,  0,    9},
         {FF_FIXED,        0,    1,   2L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_320
       { 320, "BicBanco", "",
        {{F_Agencia,       0,    3},
         {F_Cod_Cedente,   0,    9},
         {F_Nosso_Numero,  0,    7},
         {FF_FIXED,        0,    5,   0L},
         {FF_FIXED,        0,    1,   2L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_453
       { 453, "Banco Rural", "",
        {{FF_FIXED,        0,    1,   0L},
         {F_Agencia,       0,    3},
         {F_Uso_Do_Banco,  0,    9},
         {F_Uso_Do_Banco,  9,    1},
         {F_Nosso_Numero,  0,    7},
         {F_Nosso_Numero,  7,    1},
         {FF_FIXED,        0,    3,   0L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_389
       { 389, "Banco Mercantil", "",
        {{F_Agencia,       0,    4},
         {F_Nosso_Numero,  0,    11},
         {F_Cod_Cedente,   0,    9},
         {FF_FIXED,        0,    1,   2L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_479
       { 479, "BankBoston", "",
        {{FF_FIXED,        0,    9,   0L},
         {FF_FIXED,        0,    6,   0L},
         {F_Nosso_Numero,  0,    9},
         {FF_FIXED,        0,    1,   8L},
         {FF_END}
        }
       },
#endif
#ifdef BCO_291
       { 291, "BCN", "",
        {{F_Agencia,       0,    3},
         {F_Cod_Cedente,   0,    7},
         {F_Nosso_Numero,  0,    8},
         {FF_FIXED,        0,    5,   0L},
         {FF_FIXED,        0,    1,   0L},
         {FF_FIXED,        0,    1,   1L},
         {FF_END}
        }
       }
#endif
};


/* Para cada banco, deve ser definido aqui o comprimento (em mm) do   */
/* logotipo. Esta informacao e' usada para imprimir a linha digitavel */
/* no lugar certo (logo apos o logo da Ficha de Compensacao.          */

static struct {
       short code;
       short ofs;
} ofs_cod[] = {
  { 237, 72 },
  { 215, 72 },
  { 275, 72 },
  { 399, 72 },
  { 341, 72 },
  {  48, 72 },
  { 409, 72 },
  { 231, 72 },
  { 320, 72 },
  { 389, 57 },
  { 453, 72 }
};

