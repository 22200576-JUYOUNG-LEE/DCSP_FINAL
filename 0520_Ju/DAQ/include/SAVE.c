#include "Header.h"

double GetWindowTime(void)
{
    LARGE_INTEGER   liEndCounter, liFrequency;

    QueryPerformanceCounter(&liEndCounter);
    QueryPerformanceFrequency(&liFrequency);

    return(liEndCounter.QuadPart / (double)(liFrequency.QuadPart) * 1000.0);
}; // [ms]

void SaveDataset(const char* OutDirName, const char* OutFileName, const Dataset* Out_Dataset, const int num_col, const int num_row) {

    char DirPath[NAME_BUFFER];
    char FilePath[NAME_BUFFER];
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
    } else printf("ERROR: At opening file %s\n", OutFileName);

}