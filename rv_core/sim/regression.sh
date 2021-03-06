#!/bin/bash

function chkerr
{
 if [ $? != 0 ] ; then
  echo "***** error exit ******"
  exit
 fi
}

prj=rvc

source /opt/Xilinx/Vivado/2019.2/settings64.sh

echo Simulation Tool: Viavdo Simulator $prj

echo Compile tb_$prj with no waveform mode

xelab tb_$prj -generic_top "debug=0" -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -s tb_$prj

prog="hello mul ecc fptest pi gauss"


for prg in $prog ; do

 echo "==== run $prg.mem simulation"
 ./convmem.py $prg.mem
 xsim tb_$prj --log xsim-r.log <<EOF
  run 80 ms
  exit
EOF
 chkerr
 echo "==== diff $prg.ref $prg.out"
 cp stderr.out $prg.out
 diff $prg.ref $prg.out
 echo "==== $prg done"

done

echo done

