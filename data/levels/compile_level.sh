#!/bin/bash

SAMPLE=$PWD
SAMPLE_DIR=$(basename $SAMPLE )
SCRIPT_DIR=$(dirname $PWD/$0 )


cd $SCRIPT_DIR/../../generator/pygenerator/src
python3 ./generator.py -p -o /tmp/$SAMPLE_DIR $SAMPLE
if [ "$?" != "0" ]
then
    exit 1
fi
echo "=== Generated in: /tmp/$SAMPLE_DIR"
