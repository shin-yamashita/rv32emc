#!/usr/bin/env python3
#
# convert 32bit .mem file to 16bit x 2 
#
# input mem  : 00003197 19018193 00008117 ff810113 ...
# prog_u.mem : 0000 1901 0000 ff81 ...
# prog_l.mem : 3197 8193 8117 0113 ...
#

import sys

#f1 = open("prog.mem",   "w")
fu = open("prog_u.mem", "w")  # [31:16] part
fl = open("prog_l.mem", "w")  # [15:0] part

with open(sys.argv[1], 'r') as f:
  for line in f:
#    f1.write(line)
    col = line.split()
#    print(col)
    lu = ""
    ll = ""
    for c in col:
      lu += " "+c[0:4]
      ll += " "+c[4:8]

    fu.write(lu+"\n")
    fl.write(ll+"\n")

