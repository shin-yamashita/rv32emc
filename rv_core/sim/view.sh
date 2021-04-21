#!/bin/bash

function chkerr
{
 if [ $? != 0 ] ; then
  echo "***** error exit ******"
  exit
 fi
}

prj=dec
if [ $# = 1 ] ; then
 prj=$1
fi

source /opt/Xilinx/Vivado/2019.2/settings64.sh

echo Simulation Tool: Viavdo Simulator $prj

xsim -g tb_$prj.wdb -view tb_$prj.wcfg

chkerr

echo done

