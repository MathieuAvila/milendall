#!/bin/bash

SAMPLE=$1
TARGET=$2

cd $(dirname $0)/pygenerator/src

python ./generator.py -p -o $TARGET $SAMPLE
ret=$?
echo "=== Generated in: $TARGET , from $SAMPLE , result $ret"
exit $ret
