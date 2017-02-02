#ifndef MIPSREGFILEVIEW_H
#define MIPSREGFILEVIEW_H

#include "MIPSmemview.h"

class regbank;

/**
 * Visualization of register values.
 */

class MIPSregfileview : public MIPSmemview
{
    Q_OBJECT

public:
    MIPSregfileview( regbank &m, QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = 0 );
protected:
    virtual QString MemItem(unsigned int i);
};

#endif // MIPSRFILEVIEW_H
