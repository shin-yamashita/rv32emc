//
// rv32 CSR deffs table 
// genarated by ./csrtab.py CSR.csv
//



typedef struct _csrtab {
  u16  csr;
  char *name;
} csrtab_t;

static csrtab_t csrtab[] = {

 { 0x000, "ustatus"        }, 	// URW user status register.
 { 0x004, "uie"            }, 	// URW user interrupt-enable register.
 { 0x005, "utvec"          }, 	// URW user trap handler base address.
 { 0x040, "uscratch"       }, 	// URW scratch register for user trap handlers.
 { 0x041, "uepc"           }, 	// URW user exception program counter.
 { 0x042, "ucause"         }, 	// URW user trap cause.
 { 0x043, "utval"          }, 	// URW user bad address or instruction.
 { 0x044, "uip"            }, 	// URW user interrupt pending.
 { 0x001, "fflags"         }, 	// URW floating-point accrued exceptions.
 { 0x002, "frm"            }, 	// URW floating-point dynamic rounding mode.
 { 0x003, "fcsr"           }, 	// URW floating-point control and status register (frm + fflags).
 { 0xc00, "cycle"          }, 	// URO cycle counter for rdcycle instruction.
 { 0xc01, "time"           }, 	// URO timer for rdtime instruction.
 { 0xc02, "instret"        }, 	// URO instructions-retired counter for rdinstret instruction.
 { 0xc03, "hpmcounter3"    }, 	// URO performance-monitoring counter.
 { 0xc04, "hpmcounter4"    }, 	// URO performance-monitoring counter.
 { 0xc1f, "hpmcounter31"   }, 	// URO . performance-monitoring counter.
 { 0xc80, "cycleh"         }, 	// URO upper 32 bits of cycle, rv32 only.
 { 0xc81, "timeh"          }, 	// URO upper 32 bits of time, rv32 only.
 { 0xc82, "instreth"       }, 	// URO upper 32 bits of instret, rv32 only.
 { 0xc83, "hpmcounter3h"   }, 	// URO upper 32 bits of hpmcounter3, rv32 only.
 { 0xc84, "hpmcounter4h"   }, 	// URO upper 32 bits of hpmcounter4, rv32 only.
 { 0xc9f, "hpmcounter31h"  }, 	// URO upper 32 bits of hpmcounter31, rv32 only.
 { 0x100, "sstatus"        }, 	// SRW supervisor status register.
 { 0x102, "sedeleg"        }, 	// SRW supervisor exception delegation register.
 { 0x103, "sideleg"        }, 	// SRW supervisor interrupt delegation register.
 { 0x104, "sie"            }, 	// SRW supervisor interrupt-enable register.
 { 0x105, "stvec"          }, 	// SRW supervisor trap handler base address.
 { 0x106, "scounteren"     }, 	// SRW supervisor interrupt pending.
 { 0x140, "sscratch"       }, 	// SRW scratch register for supervisor trap handlers.
 { 0x141, "sepc"           }, 	// SRW supervisor exception program counter.
 { 0x142, "scause"         }, 	// SRW supervisor trap cause.
 { 0x143, "stval"          }, 	// SRW supervisor bad address or instruction.
 { 0x144, "sip"            }, 	// SRW supervisor interrupt pending.
 { 0x180, "satp"           }, 	// SRW supervisor address translation and protection.
 { 0x5a8, "scontext"       }, 	// SRW supervisor-mode context register.
 { 0x600, "hstatus"        }, 	// HRW hypervisor status register.
 { 0x602, "hedeleg"        }, 	// HRW hypervisor exception delegation register.
 { 0x603, "hideleg"        }, 	// HRW hypervisor interrupt delegation register.
 { 0x604, "hie"            }, 	// HRW hypervisor interrupt-enable register.
 { 0x606, "hcounteren"     }, 	// HRW hypervisor counter enable.
 { 0x607, "hgeie"          }, 	// HRW hypervisor guest external interrupt-enable register.
 { 0x643, "htval"          }, 	// HRW hypervisor bad guest physical address.
 { 0x644, "hip"            }, 	// HRW hypervisor interrupt pending.
 { 0x645, "hvip"           }, 	// HRW hypervisor virtual interrupt pending.
 { 0x64a, "htinst"         }, 	// HRW hypervisor trap instruction (transformed).
 { 0xe12, "hgeip"          }, 	// HRO hypervisor guest external interrupt pending.
 { 0x680, "hgatp"          }, 	// HRW hypervisor guest address translation and protection.
 { 0x6a8, "hcontext"       }, 	// HRW hypervisor-mode context register.
 { 0x605, "htimedelta"     }, 	// HRW delta for vs/vu-mode timer.
 { 0x615, "htimedeltah"    }, 	// HRW upper 32 bits of htimedelta, rv32 only.
 { 0x200, "vsstatus"       }, 	// HRW virtual supervisor status register.
 { 0x204, "vsie"           }, 	// HRW virtual supervisor interrupt-enable register.
 { 0x205, "vstvec"         }, 	// HRW virtual supervisor trap handler base address.
 { 0x240, "vsscratch"      }, 	// HRW virtual supervisor scratch register.
 { 0x241, "vsepc"          }, 	// HRW virtual supervisor exception program counter.
 { 0x242, "vscause"        }, 	// HRW virtual supervisor trap cause.
 { 0x243, "vstval"         }, 	// HRW virtual supervisor bad address or instruction.
 { 0x244, "vsip"           }, 	// HRW virtual supervisor interrupt pending.
 { 0x280, "vsatp"          }, 	// HRW virtual supervisor address translation and protection.
 { 0xf11, "mvendorid"      }, 	// MRO vendor id.
 { 0xf12, "marchid"        }, 	// MRO architecture id.
 { 0xf13, "mimpid"         }, 	// MRO implementation id.
 { 0xf14, "mhartid"        }, 	// MRO hardware thread id.
 { 0x300, "mstatus"        }, 	// MRW machine status register.
 { 0x301, "misa"           }, 	// MRW isa and extensions
 { 0x302, "medeleg"        }, 	// MRW machine exception delegation register.
 { 0x303, "mideleg"        }, 	// MRW machine interrupt delegation register.
 { 0x304, "mie"            }, 	// MRW machine interrupt-enable register.
 { 0x305, "mtvec"          }, 	// MRW machine trap-handler base address.
 { 0x306, "mcounteren"     }, 	// MRW  machine counter enable.
 { 0x310, "mstatush"       }, 	// MRW additional machine status register, rv32 only.
 { 0x340, "mscratch"       }, 	// MRW scratch register for machine trap handlers.
 { 0x341, "mepc"           }, 	// MRW machine exception program counter.
 { 0x342, "mcause"         }, 	// MRW machine trap cause.
 { 0x343, "mtval"          }, 	// MRW machine bad address or instruction.
 { 0x344, "mip"            }, 	// MRW machine interrupt pending.
 { 0x34a, "mtinst"         }, 	// MRW machine trap instruction (transformed).
 { 0x34b, "mtval2"         }, 	// MRW machine bad guest physical address.
 { 0x3a0, "pmpcfg0"        }, 	// MRW physical memory protection configuration.
 { 0x3a1, "pmpcfg1"        }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3a2, "pmpcfg2"        }, 	// MRW physical memory protection configuration.
 { 0x3a3, "pmpcfg3"        }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3ae, "pmpcfg14"       }, 	// MRW physical memory protection configuration.
 { 0x3af, "pmpcfg15"       }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3b0, "pmpaddr0"       }, 	// MRW physical memory protection configuration.
 { 0x3b1, "pmpaddr1"       }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3ae, "pmpcfg14"       }, 	// MRW physical memory protection configuration.
 { 0x3af, "pmpcfg15"       }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3b0, "pmpaddr0"       }, 	// MRW physical memory protection address register.
 { 0x3b1, "pmpaddr1"       }, 	// MRW physical memory protection address register, rv32 only.
 { 0x3ef, "pmpaddr63"      }, 	// MRW physical memory protection address register.
 { 0xb00, "mcycle"         }, 	// MRW machine cycle counter.
 { 0xb02, "minstret"       }, 	// MRW machine instructions-retired counter.
 { 0xb03, "mhpmcounter3"   }, 	// MRW machine performance-monitoring counter.
 { 0xb04, "mhpmcounter4"   }, 	// MRW machine performance-monitoring counter.
 { 0xb1f, "mhpmcounter31"  }, 	// MRW machine performance-monitoring counter.
 { 0xb80, "mcycleh"        }, 	// MRW upper 32 bits of mcycle, rv32 only.
 { 0xb82, "minstreth"      }, 	// MRW upper 32 bits of minstret, rv32 only.
 { 0xb83, "mhpmcounter3h"  }, 	// MRW upper 32 bits of mhpmcounter3, rv32 only.
 { 0xb84, "mhpmcounter4h"  }, 	// MRW upper 32 bits of mhpmcounter4, rv32 only.
 { 0xb9f, "mhpmcounter31h" }, 	// MRW upper 32 bits of mhpmcounter31, rv32 only.
 { 0x320, "mcountinhibit"  }, 	// MRW machine counter-inhibit register.
 { 0x323, "mhpmevent3"     }, 	// MRW machine performance-monitoring event selector.
 { 0x324, "mhpmevent4"     }, 	// MRW machine performance-monitoring event selector.
 { 0x33f, "mhpmevent31"    }, 	// MRW machine performance-monitoring event selector.
 { 0x7a0, "tselect"        }, 	// MRW debug/trace trigger register select.
 { 0x7a1, "tdata1"         }, 	// MRW first debug/trace trigger data register.
 { 0x7a2, "tdata2"         }, 	// MRW second debug/trace trigger data register.
 { 0x7a3, "tdata3"         }, 	// MRW third debug/trace trigger data register.
 { 0x7a8, "mcontext"       }, 	// MRW machine-mode context register.
 { 0x7b0, "dcsr"           }, 	// MRW debug control and status register.
 { 0x7b1, "dpc"            }, 	// MRW debug pc.
 { 0x7b2, "dscratch0"      }, 	// MRW debug scratch register 0.
 { 0x7b3, "dscratch1"      }, 	// MRW debug scratch register 1.
 { 0x000, "ustatus"        }, 	// URW user status register.
 { 0x001, "fflags"         }, 	// URW floating-point accrued exceptions.
 { 0x002, "frm"            }, 	// URW floating-point dynamic rounding mode.
 { 0x003, "fcsr"           }, 	// URW floating-point control and status register (frm + fflags).
 { 0x004, "uie"            }, 	// URW user interrupt-enable register.
 { 0x005, "utvec"          }, 	// URW user trap handler base address.
 { 0x040, "uscratch"       }, 	// URW scratch register for user trap handlers.
 { 0x041, "uepc"           }, 	// URW user exception program counter.
 { 0x042, "ucause"         }, 	// URW user trap cause.
 { 0x043, "utval"          }, 	// URW user bad address or instruction.
 { 0x044, "uip"            }, 	// URW user interrupt pending.
 { 0x100, "sstatus"        }, 	// SRW supervisor status register.
 { 0x102, "sedeleg"        }, 	// SRW supervisor exception delegation register.
 { 0x103, "sideleg"        }, 	// SRW supervisor interrupt delegation register.
 { 0x104, "sie"            }, 	// SRW supervisor interrupt-enable register.
 { 0x105, "stvec"          }, 	// SRW supervisor trap handler base address.
 { 0x106, "scounteren"     }, 	// SRW supervisor interrupt pending.
 { 0x140, "sscratch"       }, 	// SRW scratch register for supervisor trap handlers.
 { 0x141, "sepc"           }, 	// SRW supervisor exception program counter.
 { 0x142, "scause"         }, 	// SRW supervisor trap cause.
 { 0x143, "stval"          }, 	// SRW supervisor bad address or instruction.
 { 0x144, "sip"            }, 	// SRW supervisor interrupt pending.
 { 0x180, "satp"           }, 	// SRW supervisor address translation and protection.
 { 0x200, "vsstatus"       }, 	// HRW virtual supervisor status register.
 { 0x204, "vsie"           }, 	// HRW virtual supervisor interrupt-enable register.
 { 0x205, "vstvec"         }, 	// HRW virtual supervisor trap handler base address.
 { 0x240, "vsscratch"      }, 	// HRW virtual supervisor scratch register.
 { 0x241, "vsepc"          }, 	// HRW virtual supervisor exception program counter.
 { 0x242, "vscause"        }, 	// HRW virtual supervisor trap cause.
 { 0x243, "vstval"         }, 	// HRW virtual supervisor bad address or instruction.
 { 0x244, "vsip"           }, 	// HRW virtual supervisor interrupt pending.
 { 0x280, "vsatp"          }, 	// HRW virtual supervisor address translation and protection.
 { 0x300, "mstatus"        }, 	// MRW machine status register.
 { 0x301, "misa"           }, 	// MRW isa and extensions
 { 0x302, "medeleg"        }, 	// MRW machine exception delegation register.
 { 0x303, "mideleg"        }, 	// MRW machine interrupt delegation register.
 { 0x304, "mie"            }, 	// MRW machine interrupt-enable register.
 { 0x305, "mtvec"          }, 	// MRW machine trap-handler base address.
 { 0x306, "mcounteren"     }, 	// MRW  machine counter enable.
 { 0x310, "mstatush"       }, 	// MRW additional machine status register, rv32 only.
 { 0x320, "mcountinhibit"  }, 	// MRW machine counter-inhibit register.
 { 0x323, "mhpmevent3"     }, 	// MRW machine performance-monitoring event selector.
 { 0x324, "mhpmevent4"     }, 	// MRW machine performance-monitoring event selector.
 { 0x33f, "mhpmevent31"    }, 	// MRW machine performance-monitoring event selector.
 { 0x340, "mscratch"       }, 	// MRW scratch register for machine trap handlers.
 { 0x341, "mepc"           }, 	// MRW machine exception program counter.
 { 0x342, "mcause"         }, 	// MRW machine trap cause.
 { 0x343, "mtval"          }, 	// MRW machine bad address or instruction.
 { 0x344, "mip"            }, 	// MRW machine interrupt pending.
 { 0x34a, "mtinst"         }, 	// MRW machine trap instruction (transformed).
 { 0x34b, "mtval2"         }, 	// MRW machine bad guest physical address.
 { 0x3a0, "pmpcfg0"        }, 	// MRW physical memory protection configuration.
 { 0x3a1, "pmpcfg1"        }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3a2, "pmpcfg2"        }, 	// MRW physical memory protection configuration.
 { 0x3a3, "pmpcfg3"        }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3ae, "pmpcfg14"       }, 	// MRW physical memory protection configuration.
 { 0x3ae, "pmpcfg14"       }, 	// MRW physical memory protection configuration.
 { 0x3af, "pmpcfg15"       }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3af, "pmpcfg15"       }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3b0, "pmpaddr0"       }, 	// MRW physical memory protection configuration.
 { 0x3b0, "pmpaddr0"       }, 	// MRW physical memory protection address register.
 { 0x3b1, "pmpaddr1"       }, 	// MRW physical memory protection configuration, rv32 only.
 { 0x3b1, "pmpaddr1"       }, 	// MRW physical memory protection address register, rv32 only.
 { 0x3ef, "pmpaddr63"      }, 	// MRW physical memory protection address register.
 { 0x5a8, "scontext"       }, 	// SRW supervisor-mode context register.
 { 0x600, "hstatus"        }, 	// HRW hypervisor status register.
 { 0x602, "hedeleg"        }, 	// HRW hypervisor exception delegation register.
 { 0x603, "hideleg"        }, 	// HRW hypervisor interrupt delegation register.
 { 0x604, "hie"            }, 	// HRW hypervisor interrupt-enable register.
 { 0x605, "htimedelta"     }, 	// HRW delta for vs/vu-mode timer.
 { 0x606, "hcounteren"     }, 	// HRW hypervisor counter enable.
 { 0x607, "hgeie"          }, 	// HRW hypervisor guest external interrupt-enable register.
 { 0x615, "htimedeltah"    }, 	// HRW upper 32 bits of htimedelta, rv32 only.
 { 0x643, "htval"          }, 	// HRW hypervisor bad guest physical address.
 { 0x644, "hip"            }, 	// HRW hypervisor interrupt pending.
 { 0x645, "hvip"           }, 	// HRW hypervisor virtual interrupt pending.
 { 0x64a, "htinst"         }, 	// HRW hypervisor trap instruction (transformed).
 { 0x680, "hgatp"          }, 	// HRW hypervisor guest address translation and protection.
 { 0x6a8, "hcontext"       }, 	// HRW hypervisor-mode context register.
 { 0x7a0, "tselect"        }, 	// MRW debug/trace trigger register select.
 { 0x7a1, "tdata1"         }, 	// MRW first debug/trace trigger data register.
 { 0x7a2, "tdata2"         }, 	// MRW second debug/trace trigger data register.
 { 0x7a3, "tdata3"         }, 	// MRW third debug/trace trigger data register.
 { 0x7a8, "mcontext"       }, 	// MRW machine-mode context register.
 { 0x7b0, "dcsr"           }, 	// MRW debug control and status register.
 { 0x7b1, "dpc"            }, 	// MRW debug pc.
 { 0x7b2, "dscratch0"      }, 	// MRW debug scratch register 0.
 { 0x7b3, "dscratch1"      }, 	// MRW debug scratch register 1.
 { 0xb00, "mcycle"         }, 	// MRW machine cycle counter.
 { 0xb02, "minstret"       }, 	// MRW machine instructions-retired counter.
 { 0xb03, "mhpmcounter3"   }, 	// MRW machine performance-monitoring counter.
 { 0xb04, "mhpmcounter4"   }, 	// MRW machine performance-monitoring counter.
 { 0xb1f, "mhpmcounter31"  }, 	// MRW machine performance-monitoring counter.
 { 0xb80, "mcycleh"        }, 	// MRW upper 32 bits of mcycle, rv32 only.
 { 0xb82, "minstreth"      }, 	// MRW upper 32 bits of minstret, rv32 only.
 { 0xb83, "mhpmcounter3h"  }, 	// MRW upper 32 bits of mhpmcounter3, rv32 only.
 { 0xb84, "mhpmcounter4h"  }, 	// MRW upper 32 bits of mhpmcounter4, rv32 only.
 { 0xb9f, "mhpmcounter31h" }, 	// MRW upper 32 bits of mhpmcounter31, rv32 only.
 { 0xc00, "cycle"          }, 	// URO cycle counter for rdcycle instruction.
 { 0xc01, "time"           }, 	// URO timer for rdtime instruction.
 { 0xc02, "instret"        }, 	// URO instructions-retired counter for rdinstret instruction.
 { 0xc03, "hpmcounter3"    }, 	// URO performance-monitoring counter.
 { 0xc04, "hpmcounter4"    }, 	// URO performance-monitoring counter.
 { 0xc1f, "hpmcounter31"   }, 	// URO . performance-monitoring counter.
 { 0xc80, "cycleh"         }, 	// URO upper 32 bits of cycle, rv32 only.
 { 0xc81, "timeh"          }, 	// URO upper 32 bits of time, rv32 only.
 { 0xc82, "instreth"       }, 	// URO upper 32 bits of instret, rv32 only.
 { 0xc83, "hpmcounter3h"   }, 	// URO upper 32 bits of hpmcounter3, rv32 only.
 { 0xc84, "hpmcounter4h"   }, 	// URO upper 32 bits of hpmcounter4, rv32 only.
 { 0xc9f, "hpmcounter31h"  }, 	// URO upper 32 bits of hpmcounter31, rv32 only.
 { 0xe12, "hgeip"          }, 	// HRO hypervisor guest external interrupt pending.
 { 0xf11, "mvendorid"      }, 	// MRO vendor id.
 { 0xf12, "marchid"        }, 	// MRO architecture id.
 { 0xf13, "mimpid"         }, 	// MRO implementation id.
 { 0xf14, "mhartid"        }, 	// MRO hardware thread id.

};

#define Ncsr    (sizeof(csrtab)/sizeof(csrtab_t))


