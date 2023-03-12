#include "jarvis_jcb36n2.h"
#include "esphome/core/log.h"

namespace esphome {
namespace jarvis_jcb36n2 {

static const char *TAG = "jarvis_jcb36n2";

void JarvisJcb36n2::send_msg_(std::vector<uint8_t> content) {
  this->write_byte(0xF1);
  this->write_byte(0xF1);

  uint8_t sum = 0;
  for(int c : content) {
    this->write_byte(c);
    sum += c;
  }
  this->write_byte(sum);
  this->write_byte(0x7E);
}

void JarvisJcb36n2::move_to_position(int position) {
    // mmm, should this just be a map?
    switch (position)
    {
    case 1:
        this->send_msg_({0x05, 0x00});
        break;

    case 2:
        this->send_msg_({0x06, 0x00});
        break;

    case 3:
        this->send_msg_({0x27, 0x00});
        break;

    case 4:
        this->send_msg_({0x28, 0x00});
        break;

    default:
        ESP_LOGE(TAG, "Asked to move to invalid position %d", position);
        break;
    }
}

void JarvisJcb36n2::decode_msg_(std::vector<uint8_t> msg) {
    // check sender, end byte, and checksum
    if (msg[0] != 0xF2 || msg[1] != 0xF2 || msg[msg.size() - 1] != 0x7E) {
        ESP_LOGD(TAG, "Message is malformed.");
        return;
    }

    uint8_t sum = 0;
    for (std::vector<uint8_t>::size_type i = 2; i < msg.size() - 2; i++) {
        sum += msg[i];
    }
    if (sum != msg[msg.size() - 2]) {
        ESP_LOGD(TAG, "Message has bad checksum.");
        return;
    }

    if (msg[2] == 0x01) {
        // Height: F2 F2 01 03 (01 23) 0F 37 7E
        float height = (msg[4] << 8) + msg[5];

        if (height <= 530) {
            // The desk is reporting inches, convert to cm.
            // ideally we wouldn't hard code this 53" cutoff... the desk can tell us about
            // physical minimum/maximum height in mm, but this works for now.
            height *= 0.254;
        } else {
            height /= 10; // mm to cm
        }
        ESP_LOGD(TAG, "Desk height is now %d cm", height);

        if (this->height_sensor_ != nullptr && height != this->last_height_) {
            this->height_sensor_->publish_state(height);
            this->last_height_ = height;
        }
    }
}

void JarvisJcb36n2::setup() {
    register_service(&JarvisJcb36n2::move_to_position, "move_to_position", {"position"});
    ESP_LOGI(TAG, "Requesting settings from desk...");
    this->send_msg_({0x07, 0x00});
}

void JarvisJcb36n2::loop() {
  while (this->available()) {
    auto ch = this->read();
    this->desk_msg_.push_back(ch);
    if(ch == 0x7e) {  // end of message
      char msg[this->desk_msg_.size() * 3];
      int msgPtr = 0;
      for(int c : this->desk_msg_) {
        sprintf(msg + msgPtr, " %02X", c);
        msgPtr += 3;
      }
      ESP_LOGD(TAG, "Desk: %s", msg);
      decode_msg_(this->desk_msg_);
      this->desk_msg_.clear();
    }
  }
}

void JarvisJcb36n2::dump_config() {
  ESP_LOGCONFIG(TAG, "Jarvis desk:");
  LOG_SENSOR(TAG, "Height", this->height_sensor_);
}

}  // namespace jarvis_jcb36n2
}  // namespace esphome