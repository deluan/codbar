
/* SoftFonts usadas pelo CODBAR */
#define ARI06n  "ari06n__.sft"
#define ARI08n  "ari08n__.sft"
#define ARI09b  "ari09b__.sft"
#define ARI10b  "ari10b__.sft"
#define ARI14b  "ari14b__.sft"

/* Algumas constantes */
#define RES       300 /* dpi */
#define FORMFEED  12

/* Macros para impressao PCL */
#define SelectFont(n)          (fprintf(cbPrnFile, "\x1b(%dX", GetFontId(n)))
#define PrnCmd_s(cmd,arg,mode) (fprintf(cbPrnFile, "\x1b%s%s%c", (cmd), (arg), (mode)))
#define PrnCmd_n(cmd,arg,mode) (fprintf(cbPrnFile, "\x1b%s%d%c", (cmd), (int)(arg), (mode)))
#define PrnXY(x,y)             (fprintf(cbPrnFile, "\x1b*p%dx%dY", (int)(x), (int)(y)))

/* Conversoes de unidades: milimetros <-> dots */
#define m2d(x)                 ((int)((double)(x) * RES/25.4))
#define d2m(x)                 ((int)((double)(x) * 25.4/RES))

