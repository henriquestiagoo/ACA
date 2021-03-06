// MIPSmods.cpp


#include "../mips.h"
#include "modview.h"

#include "MIPSmods.h"
//Added by qt3to4:
#include <QPixmap>
#include <QCloseEvent>

#define MODSTEPX 160   // horizontal spacing of module views

/**
 * MIPSmods contructor.
 * Creates and inicializes the widgets that display the
 * port values of intruction memory, control, decode, register file,
 * alu and data memory.
 */
MIPSmods::MIPSmods( mips &m, QWidget* parent,  const char* name, Qt::WFlags fl )
    : Q3ScrollView( parent, name, fl ), mips1(m)
{
    //resize viewport
    resizeContents(820+5,194+5);

    resize( QSize(820+10,194+10) );
    setMaximumSize( QSize(820+10,194+10) );

    setIcon(QPixmap("mips.xpm"));

    setVScrollBarMode(Auto);
    setHScrollBarMode(Auto);

    //imemview
    ModView *imemView=new ModView("imem",viewport());
    imemView->addPort(mips1.instmem->addr, "PC");
    imemView->addPort(mips1.instmem->inst, "inst");
    addChild(imemView,5,5);

    connect(this,SIGNAL(updateModules()), imemView, SLOT(redrawModule()));

    //ctrlview
    ModView *ctrlView=new ModView("Control",viewport());
    ctrlView->addPort(mips1.ctrl->opcode,  "opcode");
    ctrlView->addPort(mips1.ctrl->Branch,  "Branch");
    ctrlView->addPort(mips1.ctrl->RegDst,  "RegDst");
    ctrlView->addPort(mips1.ctrl->ALUSrc,  "ALUSrc");
    ctrlView->addPort(mips1.ctrl->ALUOp,   "ALUOp");
    ctrlView->addPort(mips1.ctrl->MemRead, "MemRead");
    ctrlView->addPort(mips1.ctrl->MemWrite,"MemWrite");
    ctrlView->addPort(mips1.ctrl->MemtoReg,"MemtoReg");
    ctrlView->addPort(mips1.ctrl->RegWrite,"RegWrite");
    addChild(ctrlView,5+MODSTEPX*1,5);

    connect(this,SIGNAL(updateModules()), ctrlView, SLOT(redrawModule()));

    //rbankview
    ModView *rfileView=new ModView("regbank",viewport());
    rfileView->addPort(mips1.rbank->add1, "r1");
    rfileView->addPort(mips1.rbank->add2, "r2");
    rfileView->addPort(mips1.rbank->write, "write");
    rfileView->addPort(mips1.rbank->dout1, "r1data");
    rfileView->addPort(mips1.rbank->dout2, "r2data");
    rfileView->addPort(mips1.rbank->addin, "rdest");
    rfileView->addPort(mips1.rbank->din, "destdata");
    addChild(rfileView,5+MODSTEPX*2,5);

    connect(this,SIGNAL(updateModules()), rfileView, SLOT(redrawModule()));

    //aluView
    ModView *aluView=new ModView("ALU",viewport());
    aluView->addPort(mips1.ALU->din1, "din1");
    aluView->addPort(mips1.ALU->din2, "din2");
    aluView->addPort(mips1.ALU->op, "op");
    aluView->addPort(mips1.ALU->dout, "dout");
    addChild(aluView,5+MODSTEPX*3,5);

    connect(this,SIGNAL(updateModules()), aluView, SLOT(redrawModule()));

    //dmemview
    ModView *dmemView=new ModView("dmem",viewport());
    dmemView->addPort(mips1.datamem->addr, "addr");
    dmemView->addPort(mips1.datamem->din, "din");
    dmemView->addPort(mips1.datamem->dout, "dout");
    addChild(dmemView,5+MODSTEPX*4,5);

    connect(this,SIGNAL(updateModules()), dmemView, SLOT(redrawModule()));

    setFocusPolicy(Qt::StrongFocus);
}

/*
 *  Destroys the object and frees any allocated resources
 */
MIPSmods::~MIPSmods()
{
    // no need to delete child widgets, Qt does it all for us
}

void MIPSmods::closeEvent(QCloseEvent *e)
{
   e->ignore();  //dont close
}
