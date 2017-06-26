# Pure Engineering PureModules
For more information please visit www.puremodules.com. This readme will tell you where to add files for the PureModules contiki port
### Adding PureModule core

These are the steps needed to port a new core to the Contiki build system

- Create a folder for your cpu in the core folder
- Create a Makefile called "Makefile.(cpu)"
  - cpu/cc2650/Makefile.cc2650
  - In the makefile add these lines

        CONTIKI_TARGET_DIRS += core/cc2650      ### This folder
        CONTIKI_SOURCEFILES +=                  ### Add any .c files here
        CONTIKI_CPU=$(CONTIKI)/cpu/(cpu folder) ### path to cpu in contiki/cpu
        include $(CONTIKI_CPU)/Makefile.(cpu)   ### The Makefile in the cpu folder


### Adding PureModule Sensors

Follow these steps in order to make the modules available to the contiki build system and accessible to the PureModule core.

- Place the .c/h files into the sensor folder containing the other PureModules
- Append the source file in "sensors/Makefile.sensors"
  - `CONTIKI_SOURCEFILES += (new module).c`
- In the core folders, include the new sensor to the "peripherals.h" file
  - core/cc2650/peripherals.h
- In sensors/modules.c add the new sensor

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
  *Note: Each item is seperated by a comma, the last entry does not need one!*

### Creating A Project
The following steps will teach you how to create your own project using the PureModules

- Create a new folder in **examples/puremodules/(cpu)** or **examples/puremodules/common** if the example will work across multiple cores.
- Create a Makefile. Here are some of the lines you might need to add.

      all:"project name"
      CONTIKI = ../../..                  ### Relative path to contiki folder
      include $(CONTIKI)/Makefile.include
      PROJECT_SOURCEFILES +=              ### Add any .c files
      CONTIKI_WITH_IPV6 = 1               ### Enable ipv6
      APPS +=                             ### Apps used from contiki/apps

- Create a .h file called "project-conf.h" and place any defines here that you want
