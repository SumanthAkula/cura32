#include "cura32.h"

void set_led_task(void *args) {
    for (;;) {
		setLEDStrip2();
        led_strip_controller->showLeds(car.headlights_on() ? 32 : 255);
        vTaskDelay(33 / portTICK_RATE_MS);
    }
}

void setup() {
    Serial.begin(115200);
	pinMode(BUTTON_PIN, INPUT_PULLUP);
    car = Car();

    debug_led_controller = &FastLED.addLeds<SK6812, DEBUG_LED_PIN, GRB>(debug_led, 1);
    led_strip_controller = &FastLED.addLeds<WS2812B, LED_PIN>(ledsp, getRGBWsize(LED_COUNT));

    debug_led[0] = 0x00FF00;
    debug_led_controller->showLeds();
    vTaskDelay(1000 / portTICK_RATE_MS);
    debug_led[0] = 0x000000;
    debug_led_controller->showLeds();
    vTaskDelay(1000 / portTICK_RATE_MS);

    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)CAN_TX, (gpio_num_t)CAN_RX, TWAI_MODE_LISTEN_ONLY);
    g_config.tx_queue_len = 0;   // we don't want to transmit anything, so set the queue length to 0 to disable the queue
    g_config.rx_queue_len = 15;  // keep room for RX queue stuff so it doesnt start throwing errors cuz the queue is full
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // twai_filter_config_t f_config = twai_filter_config_t{
    //   .acceptance_code = (380 << 21),
    //   .acceptance_mask = ~(0x7FF << 21),
    //   .single_filter = true
    // };

    if (CanController::startCAN(g_config, t_config, f_config) != 0) {  // startCAN() returns 0 if success
        ESP_LOGI("Setup", "CAN Problem, CANnot continue...");
        while (true) {
        }
    }

    xTaskCreate(set_led_task, "LED Task", 4096, NULL, 1, NULL);

    startBT();

    ESP_LOGI("Setup", "setup finished...");
}

void startBT() {
    BLEDevice::init(DEVICE_NAME);
    server = BLEDevice::createServer();
    car_service = server->createService(CAR_SERVICE_UUID);
    can_frame_characteristic = car_service->createCharacteristic(CAN_FRAME_CHARACTERISTIC_UUID,
                                                                 BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);
    display_mode_characteristic = car_service->createCharacteristic(DISPLAY_MODE_CHARACTERISTIC_UUID,
                                                                    BLECharacteristic::PROPERTY_WRITE_NR | BLECharacteristic::PROPERTY_NOTIFY);

    server->setCallbacks(new ServerCallbacks());
    display_mode_characteristic->setCallbacks(new DisplayModeCharacteristicCallbacks());

    car_service->start();
    BLEAdvertising *pAdvertising = server->getAdvertising();
    pAdvertising->addServiceUUID(CAR_SERVICE_UUID);
    pAdvertising->start();
}

void fastLedSetProgress(const int value) {
    int numLEDsToFill = value / 256;
    int partialLEDValue = value % 256;

    for (int i = 0; i < LED_COUNT; i++) {
        if (i < numLEDsToFill) {
            // LED must be filled completely
            led_strip[i] = CRGBW(0, 255, 0, 0);
        } else if (i == numLEDsToFill) {
            // LED must be filled partially
            led_strip[i] = CRGBW(0, partialLEDValue, 0, 0);
        } else {
            // LED must be off
            led_strip[i] = CRGBW(0, 0, 0, 0);
        }
    }
}

int isBrakePressed(const twai_message_t message) {
    if (message.identifier != 380) {
        return 0;
    }

    return (message.data[4] & 0x01 == 1);
}

void setLEDStrip2() {
	led_strip_controller->clearLedData();
    // FastLED.clear();

    if (car.braking()) {
        for (int i = 0; i < LED_COUNT; i++) {
            led_strip[i].r = 0xFF;
        }
    } else {
        int pos = car.get_gas_pedal_position();
        pos = map(pos, 0, 255, 0, LED_MAX_PROGRESS);
        fastLedSetProgress(pos);
    }

    // turn the left and right LEDs orange if the indicators are on!!
    uint8_t turn_signals = car.get_turn_signals();
    if (turn_signals & 0x20) {  // check left signal
        led_strip[0] = CRGB::Orange;
    }

    if (turn_signals & 0x40) {
        led_strip[LED_COUNT - 1] = CRGB::Orange;
    }
}

// 600 microseconds
void ble_send_frame(twai_message_t message) {
    CanFrame frame(message);

    can_frame_characteristic->setValue((uint8_t *)&frame, sizeof(frame));
    can_frame_characteristic->notify();

    vTaskDelay(pdMS_TO_TICKS(5));
}

void loop() {
    uint32_t alerts;
    twai_read_alerts(&alerts, pdMS_TO_TICKS(25));
    twai_message_t message;
    if (alerts & TWAI_ALERT_RX_DATA) {
        while (twai_receive(&message, 0) == ESP_OK) {
            if (!CanController::should_handle_message(message)) {
                continue;
            }

            car.update_car_state(message);

            if (ble_connected) {
                ble_send_frame(message);
            }
        }
    }
}
