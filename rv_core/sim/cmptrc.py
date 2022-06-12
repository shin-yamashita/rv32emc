#!/usr/bin/env python3

def hs(s):
  if(('x' in s) or ('X' in s)):
    return '       x'
  else:
    return '%8x'%int(s,16)

trace = []
with open("../../rv-test/trace-ecc.out") as f:
  for line in f:
    trace.append(line.split())

run = []
with open("rv_trace.out") as f:
  for line in f:
    run.append(line.split())

#  cnt     pc       ir opc      opr   mar      mdr      mdw     rrd1     rrd2  alu rwa rwd    rwdat ra(1) sp(2)
#          1        2                  5       6                 8         9                   13   14    15
refs = []
ra = 0
sp = 0
for cols in trace:
  if('cnt' in cols[0]):
    print()
  elif('ra(' in cols[0]):
    break
  elif(len(cols)>13):
    if not (('-d' in cols[3]) or ('-b' in cols[3])): 
#   if(int(cols[1],16)>0):
#    print("%8x %8x %8x %8x"%(int(cols[1],16),int(cols[2],16),int(cols[8],16),int(cols[9],16)))
#   print(cols[1],cols[2],cols[5],cols[6],cols[8],cols[9],cols[13])
#   refs.append("%8x %8x %8x %8x %8x %8x %8x"%(int(cols[1],16),int(cols[2],16),int(cols[5],16), int(cols[6],16),int(cols[8],16),int(cols[9],16),int(cols[13],16)))
      refs.append("%8x %8x %8x %8x %8x %8x %8x"%(int(cols[1],16),int(cols[2],16),int(cols[5],16),int(cols[8],16),int(cols[9],16),ra,sp))
      ra = int(cols[14],16)
      sp = int(cols[15],16)

dut = []
col0 = 0
for cols in run:
  if( cols[0][0] in ['0','f']) and len(cols) == 10:
#    print(cols)
#   if(col0 > 0):
    col0 = int(hs(cols[0]),16)
    if cols[2] == '000' or cols[2] == '001':
#    print(cols)                       #pc1     IR       bde  mar      mdr      rrd1     rrd2     rwdat   ra    sp
    #                                   pc,   ir,            mar,       mdr,       rrd1,      rrd2,     rwdat
#    dut.append("%8x %s %s %s %s %s %s"%(col0,hs(cols[1]),hs(cols[2]),hs(cols[3]),hs(cols[4]),hs(cols[5]),hs(cols[6])))
      dut.append("%8x %s %s %s %s %s %s"%(col0,hs(cols[1]),hs(cols[3]),hs(cols[5]),hs(cols[6]),hs(cols[8]),hs(cols[9])))

for i in range(min([len(refs),len(dut)])):
  print(dut[i], refs[i])

with open("sim.log", "w") as f:
#  print("#pc1     IR       bde  mar      rrd1     rrd2     rwdat    ra    sp", file=f)
  print("#pc1     IR       bde  mar      rrd1     rrd2      ra    sp", file=f)
  for line in refs[1:]:
    print(line, file=f)

with open("dut.log", "w") as f:
#  print("#pc1     IR       bde  mar      rrd1     rrd2     rwdat    ra    sp", file=f)
  print("#pc1     IR       bde  mar      rrd1     rrd2      ra    sp", file=f)
  for line in dut[2:]:
    print(line, file=f)


