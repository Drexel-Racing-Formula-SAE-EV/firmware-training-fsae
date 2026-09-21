#include "platform.h"
#include <assert.h>
#include <stdio.h>
uint32_t HAL_GetTick(void) { return 123U; }
int main(void)
{
    uint32_t last = 0, missed = 0;
    assert(millis() == 123U);
    assert(!time_elapsed(9, 0, 10));
    assert(time_elapsed(10, 0, 10));
    assert(time_elapsed(5, UINT32_MAX - 4U, 10));
    assert(!periodic_due(9, &last, 10, &missed));
    assert(periodic_due(35, &last, 10, &missed));
    assert(last == 30 && missed == 2);
    assert(!periodic_due(35, &last, 10, &missed));
    assert(!periodic_due(40, &last, 0, &missed));
    assert(last == 30 && missed == 2);
    last = UINT32_MAX - 4U; missed = 0;
    assert(periodic_due(5, &last, 10, &missed));
    assert(last == 5 && missed == 0);
    puts("STUDENT/REFERENCE timebase: PASS");
}
