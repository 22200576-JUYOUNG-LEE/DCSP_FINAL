#include "myHeader.h"

void InitDAQ(TaskHandle* taskAI, TaskHandle* taskAO, int devNum) {

    printf("Press any key to start the program.... \n");
    getchar();

    char aiChannelString[50];
    char aoChannelString[50];

    sprintf(aiChannelString, "Dev%d/ai0:%d", devNum, NUM_AI_CHANNELS - 1);
    sprintf(aoChannelString, "Dev%d/ao0:%d", devNum, NUM_A0_CHANNELS - 1);

    printf("Using AI channel: %s\n", aiChannelString);
    printf("Using AO channel: %s\n", aoChannelString);

    DAQmxCreateTask("", taskAI);
    DAQmxCreateTask("", taskAO);

    DAQmxCreateAIVoltageChan(*taskAI, aiChannelString, "", DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, "");
    DAQmxCreateAOVoltageChan(*taskAO, aoChannelString, "", 0, 5.0, DAQmx_Val_Volts, "");

    DAQmxStartTask(*taskAI);
    DAQmxStartTask(*taskAO);
}

void ResetDAQ(TaskHandle taskAO) {
    int32       sampsWritten;
    int32       error;
    double      Vout[2] = { MOTOR_OFF_Vsw, MOTOR_OFF_Vcmd };

    error = DAQmxWriteAnalogF64(taskAO, 1, FALSE, 10.0, DAQmx_Val_GroupByChannel, Vout, &sampsWritten, NULL);
    if (error != 0)
    {
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        printf("In function ResetDAQ error: %s\n", errBuff);
    }
}

void WriteDAQ(TaskHandle taskAO, double Vcmd) {
    int32       sampsWritten;
    int32       error;
    double      Vout[2] = { MOTOR_ON_Vsw, 0.0 };

    Vout[1] = Vcmd;

    error = DAQmxWriteAnalogF64(taskAO, 1, FALSE, 10.0, DAQmx_Val_GroupByChannel, Vout, &sampsWritten, NULL);
    if (error != 0) {
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        printf("In function WriteDAQ error: %s\n", errBuff);
    }
}

void ReadDAQ(TaskHandle taskAI, double* Vin) {
    int32       sampsRead;
    int32       error;

    error = DAQmxReadAnalogF64(taskAI, 1, 10.0, DAQmx_Val_GroupByChannel, Vin, NUM_AI_CHANNELS, &sampsRead, NULL);

    if (error != 0) {
        char errBuff[2048];
        DAQmxGetExtendedErrorInfo(errBuff, 2048);
        printf("In function ReadDAQ error: %s\n", errBuff);
    }
}

void PauseDAQ(TaskHandle taskAO) {
    double  time_init;
    double  time_curr;

    double  delay_ms = PAUSE_TIME * 1000;

    ResetDAQ(taskAO);

    time_init = GetWindowTime();
    while (1) {
        time_curr = GetWindowTime();

        if (time_curr - time_init > delay_ms) break;
    }
}

void CloseDAQ(TaskHandle taskAI, TaskHandle taskAO) {
    ResetDAQ(taskAO);

    DAQmxStopTask(taskAI);
    DAQmxStopTask(taskAO);
    DAQmxClearTask(taskAI);
    DAQmxClearTask(taskAO);
}

