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

EXEEXT  = 
OBJEXT  = .o
OBJS    = cbcapt$(OBJEXT) cbcfg$(OBJEXT) cbini$(OBJEXT) \
	cbcalc$(OBJEXT) cbi25$(OBJEXT) cbprint$(OBJEXT) \
	cbpcode$(OBJEXT) winprof$(OBJEXT) cbmain$(OBJEXT) 
# prot.o

MAKE    = make
CC      = cc
BIND    = cc
RENAME  = mv
COPY    = cp
REMOVE  = rm -f
ZIP     = zipx
UNZIP   = unzipx
STRIP   = strip

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
	@echo ' aix, sysv, sidix, linux, mingw, cygwin ou win32'
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

# MinGW: Wintel with MinGW installed (www.mingw.org)
mingw:
	@$(MAKE) \
	     CFLAGS="-O -Ic:/mingw/include -Ddos -Dgnu" \
	     CC="gcc" \
	     BIND="gcc" \
	     EXEEXT=".exe" \
	     OBJS="$(OBJS) getopt.o" \
	     all

# CYGWIN: Wintel with Cygwin installed (www.cygwin.com)
cygwin:
	/bin/make all MAKE="/bin/make" CFLAGS="-O" CC="/bin/gcc" BIND="/bin/gcc"

# Win32: Wintel with Borland C++ 5.5 Free compiler
win32:
	make CFLAGS="-O -Ic:/borland/bcc55/include -VM" \
	     CC="c:/borland/bcc55/bin/bcc32" \
	     BIND="c:\borland\bcc55\bin\ilink32" \
	     EXEEXT=".exe" \
	     OBJEXT=".obj" \
	     OBJS="$(OBJS) getopt.obj" \
	     all

#**********************************************************************

all: codbar$(EXEEXT) codbar.cfg makeform$(EXEEXT) dump$(EXEEXT)

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

codbar codbar.exe: $(OBJS)
	$(BIND) $(LFLAGS1) $(OBJS) $(LFLAGS2) -o codbar$(EXEEXT)

codbar.cfg: makecfg$(EXEEXT)
	./makecfg

makeform makeform.exe: makeform.o
	$(BIND) $(LFLAGS1) makeform.o $(LFLAGS2) -o makeform$(EXEEXT)

makecfg makecfg.exe: makecfg.o
	$(BIND) $(LFLAGS1) makecfg.o $(LFLAGS2) -o makecfg$(EXEEXT)

dump dump.exe: dump.o
	$(BIND) $(LFLAGS1) dump.o $(LFLAGS2) -o dump$(EXEEXT)

.sum:
	@echo \#define SUM  \"`uname -a | sum`\" > .sum

bak \
zip:
	$(REMOVE) *.bak core
	$(ZIP) -uo codbar.zip * -x *.o codbar *.zip dump$(EXEEXT) makeform$(EXEEXT) makecfg$(EXEEXT)
	l codbar.zip

clean:
	-$(REMOVE) $(OBJS) codbar$(EXEEXT) makecfg$(EXEEXT) \
	makeform$(EXEEXT) dump$(EXEEXT) *.bak core .sum

#**********************************************************************
#* The Editor Modules                                                 *
#**********************************************************************

cbmain$(OBJEXT): codbar.h winprof.h

cbcapt$(OBJEXT): codbar.h winprof.h

cbcfg$(OBJEXT): codbar.h winprof.h

cbini$(OBJEXT): codbar.h winprof.h

cbcalc$(OBJEXT): codbar.h winprof.h

cbi25$(OBJEXT): codbar.h winprof.h

cbprint$(OBJEXT): codbar.h winprof.h pcl.h

cbpcode$(OBJEXT): codbar.h winprof.h pcl.h

winprof$(OBJEXT): winprof.h

makecfg$(OBJEXT): codbar.h winprof.h banks.c

makeform$(OBJEXT): codbar.h winprof.h pcl.h banks.c

dump$(OBJEXT): codbar.h winprof.h

prot$(OBJEXT): .sum

