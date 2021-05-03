#!/bin/bash

function chkerr
{
 if [ $? != 0 ] ; then
  echo "***** error exit ******"
  exit
 fi
}

#prj=dec
prj=rvc
if [ $# = 1 ] ; then
 prj=$1
fi

source /opt/Xilinx/Vivado/2019.2/settings64.sh

echo Simulation Tool: Viavdo Simulator $prj

xsim -wdb tb_$prj.wdb tb_$prj -R

chkerr

echo done

