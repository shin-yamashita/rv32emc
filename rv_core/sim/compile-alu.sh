#!/bin/bash

function chkerr
{
 if [ $? != 0 ] ; then
  echo "***** error exit ******"
  exit
 fi
}

source xilinx_env.sh

prj=alu

if [ $# = 1 ] ; then
 prj=$1
fi

#-generic_top "debug=0"

echo Simulation Tool: Viavdo Simulator $prj

#xelab work.tb_$prj work.glbl -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -L secureip -s tb_$prj -debug typical

xelab work.tb_$prj work.glbl -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -L secureip -s tb_$prj -debug typical -dpiheader dpi.h 
xsc alu_check.c

xelab work.tb_$prj work.glbl -timescale 1ns/1ns -prj tb_$prj.prj -i ../hdl -L unisims_ver -L secureip   -sv_lib dpi  -s tb_$prj -debug typical 

#xsim -g -wdb tb_mm8.wdb tb_mm8

echo done

