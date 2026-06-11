#include "Header.h"

double Offset_Zero(DynState s);

TaskHandle  taskAI = 0;
TaskHandle  taskAO = 0;

int         RUN_DAQ_mode = 0;
double      Vgyro_offset = 0.0; 

double GetWindowTime(void)
{
    LARGE_INTEGER   liEndCounter, liFrequency;

    QueryPerformanceCounter(&liEndCounter);
    QueryPerformanceFrequency(&liFrequency);

    return(liEndCounter.QuadPart / (double)(liFrequency.QuadPart) * 1000.0);
}; // [ms]

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

    RUN_DAQ_mode = RUN_MODE_NONLINEAR;

    MotorDynamic(OFFSET_TIME, OFFSET_DIR, OFFSET_FILE, Offset_Zero);

    Vgyro_offset = Data_avg.Vgyro;
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

void SaveDataset(const char* OutDirName, const char* OutFileName, const Dataset* Out_Dataset, const int num_col, const int num_row) {

    char DirPath[N_NAME_BUFFER];
    char FilePath[N_NAME_BUFFER];
    FILE* pFile = NULL;

    snprintf(DirPath, sizeof(DirPath), "../data/%s", OutDirName);

    int result = _mkdir(DirPath);

    if (result != 0 && errno != EEXIST) {
        printf("ERROR: at creating dir\n");
    }

    snprintf(FilePath, sizeof(FilePath), "%s/%s", DirPath, OutFileName);

    pFile = fopen(FilePath, "w+t");

    if (pFile) {
        for (int i = 0; i < num_col; i++) {
            fprintf(pFile, "%20s ", Out_Dataset[i].name);
        }
        fprintf(pFile, "\n");


        for (int idx = 0; idx < num_row; idx++) {
            for (int i = 0; i < num_col; i++) {
                fprintf(pFile, "%20.10f ", Out_Dataset[i].data[idx]);
            }
            fprintf(pFile, "\n");
        }

        fclose(pFile);
        printf("-> Saved: %s/%s\n", OutDirName, OutFileName);
    }
    else printf("ERROR: At opening file %s\n", OutFileName);

}