//MIPSarch.cpp
//
// Widget to display MIPS architecture
//

#include <qpixmap.h>
#include <qevent.h>
#include <qcursor.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QMouseEvent>

#include "PortValItem.h"
#include "PortValRead.h"

#include "MIPSarch.h"
#include "../mips.h"
#include "../mipsaux.h"

/**
 * MIPSarchcanvas constructor.
 */

MIPSarchCanvas::MIPSarchCanvas(mips &m, QObject *parent) : Q3Canvas(parent), mips1(m)
{
    // set background to MipsArch.png
    QPixmap backFig("GUI/MIPS_datapath_2016.png");
    resize(backFig.width(),backFig.height());
    setBackgroundPixmap(backFig);

    // create and position the items that display the port values
    PortValItem *portVal;
    Q3CanvasText *qText;

    // instruction labels in stages
    qText=new Q3CanvasText("nop",this);
    qText->setTextFlags(Qt::AlignLeft | Qt::AlignTop);
    qText->setVisible(true);
    qText->move(10,6);
    qText->setColor(QColor("red"));
    instStage.push_back(qText);
    pcStage.push_back(new PortValRead(mips1.PCreg->dout,"PC"));
    validpcStage.push_back(new PortValRead(mips1.reg_IF_ID->valid_IF,"valid_PC"));

    qText=new Q3CanvasText("nop",this);
    qText->setTextFlags(Qt::AlignLeft | Qt::AlignTop);
    qText->setVisible(true);
    qText->move(253,6);
    qText->setColor(QColor("red"));
    instStage.push_back(qText);
    pcStage.push_back(new PortValRead(mips1.reg_IF_ID->PC_ID,"PC_ID"));
    validpcStage.push_back(new PortValRead(mips1.reg_IF_ID->valid_ID,"valid_PC_ID"));

    qText=new Q3CanvasText("nop",this);
    qText->setTextFlags(Qt::AlignLeft | Qt::AlignTop);
    qText->setVisible(true);
    qText->move(484,6);
    qText->setColor(QColor("red"));
    instStage.push_back(qText);
    pcStage.push_back(new PortValRead(mips1.reg_ID_EX->PC_EX,"PC_EX"));
    validpcStage.push_back(new PortValRead(mips1.reg_ID_EX->valid_EX,"valid_PC_EX"));

    qText=new Q3CanvasText("nop",this);
    qText->setTextFlags(Qt::AlignLeft | Qt::AlignTop);
    qText->setVisible(true);
    qText->move(661,6);
    qText->setColor(QColor("red"));
    instStage.push_back(qText);
    pcStage.push_back(new PortValRead(mips1.reg_EX_MEM->PC_MEM,"PC_MEM"));
    validpcStage.push_back(new PortValRead(mips1.reg_EX_MEM->valid_MEM,"valid_PC_MEM"));

    qText=new Q3CanvasText("nop",this);
    qText->setTextFlags(Qt::AlignLeft | Qt::AlignTop);
    qText->setVisible(true);
    qText->move(800,6);
    qText->setColor(QColor("red"));
    instStage.push_back(qText);
    pcStage.push_back(new PortValRead(mips1.reg_MEM_WB->PC_WB,"PC_WB"));
    validpcStage.push_back(new PortValRead(mips1.reg_MEM_WB->valid_WB,"valid_PC_WB"));

    // value of port signals
    // IF
    portVal=new PortValItem(this,mips1.instmem->addr, "PC");
    portVal->move(86,287);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.add4->res, "PC4");
    portVal->move(174,173);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.PCreg->din, "NPC");
    portVal->move(52,306);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.instmem->inst, "inst");
    portVal->move(172,303);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    //ID
    portVal=new PortValItem(this,mips1.ctrl->opcode, "opcode");
    portVal->move(249,51);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.ctrl->Branch, "Branch");
    portVal->move(344,105);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.e32->din, "immediate");
    portVal->move(218,209);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.rbank->add1, "rs");
    portVal->move(237,270);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.rbank->add2, "rt");
    portVal->move(237,287);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.rbank->dout1, "dataout1");
    portVal->move(368,266);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.rbank->dout2, "dataout2");
    portVal->move(368,286);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.aeq->dout, "PCSrc");
    portVal->move(48,245);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.intlock->enable_PC, "PCWrite");
    portVal->move(76,245);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.intlock->enable_IFID, "IFIDWrite");
    portVal->move(208,85);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mNOP0->sel, "NOP selection");
    portVal->move(376,92);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.addbr->res, "target address");
    portVal->move(365,152);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.fwd->fwdIDA, "selection IDA");
    portVal->move(390,176);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.fwd->fwdIDB, "selection IDB");
    portVal->move(330,210);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.fwd->fwdEXA, "selection EXA");
    portVal->move(530,77);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.fwd->fwdEXB, "selection EXB");
    portVal->move(528,169);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.fwd->fwdMEM, "selection MEM");
    portVal->move(651,194);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.eq->op1, "rsdataTest");
    portVal->move(295,224);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.eq->op2, "rsdataTest");
    portVal->move(295,237);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    //EXE
    portVal=new PortValItem(this,mips1.aluctrl->funct, "funct");
    portVal->move(492,240);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.reg_ID_EX->imm_EX, "offset");
    portVal->move(427,188);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.reg_ID_EX->rsData_EX, "rsData_EX");
    portVal->move(427,141);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.reg_ID_EX->rtData_EX, "rtData_EX");
    portVal->move(427,228);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.aluctrl->ALUOp, "ALUOp");
    portVal->move(492,255);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.ALU->op, "op");
    portVal->move(565,220);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.ALU->din1, "ALUin1");
    portVal->move(508,142);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.ALU->din2, "ALUin2");
    portVal->move(508,217);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.ALU->dout, "ALUout");
    portVal->move(569,180);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mDst->sel, "RegDst_EX");
    portVal->move(557,291);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mDst->din0, "RegRt_EX");
    portVal->move(438,302);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mDst->din1, "RegRd_EX");
    portVal->move(438,320);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.reg_EX_MEM->rdest_EX, "rdest_EX");
    portVal->move(567,313);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    //MEM
    portVal=new PortValItem(this,mips1.reg_EX_MEM->rdest_MEM, "rdest_MEM");
    portVal->move(624,313);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.datamem->rd, "MemRead_MEM");
    portVal->move(703,146);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.datamem->wr, "MemWrite_MEM");
    portVal->move(703,264);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.datamem->addr, "MemAdd_MEM");
    portVal->move(625,165);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.datamem->din, "MemDataIn_MEM");
    portVal->move(655,223);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.datamem->dout, "MemDataOut_MEM");
    portVal->move(736,208);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    //WB
    portVal=new PortValItem(this,mips1.reg_MEM_WB->rdest_WB, "rdest_WB1");
    portVal->move(786,313);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.reg_MEM_WB->rdest_WB, "rdest_WB2");
    portVal->move(277,308);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mreg->dout, "destdata_WB1");
    portVal->move(831,198);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mreg->dout, "destdata_WB2");
    portVal->move(261,332);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.reg_MEM_WB->RegWrite_WB, "RegWrite_WB1");
    portVal->move(835,294);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.reg_MEM_WB->RegWrite_WB, "RegWrite_WB2");
    portVal->move(330,365);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mreg->sel, "MemToReg_WB");
    portVal->move(822,237);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mreg->din0, "ALUout_WB");
    portVal->move(787,128);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    portVal=new PortValItem(this,mips1.mreg->din1, "MemOut_WB");
    portVal->move(787,205);
    portVal->setColor(QColor("blue"));
    portValVec.push_back(portVal);

    updateArch();

    setDoubleBuffering(true);
}

/**
 * updates the values of all ports values.
 */
void MIPSarchCanvas::updateArch(void)
{
    unsigned int p,s,addr;
    char instText[200];
    for(p=0; p<portValVec.size(); p++) {
         portValVec[p]->updateVal();
    }
    for(s=0; s<instStage.size(); s++) {
	 if(validpcStage[s]->read() == 1) { // Stage PC is valid
	     if(pcStage[s]->read() < mips1.instmem->size()) { // StagePC is less than size of instmem
	        addr=mips1.instmem->at(pcStage[s]->read());
	        disassemble(addr,instText);
                instStage[s]->setText(instText);
	     }
	     else instStage[s]->setText("nop");
	 }
	 else instStage[s]->setText("bubble");
    }
    update();
}

MIPSarchCanvas::~MIPSarchCanvas()
{
    // no need to delete child widgets, Qt does it all for us
    for(unsigned int p=0; p < portValVec.size(); p++) {
       delete portValVec[p];
       portValVec[p]=0;
    }
}


/**
 * MIPSarch contructor.
 * Creates and sets the MIPSarchcanvas to be displayed, and some
 * GUI functionalities.
 */
MIPSarch::MIPSarch( mips &m, QWidget* parent,  const char* name, Qt::WFlags fl )
    : Q3CanvasView(0, parent, name, fl )
{
    archCanvas=new MIPSarchCanvas(m,this);
    setCanvas(archCanvas);

    setIcon(QPixmap("mips.xpm"));

    resize(QSize(896+5,455+5));
    setMaximumSize(QSize(896+5,455+5));

    cursor=new QCursor(Qt::PointingHandCursor);
    setCursor(*cursor);

    viewport()->setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

/*
 *  Destroys the object and frees any allocated resources
 */
MIPSarch::~MIPSarch()
{
    // no need to delete child widgets, Qt does it all for us
    delete cursor;
}

/**
 * updates the port values.
 */
void MIPSarch::updateArch(void)
{
     archCanvas->updateArch();
}

/**
 * emits signals when click is inside certain regions
 */
void MIPSarch::contentsMousePressEvent(QMouseEvent *e)
{
/*

    QPoint pos=e->pos();
    if(pos.x() > 80 && pos.x() < 112
       && pos.y()> 200 && pos.y() < 270)
       emit imemClicked();
    if(pos.x() > 320 && pos.x() < 370
       && pos.y()> 190 && pos.y() < 300)
       emit regfileClicked();
    if(pos.x() > 590 && pos.x() < 645
       && pos.y()> 220 && pos.y() < 280)
       emit dmemClicked();
*/
}

/**
 * changes cursor when mouse is over certain regions
 */

void MIPSarch::contentsMouseMoveEvent(QMouseEvent *e)
{
    QPoint pos=e->pos();

//    fprintf(stderr,"x=%4d y=%4d\n",pos.x(),pos.y());

    cursor->setShape(Qt::ArrowCursor);
    setCursor(*cursor);

}

/**
 * This window is not to be closed.
 */
void MIPSarch::closeEvent(QCloseEvent *e)
{
   e->ignore();
}
