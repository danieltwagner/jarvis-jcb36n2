#pragma once

#include "esphome/core/component.h"
#include "esphome/components/api/custom_api_device.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace jarvis_jcb36n2 {

class JarvisJcb36n2 : public Component, public api::CustomAPIDevice, public sensor::Sensor, public uart::UARTDevice {
 public:
  float get_setup_priority() const override { return setup_priority::LATE; }
  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_height_sensor(sensor::Sensor *sensor) { this->height_sensor_ = sensor; }
  void move_to_position(int position);

 protected:
  void decode_msg_(std::vector<uint8_t> msg);
  void send_msg_(std::vector<uint8_t> content);

  sensor::Sensor *height_sensor_{nullptr};
  std::vector<uint8_t> desk_msg_;
  float last_height_{0};
};

}  // namespace jarvis_jcb36n2
}  // namespace esphome