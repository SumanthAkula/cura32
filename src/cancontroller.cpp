#include "driver/twai.h"
#include "cancontroller.h"
#include <Arduino.h>

int CanController::startCAN(twai_general_config_t g_config, twai_timing_config_t t_config, twai_filter_config_t f_config) {
  /*
  0001 0111 1100 XXXX XXXX XXXX XXXX XXXX <- (380 << 21)
  0001 0101 0110 XXXX XXXX XXXX XXXX XXXX <- (342 << 21)
  0001 0101 1000 XXXX XXXX XXXX XXXX XXXX <- (344 << 21)
  1000 0010 1110 1111 1111 1111 1111 1111

  0001 0111 1100 <- 380
  0001 0101 0110 <- 342
  0001 0101 1000 <- 344
  1000 0010 1110
  */

  // twai_filter_config_t f_config = twai_filter_config_t{
  //   .acceptance_code = (380 << 21),
  //   .acceptance_mask = ~(0x7FF << 21),
  //   .single_filter = true
  // };

  if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
    return 1;
  }

  // Start TWAI driver
  if (twai_start() != ESP_OK) {
    return 2;
  }

  // Reconfigure alerts to detect frame receive, Bus-Off error and RX queue full states
  uint32_t alerts_to_enable = TWAI_ALERT_RX_DATA | TWAI_ALERT_ERR_PASS | TWAI_ALERT_BUS_ERROR | TWAI_ALERT_RX_QUEUE_FULL;
  if (twai_reconfigure_alerts(alerts_to_enable, NULL) != ESP_OK) {
    return 3;
  }

  return 0;
}

void CanController::serialSendFrame(CanFrame frame) {
  Serial.printf("%d,%d", frame.get_id(), frame.get_dlc());
  for (int i = 0; i < frame.get_dlc(); i++) {
    Serial.printf(",%02X", frame.get_data()[i]);
  }
  Serial.println();
}

CanFrame CanController::genRandRPMFrame() {
  uint8_t throttle = random(0, 256);
  uint8_t rpm2 = random(0x00, 0x1A);
  uint8_t rpm3 = random(0x10, 0x2C);
  uint8_t data[] = { throttle, 0, rpm2, rpm3, 0, 0, 0, 0 };

  CanFrame frame(380, 8, data);

  return frame;
}