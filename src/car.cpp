#include "car.h"
#include <Arduino.h>

void Car::update_car_state(twai_message_t message) {
    if (message.rtr) {
        return;
    }

    // each message that gets passed into this function will only modify a few attributes
    switch (message.identifier) {
        case STEERING_SENSORS_FRAME_ID: {  // handle steering angle change
            this->steer_angle = ((message.data[0] << 8) | message.data[1]) / 10;
            break;
        }
        case POWERTRAIN_DATA_FRAME_ID: {  // handle RPM, braking, and gas pedal position changes
            this->gas_pedal_position = message.data[0];
            this->brake = (message.data[4] & 1) == 1;
            this->rpm = (message.data[2] << 8) | message.data[3];
            break;
        }
        case CAR_SPEED_FRAME_ID: {  // handle speed change
            float kph = ((message.data[4] << 8) | message.data[5]) * 0.01;
            this->speed = (uint8_t)(kph / 1.609344);
            break;
        }
        case GEARBOX_FRAME_ID: {  // handle gear change
            this->gear = message.data[0];
            break;
        }
        case TURN_SIGNAL_FRAME_ID: {  // handle turn signal changes
            this->turn_signals = message.data[0];
            break;
        }
		case CRUISE_BUTTONS_FRAME_ID: {
			this->headlights = (message.data[0] & 0x02) == 0x02;
			break;
		}
		case CRUISE_FRAME_ID: {
			this->cruise_speed = message.data[4];
			break;
		}
		case BRAKE_PRESSURE_FRAME_ID: {
			this->brake_pressure = (message.data[0] << 8) | message.data[1];
			break;
		}
    }
}
