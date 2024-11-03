#ifndef _NTR_ARM_H
#define _NTR_ARM_H

// From ARM reference

typedef enum Arm_ProcessorMode {
    Arm_ProcessorMode_Usr = 0b10000,
    Arm_ProcessorMode_Fiq = 0b10001,
    Arm_ProcessorMode_Irq = 0b10010,
    Arm_ProcessorMode_Svc = 0b10011,
    Arm_ProcessorMode_Mon = 0b10110,
    Arm_ProcessorMode_Abt = 0b10111,
    Arm_ProcessorMode_Und = 0b11011,
    Arm_ProcessorMode_Sys = 0b11111
} Arm_ProcessorMode;

Arm_ProcessorMode Arm_GetProcessorMode(void);

#define NTR_ARM_BUS_CLOCK 33514000

inline int Arm_MillisecondsToTicks(const int ms) {
    return (ms * (NTR_ARM_BUS_CLOCK / 1000)) / 64;
}

#endif
