* p - needs to be ported for micro

* a - abstract class
  
App
===
- main
    - buffer definitions/ allocation
    - parameter/ voice assignment
- config
    - interrupt hooks
    - I/O
    - assert checks

Modifier
========
- a modifier
- lfo
- env
- stepper
- midi controller
- cv controller

Voice
=====
- voice 
- a parameter
- a generator
    - osc
    - sampler
    - resonator
- a processor
    - filter
    - fx etc.

UI
=====
- a I/O
- led
- midi
- button
- potentiometer
- cv
- a screen
    - touch
    - tft
- a connect
    - bootloader
    - host

Backbone 
========

Perpiheral
++++++++++
- p UART
- p SPI
- p I2C
- p USB
- p PIN
- p DMA
- a ADC 
- a DAC
- a Flash

System
++++++
- SYS
- DBG
- STG
- p Power
- p Reset
- p Clock

Util
++++
- buffer
- math

Port
====
- p MCU.h
- p UART
- p SPI
- p I2C
- p USB
- p PIN
- p DMA
- p Power
- p Reset
- p Clock
- p internal DAC
- p internal ADC 
- p Internal flash