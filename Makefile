#*********************************************************************
#*                                                                   *
#* CODBAR - Impressao de Bloquetos de Cobranca com Codigo de Barra   *
#*                                                                   *
#* makefile  Compila e linka o sistema CODBAR no Unix                *
#*                                                                   *
#*********************************************************************
#*                                                                   *
#* By: Deluan Perez, General Software, Rio 40 Graus                  *
#*********************************************************************
#*       (C) Copyright General Software Tecnologia 1994              *
#*********************************************************************

# CFLAGS are flags for the C compiler.  LFLAGS are flags for the loader.
# LFLAGS2 are more flags for the loader, if they need to be at the end
# of the line instead of at the beginning.
# Defaults most systems use
CLIENT  = DIMARCO
CFLAGS  = -F -O
LFLAGS1 =
LFLAGS2 = -s

OBJS   = cbcapt.o cbcfg.o cbini.o cbcalc.o cbi25.o \
	cbprint.o cbpcode.o winprof.o cbmain.o prot.o

MAKE   = make
CC     = cc
BIND   = cc
RENAME = mv
COPY   = cp
REMOVE = rm -f
ZIP    = zipx
UNZIP  = unzipx
STRIP  = strip

BIN    = /usr/local/bin

.c.o:
	$(CC) $(CFLAGS) -DCLIENT_$(CLIENT) -c $<

#**********************************************************************
#* codbar                                                             *
#**********************************************************************

# what you can make ...

default:
	@if test -z "$(SYSTEM)";\
	then $(MAKE) list;\
	else $(MAKE) $(SYSTEM);\
	fi;

list:
	@echo ''
	@echo 'Digite "make <system>", onde <system> deve ser um dos seguintes:'
	@echo ''
	@echo ' aix, sysv, sidix ou linux'
	@echo ''
	@echo\
 'Ou entao atribua um destes a variavel SYSTEM e simplesmente digite "make".'
	@echo ''

#**********************************************************************

# IBM RS/6000 AIX 3.2
aix:
	@$(MAKE) all CFLAGS="-O"

# AT&T System V, Rel 3.  Also SCO, OpenDeskTop.
sysv:
	@$(MAKE) all CFLAGS="-O"

# Linux 1.1.0 with GNU Development Kit
linux:
	@$(MAKE) all CFLAGS="-O" CC="gcc" BIND="gcc"

# SIDIX: hardware SMX-xxx, 68xxx, da S!D INFORMATICA
sidix:
	@$(MAKE) all CFLAGS="-O -F"

#**********************************************************************

all: codbar codbar.cfg makeform dump

install: all $(BIN)/codbar $(CODBAR_DIR)/codbar.cfg $(CODBAR_DIR)/codbar.ini
	cp codbar.[0-9][0-9][0-9] $(CODBAR_DIR)
	cp *.sft $(CODBAR_DIR)
	@echo ""
	@echo Ok. Sistema CODBAR Instalado!
	@echo ""

$(BIN)/codbar: codbar
	cp codbar $(BIN)

$(CODBAR_DIR)/codbar.cfg: codbar.cfg
	cp codbar.cfg $(CODBAR_DIR)

$(CODBAR_DIR)/codbar.ini: codbar.ini
	cp codbar.ini $(CODBAR_DIR)

codbar: $(OBJS)
	$(BIND) $(LFLAGS1) $(OBJS) $(LFLAGS2) -o codbar

codbar.cfg: makecfg
	./makecfg

makeform: makeform.o
	$(BIND) $(LFLAGS1) makeform.o $(LFLAGS2) -o makeform

makecfg: makecfg.o
	$(BIND) $(LFLAGS1) makecfg.o $(LFLAGS2) -o makecfg

dump: dump.o
	$(BIND) $(LFLAGS1) dump.o $(LFLAGS2) -o dump

.sum:
	@echo \#define SUM  \"`uname -a | sum`\" > .sum

bak \
zip:
	$(REMOVE) *.bak core
	$(ZIP) -uo codbar.zip * -x *.o codbar *.zip dump makeform makecfg
	l codbar.zip

clean:
	$(REMOVE) $(OBJS) codbar makecfg makeform dump *.bak core .sum

#**********************************************************************
#* The Editor Modules                                                 *
#**********************************************************************

cbmain.o: codbar.h winprof.h

cbcapt.o: codbar.h winprof.h

cbcfg.o: codbar.h winprof.h

cbini.o: codbar.h winprof.h

cbcalc.o: codbar.h winprof.h

cbi25.o: codbar.h winprof.h

cbprint.o: codbar.h winprof.h pcl.h

cbpcode.o: codbar.h winprof.h pcl.h

winprof.o: winprof.h

makecfg.o: codbar.h winprof.h banks.c

makeform.o: codbar.h winprof.h pcl.h banks.c

dump.o: codbar.h winprof.h

prot.o: .sum

