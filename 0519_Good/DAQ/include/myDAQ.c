#include "myHeader.h"

void DAQ_Init(int dev_num)
{
    printf("Press any key to start the program....\n");
    getchar();

    char ai_ch[50];
    char ao_ch[50];

    sprintf(ai_ch, "Dev%d/ai0:%d", dev_num, NUM_AI_CHANNELS - 1);
    sprintf(ao_ch, "Dev%d/ao0:%d", dev_num, NUM_AO_CHANNELS - 1);

    printf("Using AI channel: %s\n", ai_ch);
    printf("Using AO channel: %s\n", ao_ch);

    DAQmxCreateTask("", &taskAI);
    DAQmxCreateTask("", &taskAO);

    DAQmxCreateAIVoltageChan(taskAI, ai_ch, "", DAQmx_Val_RSE,
                             -10.0, 10.0, DAQmx_Val_Volts, "");
    DAQmxCreateAOVoltageChan(taskAO, ao_ch, "", 0,
                             5.0, DAQmx_Val_Volts, "");

    DAQmxStartTask(taskAI);
    DAQmxStartTask(taskAO);


    Vgyro_offset = Find_Vgyro_offset();
}

void DAQ_Reset(void)
{
    int32  sampsWritten;
    int32  error;
    double Vout[2] = { MOTOR_OFF_VSW, MOTOR_OFF_VCMD };

    error = DAQmxWriteAnalogF64(taskAO, 1, FALSE, 10.0,
                                DAQmx_Val_GroupByChannel,
                                Vout, &sampsWritten, NULL);
    if (error != 0) {
        char err_buf[2048];
        DAQmxGetExtendedErrorInfo(err_buf, 2048);
        printf("DAQ_Reset error: %s\n", err_buf);
    }
}

void DAQ_Write(double Vcmd)
{
    int32  sampsWritten;
    int32  error;
    double Vout[2] = { MOTOR_ON_VSW, Vcmd };

    error = DAQmxWriteAnalogF64(taskAO, 1, FALSE, 10.0,
                                DAQmx_Val_GroupByChannel,
                                Vout, &sampsWritten, NULL);
    if (error != 0) {
        char err_buf[2048];
        DAQmxGetExtendedErrorInfo(err_buf, 2048);
        printf("DAQ_Write error: %s\n", err_buf);
    }
}

void DAQ_Read(double* Vin)
{
    int32 sampsRead;
    int32 error;

    error = DAQmxReadAnalogF64(taskAI, 1, 10.0,
                               DAQmx_Val_GroupByChannel,
                               Vin, NUM_AI_CHANNELS, &sampsRead, NULL);
    if (error != 0) {
        char err_buf[2048];
        DAQmxGetExtendedErrorInfo(err_buf, 2048);
        printf("DAQ_Read error: %s\n", err_buf);
    }
}

void DAQ_Pause(void)
{
    double time_init;
    double time_curr;
    double delay_ms = PAUSE_TIME * 1000.0;

    DAQ_Reset();

    time_init = GetWindowTime();
    while (1) {
        time_curr = GetWindowTime();
        if (time_curr - time_init > delay_ms) break;
    }
}

void DAQ_Close(void)
{
    DAQ_Reset();

    DAQmxStopTask(taskAI);
    DAQmxStopTask(taskAO);
    DAQmxClearTask(taskAI);
    DAQmxClearTask(taskAO);
}
