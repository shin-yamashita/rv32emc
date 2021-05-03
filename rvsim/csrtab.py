#!/usr/bin/env python3

import sys
import csv
import re

if(len(sys.argv) < 2):
  print("*** %s .csv > csrtab.h"%sys.argv[0])
  sys.exit(0)
  
print('''//
// rv32 CSR deffs table 
// genarated by ./csrtab.py %s
//
'''%sys.argv[1])

print('''

typedef struct _csrtab {
  u16  csr;
  char *name;
} csrtab_t;

static csrtab_t csrtab[] = {
''')

prstr = ['U','S','H','M']
rwstr = ['RW','RW','RW','RO']

def mode(csr):
  icsr = int(csr, 0)
  rw = icsr >> 10
  pr = (icsr >> 8) & 0x3
  return prstr[pr]+rwstr[rw]

#//  category, csr number, privilege, name, deffs

table = []

with open(sys.argv[1], "r") as f:
  reader = csv.reader(f)
#  for row in reader:
#    if row[0] == "#2":
#      break
#  print("// ", end='')
#  for i in range(1,18):
#    print("%s, \t"%row[i], end='')
#  print()

  for row in reader:
    if len(row) > 0:
      csr  = row[1].lower()
      name = row[3].lower()
      desc = row[4].lower()
      if csr:
        line = ' { 0x%03x, %-16s }, \t// %s %s'%(int(csr, 0), '"%s"'%name, mode(csr), desc)	# mnemonic
        print( line)
        table.append((int(csr, 0), line))

from operator import itemgetter

table = sorted(table, key=itemgetter(0))

opc = {}
for i,line in enumerate(table):
  if(line[0] not in opc ) : opc[line[0]] = i
  print(line[1])

print("""
};

#define Ncsr    (sizeof(csrtab)/sizeof(csrtab_t))

""")




