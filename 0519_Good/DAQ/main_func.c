#include "myHeader.h"

TaskHandle taskAI;
TaskHandle taskAO;
double     Vgyro_offset;

void main(void)
{
    DAQ_Init(DEVICE_NUMBER);

    RunMode(SelectOperatingMode());

    DAQ_Close();
}
