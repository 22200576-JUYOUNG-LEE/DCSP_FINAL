#include "myHeader.h"

#define FILE_NAME_BUFFER  256
#define DIR_NAME_BUFFER   256

// Returns wall-clock time in milliseconds.
double GetWindowTime(void)
{
    LARGE_INTEGER counter, freq;

    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&freq);

    return (double)counter.QuadPart / (double)freq.QuadPart * 1000.0;
}

void SaveDataset(const char* OutDirName, const char* OutFileName,
                 const Dataset* Out_Dataset, const int num_col, const int num_row)
{
    char  dir_path [DIR_NAME_BUFFER];
    char  file_path[FILE_NAME_BUFFER];
    FILE* pFile = NULL;

    snprintf(dir_path,  sizeof(dir_path),  "../data/%s", OutDirName);
    snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, OutFileName);

    int result = _mkdir(dir_path);
    if (result != 0 && errno != EEXIST)
        printf("ERROR: Could not create directory %s\n", dir_path);

    pFile = fopen(file_path, "w+t");

    if (pFile) {
        for (int i = 0; i < num_col; i++)
            fprintf(pFile, "%20s ", Out_Dataset[i].name);
        fprintf(pFile, "\n");

        for (int row = 0; row < num_row; row++) {
            for (int col = 0; col < num_col; col++)
                fprintf(pFile, "%20.10f ", Out_Dataset[col].data[row]);
            fprintf(pFile, "\n");
        }

        fclose(pFile);
        printf("-> Saved: %s/%s\n", OutDirName, OutFileName);
    } else {
        printf("ERROR: Could not open file %s\n", file_path);
    }
}
