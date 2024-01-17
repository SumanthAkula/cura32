#ifndef CAR_H
#define CAR_H

#include "config.h"
#include "driver/twai.h"

class Car {
   private:
    uint16_t rpm;
    int16_t steer_angle;
    uint8_t gas_pedal_position;
    uint8_t speed;
    uint8_t gear;
    uint8_t turn_signals;
    bool brake;
	bool headlights;

   public:
    Car() {
        this->rpm = 0;
        this->steer_angle = 0;
        this->gas_pedal_position = 0;
        this->speed = 0;
        this->gear = 0;
        this->turn_signals = 0;
        this->brake = false;
		this->headlights = false;
    }

    bool braking() {
        return this->brake;
    }

	bool headlights_on() {
		return this->headlights;
	}

    uint8_t get_gas_pedal_position() {
        return this->gas_pedal_position;
    }

    uint8_t get_turn_signals() {
        return this->turn_signals;
    }

    void update_car_state(twai_message_t message);
};

#endif
