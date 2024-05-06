#!/bin/bash

function chkerr
{
 if [ $? != 0 ] ; then
  echo "***** error exit ******"
  exit
 fi
}

source xilinx_env.sh

prj=rvc

if [ $# = 1 ] ; then
 prj=$1
fi

#-generic_top "debug=0"

echo Simulation Tool: Viavdo Simulator $prj

xelab work.tb_$prj work.glbl -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -L secureip -sv_lib dpi -s tb_$prj -debug typical -dpiheader dpi.h 
xsc c_main.c

#xelab work.tb_$prj work.glbl -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -L secureip -s tb_$prj -debug typical -dpiheader dpi.h 
#xsc alu_check.c
#xelab work.tb_$prj work.glbl -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -L secureip   -sv_lib dpi  -s tb_$prj -debug typical 

echo done

