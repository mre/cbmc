#include <assert.h>

void main(void)
{
  int r = 0;
  if ( r == 0 ) 
  {
    goto l1;
    r = 1;
  }
  l1:
  assert(r != 0); // should fail
}
