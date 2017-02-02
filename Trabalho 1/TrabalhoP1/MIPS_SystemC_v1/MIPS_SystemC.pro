QT              += qt3support
TEMPLATE	= app
CONFIG		+= qt thread release
HEADERS		= mips.h\
                  add.h alu.h ALUcontrol.h control.h dmem.h equality.h ext.h\
		  forward.h gates.h imem.h mem32.h interlock.h mux2.h mux2.h\
		  registerP.h registerT.h regbank.h shiftl2.h decode.h\
		  register_IF_ID.h register_ID_EX.h register_EX_MEM.h\
		  register_MEM_WB.h\
		  mipsaux.h\
		  GUI/MIPSctrl.h\
		  GUI/MIPSarch.h GUI/MIPSmods.h GUI/modview.h\
		  GUI/MIPSimemview.h GUI/MIPSregfileview.h GUI/MIPSdmemview.h GUI/MIPSmemview.h\
		  GUI/PortValItem.h GUI/PortValRead.h\
		  GUI/cycle.xpm GUI/reset.xpm \
		  GUI/regs.xpm GUI/datamem.xpm GUI/instmem.xpm \
		  GUI/readimem.xpm GUI/readdmem.xpm
SOURCES		= main.cpp mips.cpp\
                  add.cpp alu.cpp ALUcontrol.cpp control.cpp dmem.cpp equality.cpp ext.cpp\
		  forward.cpp gates.cpp imem.cpp mem32.cpp interlock.cpp\
		  registerP.cpp regbank.cpp shiftl2.cpp decode.cpp\
		  mipsaux.cpp\
		  GUI/MIPSctrl.cpp\
		  GUI/MIPSarch.cpp GUI/MIPSmods.cpp GUI/modview.cpp\
		  GUI/MIPSimemview.cpp GUI/MIPSdmemview.cpp GUI/MIPSregfileview.cpp GUI/MIPSmemview.cpp\
		  GUI/PortValItem.cpp GUI/PortValRead.cpp\

INCLUDEPATH     = $(SYSTEMC)/include
LIBS            += -L$(SYSTEMC)/lib-linux64 -L$(SYSTEMC)/lib-linux -lsystemc
unix:TMAKE_UIC  = /usr/bin/uic
TARGET		= MIPS_SystemC

QMAKE_CXXFLAGS +=-fpermissive
QMAKE_CXXFLAGS_RELEASE +=-fpermissive
QMAKE_CFLAGS +=-fpermissive
QMAKE_CFLAGS_RELEASE +=-fpermissive
#The following line was inserted by qt3to4
QT +=
