#include "equality.h"

/**
 * \c Equality test module callback function.
 */
void equality::entry()
{
   if (op1.read() == op2.read())
      result.write(1);
      else result.write(0);
}
