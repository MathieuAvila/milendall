#!/bin/bash

SRC=$1
TARGET=$2

x=$(find  $1 -type f -printf '%p ')
echo $2/level.json: $x > $2/depends.txt

