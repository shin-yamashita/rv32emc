#!/usr/bin/env python3

def hs(s):
  if('x' in s):
    return '       x'
  else:
    return '%8x'%int(s,16)

trace = []
with open("../../rv-test/trace.log") as f:
  for line in f:
    trace.append(line.split())

run = []
with open("run.log") as f:
  for line in f:
    run.append(line.split())

#  cnt     pc       ir opc      opr   mar      mdr      mdw     rrd1     rrd2  alu rwa rwd    rwdat        ra       sp       gp       a0       a1       a2       a3       s4       s5

refs = []
for cols in trace:
  if('cnt' in cols[0]):
    print()
  elif('ra(' in cols[0]):
    break
  elif(len(cols)>13):
#   if(int(cols[1],16)>0):
    refs.append("%8x %8x %8x %8x %8x %8x"%(int(cols[1],16),int(cols[2],16),int(cols[5],16), int(cols[8],16),int(cols[9],16),int(cols[13],16)))
#    print("%8x %8x %8x %8x"%(int(cols[1],16),int(cols[2],16),int(cols[8],16),int(cols[9],16)))

dut = []
col0 = 0
for cols in run:
  if( cols[0][0] in ['0','f']):
#    print(cols)
#   if(col0 > 0):
    col0 = int(hs(cols[0]),16)
    dut.append("%8x %s %s %s %s %s"%(col0,hs(cols[1]),hs(cols[2]),hs(cols[4]),hs(cols[5]),hs(cols[6])))

for i in range(min([len(refs),len(dut)])):
  print(dut[i], refs[i])

with open("sim.log", "w") as f:
  for line in refs[1:]:
    print(line, file=f)

with open("dut.log", "w") as f:
  for line in dut[5:]:
    print(line, file=f)


