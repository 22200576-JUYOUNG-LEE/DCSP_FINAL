#include "Header.h"

double Offset_Zero(DynState s);

TaskHandle  taskAI = 0;
TaskHandle  taskAO = 0;

int         RUN_DAQ_mode = 0;
double      Vgyro_offset = 0.0; 

void DAQ_Init() {

    printf("Press any key to start the program.... \n");
    getchar();

    char aiChannelString[50];
    char aoChannelString[50];

    sprintf(aiChannelString, "Dev%d/ai0:%d", DEVICE_NUM, NUM_AI_CHANNELS - 1);
    sprintf(aoChannelString, "Dev%d/ao0:%d", DEVICE_NUM, NUM_A0_CHANNELS - 1);

    printf("Using AI channel: %s\n", aiChannelString);
    printf("Using AO channel: %s\n", aoChannelString);

    DAQmxCreateTask("", &taskAI);
    DAQmxCreateTask("", &taskAO);

    DAQmxCreateAIVoltageChan(taskAI, aiChannelString, "", DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, "");
    DAQmxCreateAOVoltageChan(taskAO, aoChannelString, "", 0, 5.0, DAQmx_Val_Volts, "");

    DAQmxStartTask(taskAI);
    DAQmxStartTask(taskAO);

    RUN_DAQ_mode = NON_LINEAR;

    RunDAQ(OFFSET_TIME, OFFSET_DIR, OFFSET_FILE, Offset_Zero);

    Vgyro_offset = g_daqAvg.Vgyro;
    printf("\nVgyro_offset is %.4f\n", Vgyro_offset);

    ReadLinearCoefficent();

}

double Offset_Zero(DynState s) {
    return DAQ_V_STANDARD;
}


void DAQ_Reset() {
    int32       sampsWritten;
    int32       error;
    double      Vout[2] = { DAQ_V_STOP, DAQ_V_STANDARD };

    error = DAQmxWriteAnalogF64(taskAO, 1, FALSE, 10.0, DAQmx_Val_GroupByChannel, Vout, &sampsWritten, NULL);
    if (error != 0)
    {
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        printf("In function ResetDAQ error: %s\n", errBuff);
    }
}

void DAQ_Write(double Vcmd) {
    int32       sampsWritten;
    int32       error;
    double      Vout[2] = { DAQ_V_START, 0.0 };

    Vout[1] = Vcmd;

    error = DAQmxWriteAnalogF64(taskAO, 1, FALSE, 10.0, DAQmx_Val_GroupByChannel, Vout, &sampsWritten, NULL);
    if (error != 0) {
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        printf("In function WriteDAQ error: %s\n", errBuff);
    }
}

void DAQ_Read(double* Vin) {
    int32       sampsRead;
    int32       error;

    error = DAQmxReadAnalogF64(taskAI, 1, 10.0, DAQmx_Val_GroupByChannel, Vin, NUM_AI_CHANNELS, &sampsRead, NULL);

    if (error != 0) {
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        printf("In function ReadDAQ error: %s\n", errBuff);
    }
}

void DAQ_Pause(double time_delay) {
    double  time_init;
    double  time_curr;

    double  delay_ms = time_delay * 1000;

    DAQ_Reset();

    time_init = GetWindowTime();
    while (1) {
        time_curr = GetWindowTime();

        if (time_curr - time_init > delay_ms) break;
    }
}

void DAQ_Close() {
    DAQ_Reset();

    DAQmxStopTask(taskAI);
    DAQmxStopTask(taskAO);
    DAQmxClearTask(taskAI);
    DAQmxClearTask(taskAO);
}

