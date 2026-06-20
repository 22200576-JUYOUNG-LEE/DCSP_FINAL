#include "Header.h"

static const char* MODE_LABELS[N_MODE] = {
    "Check DAQ",
    "Obtain Motor Static Property",
    "Motor Validation",
    "Bode Mag",
    "Potentio",
    "Designation",
    "Stabilization"
};

Mode SelectOperatingMode(void){
    int answer = 0;

    printf("\n[Select Mode] ================\n");
    for (int i = 0; i < N_MODE; i++)
        printf("%d: %s\n", i, MODE_LABELS[i]);
    printf("==============================\n");
    printf("Input: ");
    scanf_s("%d", &answer);
    printf("\n");

    return (Mode)answer;
}

void RunMode(void)
{

    switch (SelectOperatingMode()) {
    case MODE_CHECK_DAQ:     Check_DAQ();               break;
    case MODE_STATIC:        IterStaticProperty();      break;
    case MODE_VALIDATION:    StaticValidation();        break;
    case MODE_BODE:          BodeMag();                 break;
    case MODE_POTEN:         Potentio();                break;
    case MODE_DESIGNATION:   Designation();             break;
    case MODE_STABIL:        Stabilization();           break;



    default: printf("[ERROR] Unknown mode:\n");         break;
    }
    
}

