# Pure Engineering PureModules
For more information please visit www.puremodules.com. This readme will tell you where to add files for the PureModules contiki port

### Adding PureModule Sensors

Follow these steps in order to make the modules available to the contiki build system and accessible to the PureModule core.

- Place the .c/h files into the sensor folder containing the other PureModules
- Append the source file in "sensors/Makefile.sensors"
  - `CONTIKI_SOURCEFILES += (new module).c`
- In the core folders add the new sensor to the "peripherals.h" file
  - Include the sensors .h file
  - Add the sensor to the SENSORS macro

  ```c
  ...
  #include "gas-sensor.h"
  ...

  SENSORS(
    ...
    &gas_sensor,
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
- Create a .h file called "project-conf.h" and place any defines here that you want
