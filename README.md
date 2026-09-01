\# C2000 Bidirectional DC-DC Converter



A bidirectional DC-DC converter controlled with a Texas Instruments C2000 microcontroller.



The project was built around the LAUNCHXL-F280049C and focuses on embedded PWM control, voltage feedback, soft-start behavior, protection logic, and operation in both buck and boost directions.



\## Overview



The converter uses the C2000 microcontroller to control the switching stage and monitor converter voltage through the ADC.



The firmware handles:



\- PWM generation

\- Complementary switching

\- Dead-band control

\- ADC voltage measurement

\- Duty-cycle control

\- Soft-start

\- Overvoltage protection

\- Emergency shutdown

\- Buck operation

\- Reverse / boost operation



The project began from a Texas Instruments ePWM dead-band example and was modified and extended for the converter hardware and control requirements.



\## Main Hardware



\- TI LAUNCHXL-F280049C C2000 LaunchPad

\- UCC27301A gate driver

\- IPP110N20N3 MOSFETs

\- Custom bidirectional converter power stage

\- Resistor-divider voltage sensing

\- Siglent SDS814X oscilloscope



\## Firmware



The main application code is located in:



```text

epwm\_ex8\_deadband.c

```



The firmware configures the C2000 peripherals and implements the converter-specific control logic.



\### PWM Control



The C2000 generates complementary PWM signals for the converter switching stage.



Dead-band is used between the high-side and low-side switching transitions to prevent both MOSFETs from conducting at the same time.



The firmware adjusts PWM duty cycle depending on the operating direction and desired output behavior.



\### Voltage Sensing



The converter output is measured using a resistor divider connected to the C2000 ADC.



The firmware converts the ADC result back into the estimated converter voltage and uses that value for monitoring and protection.



\### Soft-Start



A soft-start routine gradually changes the switching duty cycle instead of immediately commanding the final operating point.



This helps reduce large transients during converter startup.



\### Protection



The firmware includes protection behavior for unsafe operating conditions.



Implemented protection features include:



\- Output overvoltage detection

\- Emergency stop behavior

\- PWM shutdown during fault conditions



\## Buck Operation



The converter was tested in buck mode using a 10 V input.



Measured operating points included approximately:



\- 3.0 V output at about 0.30 duty cycle

\- 5.0 V output at about 0.58 duty cycle



These measurements were used to verify that the PWM control produced the expected converter behavior.



\## Boost / Reverse Operation



The converter was also operated in the reverse direction to demonstrate bidirectional power conversion.



In this mode, the firmware changes which switching duty is controlled while maintaining complementary PWM behavior.



\## Testing and Validation



The system was tested using a bench power supply and oscilloscope.



Validation included:



\- Measuring PWM waveforms

\- Checking complementary switching behavior

\- Verifying dead-band between MOSFET transitions

\- Comparing ADC-based voltage measurements with external measurements

\- Testing buck operation at different duty cycles

\- Demonstrating reverse / boost operation

\- Testing soft-start behavior

\- Triggering protection conditions to confirm PWM shutdown



\## Repository Structure



```text

c2000-bidirectional-dc-dc-converter/

├── epwm\_ex8\_deadband.c

├── epwm\_ex8\_deadband.syscfg

├── device/

├── targetConfigs/

├── .ccsproject

├── .cproject

├── .project

└── README.md

```



The `device/` directory contains Texas Instruments C2000 DriverLib support files used by the project.



\## Development Tools



\- Code Composer Studio

\- C

\- TI C2000 DriverLib

\- C2000 ePWM

\- ADC

\- Oscilloscope-based hardware testing



\## Project Goals



I built this project to gain experience controlling a real power-electronics system with embedded firmware.



The project gave me hands-on experience with PWM generation, MOSFET switching, dead-time control, ADC feedback, protection logic, and debugging software while working directly with electrical hardware.

