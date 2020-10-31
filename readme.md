# rhio-puf

Portable C++ library to create event driven applications using the publisher/subscriber pattern. Works primarily on embedded constrained devices.

## Why and event driven library?

Develop good code for microcontrollers is not easy at all. Develop large [good]applications for microcontroller is hard. This library allow different modules on the firmware to communicate with others to notify about changes or actions in the system.

We use it in Rhomb.IoT, our IoT framework. For example when the battery module detects a low battery charge it fires an event to tell other modules, like the network module, that enable the low power mode. There are a lot of usages. For example, the ISR library can launch an event when an interruption has been fired and multiple callbacks will be executed to do any required action.

## How it works

It exports mainly three methods:

- **puf::on()** *[subscriber]* Add a callback to an event. Pass a event name and callback.
- **puf::emit()** *[publisher]* Execute all callbacks added with puf::on()
- **puf::off()**  Removes a certain callback from an event

Every event has a name, there a limit of events (static memory allocated) and every event may have multiple callbacks assigned.

Minimal example:

```c++
#include "rhio-puf.h"

Puf puf;

// define a event
#define EV_INIT 0x01

// define 2 callbacks for the event
pufcb init_temp_sensor(int param) {
  printf("init_temp_sensor of EV_INIT");
  return 0;
}

pufcb int_lcd_sensor(int param) {
  printf("int_lcd_sensor of EV_INIT");
  return 0;
}


int main() {

  // Create the subscribers
  puf.on(EV_INIT, init_temp_sensor);
  puf.on(EV_INIT, int_lcd_sensor);

  // publish the event
  // init_temp_sensor() and int_lcd_sensor() will be executed in order
  puf.emit(EV_INIT, 0);

  while (1){}
  return 0;
}

```

The example shows how to publish an event to run multiple callbacks in order.

Every callback should return an error code represented with an `unsigned char`. The value `0` represents a success state, any value greater than 0 will be an error and will interrupt the execution of the next callbacks.

Managing errors using the previuos example:

```c++
#include "rhio-puf.h"

Puf puf;

#define EV_INIT 0x01

pufcb init_temp_sensor(int param) {
  // simulate this functions fails because the sensor temp is not available
  // (at hardware level)
  return 0x12; // 0x12 is the error code for this sensor
}

pufcb int_lcd_sensor(int param) {
  printf("int_lcd_sensor of EV_INIT");
  return 0;
}


int main() {

  puf.on(EV_INIT, init_temp_sensor);
  puf.on(EV_INIT, int_lcd_sensor);

  // init_temp_sensor() returns an error code (0x12)
  // int_lcd_sensor() will be not executed
  char err = puf.emit(EV_INIT, 0);
  if (err > 0) {
    if (err == 0x12) { // replace "if" with some kind of `process_error()` method
      printf("FATAL ERROR, please check your hardware...");
      while (1) {}; // hang the system
    }
  }

  while (1){ [...] }
  return 0;
}

```

Again we are trying to run multiple callbacks, but the first has return an error code and then the execution has stopped, the second callback will not run.

The return error code may be processed as you want. In this last example we are hanging the system with a `while(1);` _because we cant not continue with that sensor_.

### Order of invocation

The order of invocation of callbacks is equivalent to the order of calls to `puf.on()`. On the previuos examples, when call `puf.emit()`, `init_temp_sensor()` will be executed always before `int_lcd_sensor()`.

There are no options to set order execution or priority.

## Development

If you want to contribute to the development, the best way is to use [rhio-devkit](https://github.com/Rhomb-io/rhio-devkit), a project with VSCode and Platformio that includes a development copy of this repository.

## License

Developed by [Rhomb.io](https://rhomb.io) Software Team from Tecnofigners. Licensed under the GNU/GPL V3. Please concat us for commercial license or custom development.
