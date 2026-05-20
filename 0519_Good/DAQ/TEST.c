# include "include/myHeader.h"

int main_1(void) {

    double Out_Time[2] = { 0.0 };
    double Out_ao1_Data[2] = { 0.1, 0.2 };

    const char* DirName = "test1";
    const char*  OutFileName = "test5.out";

    Dataset     Out_DAQ_Dataset[] = {
        {"Time[sec]", Out_Time},
        {"\\omega_{gyro}[rad]",  Out_ao1_Data},
    };

    SaveDataset(DirName, OutFileName, Out_DAQ_Dataset, 2, 2);

	return 0;
}