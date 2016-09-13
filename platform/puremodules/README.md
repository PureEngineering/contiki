# Pure Engineering PureModules

This readme will tell you where to add files for the PureModules contiki port

#### Adding PureModule Sensors

Follow these steps in order to make the modules available to the contiki build system and accessible to the PureModule core.

- Place the .c/h files into the corresponding sensor type folder in the sensors folder containing the other PureModules
- If the sensor typer folder does not exist create the folder (i.e. sensors/temperature)
  - Append the new folder in "Makefile.puremodules"
    - `CONTIKI_TARGET_DIRS += sensors/(new folder)`
- Append the source file in "Makefile.puremodules"
  - `CONTIKI_SOURCEFILES += (new module).c`
- In the core folders add the new sensor to the "peripherals.h" file
  - Put the includes behind a `#ifdef`. For example, if we just added gas-sensor.c/h to the correct folders and makefiles, in peripherals.h we add the following. Make sure the define is the same name as the file!
  ```c
  #ifdef GAS_SENSOR
  #include "gas-sensor.h"
  #endif

  SENSORS(
    ...
    #ifdef GAS_SENSOR
    &gas_sensor,
    #endif
    ...
    )
  ```
  *Note: If you are adding the last entry in the SENSORS macro do not include the comma!*

#### Creating A Project
The following steps will teach you how to create your own project using the PureModules

- Create a new folder in **examples/puremodules/(cpu)** or **examples/puremodules/common** if the example will work across multiple cores.
- Create a file called "Makefile" and add the following lines

      all:"project name"
      ### Relative path to contiki folder
      CONTIKI = ../../..
      include $(CONTIKI)/Makefile.include

  *Note: replace "project name" with the name of the c file that you want to compile. For example if you have hello-world.c, the project name is "hello world"*
- Create a .h file called "project-conf.h" and place any defines here including the sensors that you want to use.
