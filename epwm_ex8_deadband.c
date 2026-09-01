#include "driverlib.h"
#include "device.h"
#include "board.h"

#define ADC_REF_VOLTAGE 3.3f
#define ADC_MAX_COUNT 4095.0f

// New high-side divider: 100k top, 10k bottom
// old Vin+ / boost output -> 100k -> ADC node -> 10k -> GND
#define HIGH_SIDE_DIVIDER_GAIN 11.0f

volatile uint16_t highAdcRaw = 0;
volatile float highAdcVoltage = 0.0f;
volatile float highBusVoltage = 0.0f;

// In reverse/boost mode, this is the LOW-SIDE MOSFET duty.
// Start low.
volatile float boostSwitchDuty = 0.10f;
volatile float boostDutyMin = 0.05f;
volatile float boostDutyMax = 0.20f;

// This is the actual high-side PWM duty sent into the same old PWM function.
// Since the bottom MOSFET is complementary:
// highSideDuty = 1 - boostSwitchDuty
volatile float pwmHighSideDuty = 0.90f;

volatile uint16_t pwmPeriodCounts = 0;
volatile uint16_t pwmCompareCounts = 0;

void initADCB(void);
void initADCBSOC(void);
void readHighSideADC(void);
void updateReversePWM(float bottomSwitchDuty);

void main(void)
{
    Device_init();
    Device_initGPIO();

    Interrupt_initModule();
    Interrupt_initVectorTable();

    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    Board_init();

    initADCB();
    initADCBSOC();

    updateReversePWM(boostSwitchDuty);

    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    EINT;
    ERTM;

    for(;;)
    {
        readHighSideADC();

        updateReversePWM(boostSwitchDuty);

        DEVICE_DELAY_US(100000);
    }
}

void initADCB(void)
{
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_ADCB);

    ADC_setVREF(ADCB_BASE, ADC_REFERENCE_INTERNAL, ADC_REFERENCE_3_3V);
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);
    ADC_enableConverter(ADCB_BASE);

    DEVICE_DELAY_US(1000);
}

void initADCBSOC(void)
{
    // High-side bus / old Vin+ divider on ADCINB0 / J3 pin 24
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY, ADC_CH_ADCIN0, 64);
    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
}

void readHighSideADC(void)
{
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

    ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);

    while(ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1) == false)
    {
    }

    highAdcRaw = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    highAdcVoltage = ((float)highAdcRaw * ADC_REF_VOLTAGE) / ADC_MAX_COUNT;
    highBusVoltage = highAdcVoltage * HIGH_SIDE_DIVIDER_GAIN;
}

void updateReversePWM(float bottomSwitchDuty)
{
    if(bottomSwitchDuty < boostDutyMin)
    {
        bottomSwitchDuty = boostDutyMin;
    }

    if(bottomSwitchDuty > boostDutyMax)
    {
        bottomSwitchDuty = boostDutyMax;
    }

    boostSwitchDuty = bottomSwitchDuty;

    // Old buck duty variable controlled high-side on-time.
    // For boost, the bottom MOSFET is the boost switch.
    pwmHighSideDuty = 1.0f - boostSwitchDuty;

    pwmPeriodCounts = EPWM_getTimeBasePeriod(EPWM4_BASE);
    pwmCompareCounts = (uint16_t)((float)pwmPeriodCounts * (1.0f - pwmHighSideDuty));

    EPWM_setCounterCompareValue(EPWM4_BASE, EPWM_COUNTER_COMPARE_A, pwmCompareCounts);
}