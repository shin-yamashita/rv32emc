#!/bin/bash

for fn in *.obj; do
    echo $fn
    tgif -print -color -eps $fn
    name=`basename $fn .obj`
    epstopdf $name.eps
    pdftocairo -svg $name.pdf
    rm $name.eps $name.pdf
done
