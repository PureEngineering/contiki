# Adding New PureModule Sensors
Here we will explain on the steps needed to add new modules for PureModules.

The goal of the PureModules sensors is to use a generic I2C interface so that only one sensor firmware has to be written. Each core will have its own I2C backend that will use the generic interface.

## Contiki Sensor API

#### Includes
Include the following files in your sensor.c file

```c
#include "contiki-conf.h"
#include "lib/sensors.h" // sensor api
#include "sys/ctimer.h" // timer for sensor ready times
#include "ti-lib.h"
#include "board-i2c.h" // generic i2c interface
#include "sensor-common.h"
```
  *Note: Other includes will be needed depending on your code*

#### Sensor Struct
The following externed struct is requred for the sensor api. This struct will be used by contiki to access your sensor.

```c
extern const struct sensors_sensor your_sensor;

```
"your_sensor" should be replaced with a descriptive name of the sensor. For example the opt-3001 light sensor was called opt_3001_sensor.

#### Required Functions
- Defines

Before we can talk about the three main functions of the sensor api we need to know of the defines that will be used.

```c
#define SENSORS_HW_INIT 128 /* internal - used only for initialization */
#define SENSORS_ACTIVE 129 /* ACTIVE => 0 -> turn off, 1 -> turn on */
#define SENSORS_READY 130 /* read only */
```
*Code found in Sensors.h*

The defines will passed into the functions as "int type". READY will be used to check if our sensor is ready, ACTIVE will be for turning our sensor on/off, and HW_INIT will not be used by us.
- Value

```c
static int value(int type)
```
The value function will return the data from our sensor. Return a descriptive error if type != SENSORS_READY. Have the value function call your own sensor reading functions.
- Configure

```c
static int configure(int type, int enable)
```
The configure function is used to intialize, turn on, and turn off your sensor. Enable will be 1 or 0 depending if your sensor is turning on or off. Returns 1 on successful startup.

Here is a good way of implementing configure.

```c
static int
configure(int type, int enable)
{
  int rv = 0;
  switch(type) {
  case SENSORS_HW_INIT:
    enable_sensor(0);
    rv = 0;
    break;
  case SENSORS_ACTIVE:
    if(enable) {
      enable_sensor(1);
      ctimer_set(&startup_timer, SENSOR_STARTUP_DELAY, notify_ready, NULL);
      rv = 1;
    } else {
      ctimer_stop(&startup_timer);
      enable_sensor(0);
      rv = 0;
    }
    break;
  default:
    break;
  }
  return rv;
}
```
*code used for opt-3001 light sensor*
- Status

```c
static int value(int type)
```
Returns
