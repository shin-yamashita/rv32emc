#!/bin/bash

for fn in *.obj; do
 echo $fn
 tgif -print -color -eps $fn
 name=`basename $fn .obj`
 eps2svg $name.eps
 rm $name.eps $name.pdf
done
