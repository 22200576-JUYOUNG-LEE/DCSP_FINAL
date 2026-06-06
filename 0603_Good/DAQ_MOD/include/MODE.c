#include "Header.h"



static const char* MODE_LABELS[N_MODE] = {
    "Obtain Motor Static Property",
    "Linearized Mapping",
    "Motor Validation",
    "Bode Mag",
    "Potentio",
    "Designation"
};

Mode SelectOperatingMode(void){
    int answer = 0;

    printf("\n[Select Mode] ================\n");
    for (int i = 0; i < N_MODE; i++)
        printf("%d: %s\n", i + 1, MODE_LABELS[i]);
    printf("==============================\n");
    printf("Input: ");
    scanf_s("%d", &answer);
    printf("\n");

    return (Mode)answer;
}

void RunMode(void)
{

    switch (SelectOperatingMode()) {
    case MODE_STATIC:        IterStaticProperty(RUN_MODE_NONLINEAR);         break;
    case MODE_STATIC_LINEAR: StaticProperty(RUN_MODE_LINEAR);             break;
    case MODE_VALIDATION:    StaticValidation();                 break;
    case MODE_BODE:          BodeMag();                          break;
    case MODE_POTEN:         Potentio();                         break;
    case MODE_DESIGNATION:   Designation();                      break;



    default: printf("[ERROR] Unknown mode:\n");                  break;
    }
    
}

