#ifndef CAR_H
#define CAR_H

#include "driver/twai.h"
#include "config.h"

class Car {
private:
  uint16_t rpm;
  int16_t steer_angle;
  uint8_t gas_pedal_position;
  uint8_t speed;
  uint8_t gear;
  uint8_t turn_signals;
  bool brake;

public:
  Car() {
	this->rpm = 0;
	this->steer_angle = 0;
	this->gas_pedal_position = 0;
	this->speed = 0;
	this->gear = 0;
	this->turn_signals = 0;
	this->brake = false;
  }

  bool braking() {
    return this->brake;
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
