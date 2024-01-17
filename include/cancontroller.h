#ifndef CANCONTROLLER_H
#define CANCONTROLLER_H

#include <sys/_stdint.h>

#include <cstring>

#include "driver/twai.h"

#define CAN_RX 4
#define CAN_TX 5

// CAN frame data:
// HEX: 00       00       00       00       00       00       00       00
// BIN: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
// BIT: 7      0 15     8 23    16 31    24 39    32 47    40 55    48 63    56
//      reading this is a bit weird.
//      SG_ ENGINE_RPM : 23|16@0+ (1,0) [0|15000] "rpm" EON       => Read from bytes 2 and 3 (Big Endian). value is between 0 and 15000. unsigned
//      SG_ STEER_ANGLE:  7|16@0- (-0.1)[-500|500] "degrees" EON  => Read from bytes 0 and 1 (Big Endian). value is between -500 and 500 after scaling. signed

// HEX: 00       00       00       00       00       00       00       00
// BIN: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
// BIT: 7      0 15     8 23    16 31    24 39    32 47    40 55    48 63    56

// struct can_frame {
//   uint32_t can_id;
//   uint8_t can_dlc;
//   uint8_t data[8] __attribute__((aligned(8)));
// };

class CanFrame {
   private:
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];

   public:
    CanFrame(twai_message_t message) {
        this->id = message.identifier;
        this->dlc = message.data_length_code;
        memcpy(this->data, message.data, message.data_length_code);
    }

    CanFrame(uint32_t id, uint8_t dlc, uint8_t data[]) {
        this->id = id;
        this->dlc = dlc;
        memcpy(this->data, data, dlc);
    }

    uint32_t get_id() {
        return this->id;
    }

    uint8_t get_dlc() {
        return this->dlc;
    }

    uint8_t *get_data() {
        return this->data;
    }
};

class CanController {
   public:
    /**
     * @brief Start the CAN system
     * @return
     *    - 0 if success
     *    - 1 if driver error
     *    - 2 if start error
     *    - 3 if alerts error
     */
    static int startCAN(twai_general_config_t g_config, twai_timing_config_t t_config, twai_filter_config_t f_config);

    /**
     * @brief Prints a CAN frame to the Serial monitor.
     *
     * This is useful along with the utility from @link
     */
    static void serialSendFrame(CanFrame frame);
    static CanFrame genRandRPMFrame();
};

#endif
