#!/bin/bash

function chkerr
{
 if [ $? != 0 ] ; then
  echo "***** error exit ******"
  exit
 fi
}

prj=rvc

source xilinx_env.sh

echo Simulation Tool: Viavdo Simulator $prj

echo Compile tb_$prj with no waveform mode

xelab work.tb_$prj -generic_top "debug=0" -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -s tb_$prj

prog="hello mul ecc fptest pi gauss"


for prg in $prog ; do

 ./convmem.py $prg.mem
 runtime=`cat $prg.ref | awk  '/Total clock cycles:/{print $4 * 11, "ns"}'`
 echo "==== run $prg.mem simulation, runtime : " $runtime

 xsim tb_$prj --log xsim-r.log <<EOF
  run $runtime
  exit
EOF

 chkerr
 echo "==== diff $prg.ref $prg.out"
 cp stderr.out $prg.out
 diff $prg.ref $prg.out
 echo "==== $prg done"

done

echo "=== regression done"
