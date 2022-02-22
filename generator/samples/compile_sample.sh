#!/bin/bash

SAMPLE=$PWD
SAMPLE_DIR=$(basename $SAMPLE )
SCRIPT_DIR=$(dirname $PWD/$0 )


cd $SCRIPT_DIR/../pygenerator/src
python ./generator.py -p -o /tmp/$SAMPLE_DIR $SAMPLE
echo "=== Generated in: /tmp/$SAMPLE_DIR"
