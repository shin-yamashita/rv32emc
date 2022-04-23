#
# file: write_mmi.tcl
#
# Purpose: This TCL codes reads an implemented design and builds a
# skeleton of a .mmi file to be used with 'updatemem'. 
#
#  tdt 14/04/18 -- Initial Version
#  


set myInsts [get_cells -hier -filter {PRIMITIVE_TYPE =~ BMEM.*.*}]

puts "Raw Number for Instances: [llength $myInsts]"
set bmmList {}; # make it empty incase you were running it interactively

foreach memInst $myInsts {
  if [string match "u_dpram/u_dpram_h_*" $memInst] {
    # this is the property we need
    #Property  Type  Read-only  Value
    #LOC       site     false   RAMB36_X6Y39
    # report_property $memInst LOC
    set loc [get_property LOC $memInst]
    # for BMM the location is just the XY location so remove the extra data
    set loc [string trimleft $loc RAMB36_]
    # find the bus index, this is specific to our design
    set memindex [string range $memInst [string first \[ $memInst] [string last \] $memInst]]
    # build a list in a format which is close to the output we need
    set x "$memInst $memindex $loc"
    lappend bmmList $x
    #DEBUG:
    puts "memInst: $memInst  LOC: $x"
  }
}

# debug message: 
puts "Parsed Locations Number of Intances: [llength $bmmList]"
# Remove duplicates, although there shouldn't be any
set sortbmmList [lsort -dictionary [lsort -unique $bmmList]]
#DEBUG: foreach memInst $sortbmmList { puts "Stored: $memInst" }
# FIXUP: Here we have a the fixed file location for our output:
#puts $bmmList
#puts $sortbmmList

# Open a file to put the data into.
set fp [open prog.mmi w]

set NBLK   1
set NSLICE [llength $sortbmmList];  # # of 32kbit (4kB) BRAM
set kB     [expr $NSLICE * (32/8)]; # total number of bytes
set NBit   [expr 32 / $NSLICE] ;    #  
set rvs    [expr 8 / $NBit - 1 ]

set part   [get_property PART [current_design]]

puts "   u_dpram: $kB kB ([expr $kB / 4]k x 32bit)  BRAM32 x $NSLICE / $NBit bit slice  rvs:$rvs  part:$part"

puts $fp             "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
puts $fp             "<MemInfo Version=\"1\" Minor=\"0\">"
puts $fp             "  <Processor Endianness=\"Big\" InstPath=\"u_dpram\">"
puts $fp [format     "    <AddressSpace Name=\"rv_core_0\" Begin=\"0\" End=\"%d\">" [expr $kB*1024-1 ] ]
for {set i 0} {$i < $NBLK} {incr i} {
  puts $fp           "      <BusBlock>"
  for {set j 0} {$j < $NSLICE} {incr j} {
    set mix [expr $j ^ $rvs ] ;   # Change the order of slices in 1 byte in descending order
    set printList [lindex $sortbmmList $mix]
    set loc [lindex [split $printList ] 2]
    set bit [expr $mix*$NBit ]
    #DEBUG: puts "Processing $printList"
    puts $fp [format "        <BitLane MemType=\"RAMB32\" Placement=\"%s\"><DataWidth MSB=\"%d\" LSB=\"%d\"/>" $loc [expr $bit+$NBit-1]  $bit ]
    puts $fp [format "          <AddressRange Begin=\"%d\" End=\"%d\"/><Parity ON=\"false\" NumBits=\"0\"/></BitLane>" [expr 0] [expr 32/$NBit*1024-1] ]
  }
  puts $fp           "      </BusBlock>"
}
puts $fp             "    </AddressSpace>"
puts $fp             "  </Processor>"
puts $fp             "  <Config>"
puts $fp             "    <Option Name=\"Part\" Val=\"$part\"/>"
puts $fp             "  </Config>"
puts $fp             "</MemInfo>\n"
close $fp

# cleanup
unset myInsts
unset bmmList
unset memindex
unset loc
unset x 
unset kB
unset NBLK
unset NSLICE
unset NBit
unset mix
unset printList
unset bit
unset fp 

# end of file

