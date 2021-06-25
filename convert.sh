#!/bin/bash
/Users/dhananas/CheckedC/build/bin/3c \
-dump-stats \
-alltypes \
-p \
$BASEDIR/compile_commands.json \
-extra-arg=-w \
-base-dir=$BASEDIR \
-output-dir=$BASEDIR/out.checked \