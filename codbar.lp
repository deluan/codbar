:
#       @(#) codbar.lp 1.00 94/06/30 
#
#       Copyright (C)1994 General Software Tecnologia LTDA.
#
#!      Interface para impressao de bloquetos com codigo de barras em
#!      impressoras HP/LaserJet compativeis.
#!      Sistema Operacional: SIDIX 2.1
#

printer=`basename $0`
request=$1
name=$2
title=$3
copies=$4
options=$5
shift; shift; shift; shift; shift

CODBAR_OPT="-s -p-"
CODBAR_DIR="/usr/spool/lp/codbar"
export CODBAR_DIR

for i in $options
do
  CODBAR_OPT=$CODBAR_OPT" -"$i
done

# send the file(s) to the standard out $copies times
while   [ "$copies" -gt 0 ]
do
        for file
        do
                $CODBAR_DIR/codbar $CODBAR_OPT "$file" 2>&1
        done
        copies=`expr $copies - 1`
done

exit 0
