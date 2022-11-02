#!/bin/bash

SAMPLE=$PWD
SAMPLE_DIR=$(basename $SAMPLE )
SCRIPT_DIR=$(dirname $PWD/$0 )

for SAMPLE in $(find $SCRIPT_DIR/ -type d -mindepth 1 -maxdepth 1)
do
    echo "Compile $SAMPLE"
    cd $SAMPLE
    bash ../compile_sample.sh
    if [ "$?" != "0" ]
    then
        echo "Error on compilation of $SAMPLE"
        exit 1
    fi
done
echo "Success compilation of all samples"