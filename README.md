# cura32 OBD2 CAN Bus Reader


## Objective
The objective of this project was to take certain CAN messages from the vehicle and provide visual feedback about the vehicle's state to the occupants inside the vehicle via an LED strip.

### Examples:
* When braking the LED strip lights up red
* When pressing down on the accelerator pedal, the LED strip displays how far the pedal has been moved
* When either the left or right turn indicator is on, the LED strip will light up either the leftmost or rightmost pixel accordingly

In addition to the LED strip, the ESP32-S3 MCU also supports Bluetooth Low Energy functionality, which is leveraged by an accompanying custom iOS app. This app shows all the same information the LED strip can show, but also has readouts for vehicle speed, gear, and steering angle. The app also provides a view of the raw CAN bus information for select packet IDs.

## How It Works
The use of the CAN Bus has been mandated in the OBD2 standard since 2008. Because of this, the `CAN_H` and `CAN_L` signal wires can be accessed through OBD 2 pins 4 and 16. `CAN_H` and `CAN_L` are wired to a SN65HVD230 CAN tranceiver. The SN65HVD230 `CAN_RX` and `CAN_TX` are wired up to the ESP32-S3. The exact pins are defined in `include/cura32.h` as `CAN_RX` and `CAN_TX`. The ESP32-S3 has a built in CAN controller, and using the built in Espressif TWAI API, the messages can be used to control other external hardware (i.e. the LED strip and Bluetooth module)

## CAN Bus
The CAN bus communication system that consists of 2 data lines. Each message sent on the CAN bus will have a few properties:

* ID - The ID of the node that sent the message
* Data Length Code (DLC) - The length (in bytes) of the data section
* Data - The actual data contained in the message (sometiems called a 'packet' or a 'frame')

Depending on the message's ID, the message's data could have different meanings. For example, any message with ID 380 (decimal) will have information in the data section about the gas pedal position, engine RPM, and if the brake is being applied or not. The gas pedal position is stored in the first byte of the data section. Messages with ID 342 (decimal) will contain information about steering angle and steering angle rate of change. The steering angle is stored in the first 2 bytes of the data section. The function of all messages is not yet known, but it can be deduced by logging messages sent from the vehicle and seeing how it changes while adjusting settings in the vehicle.

## Compatibility
This project was designed for and tested with a 2018 Acura ILX Base model. Because this project does not utilize the OBD 2 standard for getting information via PIDs, much more granular information about the vehicle can be extracted. The downside of this approach is that porting this over to a different vehicle can be challenging if the CAN bus IDs and data segments are different. Ususally manufacturers tend to stick with the same setup for most of their vehicles, so this project is more likely to work in other Honda vehicles (similar to the Civic), than any other vehicles. With that said, **This project is not designed to work in any vehicle other than the 2018 Acura ILX Base model**, and ***trying to use this exact code in other vehicles may cause unexpected results***.

## Hardware and Building
Build using the PlatformIO Visual Studio Code extension. This project was written for the ESP32-S3. A dual core ESP32 device with Bluetooth Low Energy is required, and **any target other than the ESP32-S3-DevKitC-1-N8R2 may require some modification of the project files**.

The LED strip used in testing is an 8 LED RGBW NeoPixel strip from Adafruit. If more LEDs are desired, make sure to edit `include/cura32.h` and change the `LED_COUNT` and `LED_MAX_PROGRESS` values. `LED_MAX_PROGRESS` is simply the number of LEDs in use multiplied by 256
