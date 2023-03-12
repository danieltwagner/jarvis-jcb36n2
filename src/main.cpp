#include <Arduino.h>
#include <SoftwareSerial.h>

// RJ-45   Signal   Description
//  pin     ID
//   1      HS3     Handset control line 3
//   2      DTX     Serial control messages from controller to handset
//   3      GND
//   4      HTX     Serial control messages from handset to controller
//   5      VCC 5v
//   6      HS2     Handset control line 2
//   7      HS1     Handset control line 1
//   8      HS0     Handset control line 0

static constexpr int RJ45_DTX = D6; // Pin 2
static constexpr int RJ45_HTX = D7; // Pin 4

SoftwareSerial deskSerial(RJ45_DTX);
SoftwareSerial hsSerial(-1, RJ45_HTX);

void setup() {
  Serial.begin(115200);

  deskSerial.begin(9600);
  pinMode(RJ45_DTX, INPUT);

  hsSerial.begin(9600);
  pinMode(RJ45_HTX, INPUT);

  Serial.println("Ready.");
}

void send_msg(std::vector<byte> content, byte sender = 0xF1) {
  hsSerial.write(sender);
  hsSerial.write(sender);

  byte sum = 0;
  for(int c : content) {
    hsSerial.write(c);
    sum += c;
  }
  hsSerial.write(sum);
  hsSerial.write(0x7E);
}

void send(byte what) {
  pinMode(RJ45_HTX, OUTPUT);

  Serial.print("Requesting one step higher...");
  send_msg({0x01, 0x00});
  // delay(200);
  // send_msg({0x01, 0x00});
  // delay(200);
  // send_msg({0x01, 0x00});

  // Serial.print("Requesting one step lower...");
  // send_msg({0x02, 0x00});

  // Serial.print("Setting position 1...");
  // send_msg({0x03, 0x00});

  // Serial.print("Setting position 2...");
  // send_msg({0x04, 0x00});

  // Serial.print("Requesting position 1...");
  // send_msg({0x05, 0x00});

  // Serial.print("Requesting position 2...");
  // send_msg({0x06, 0x00});

  // Serial.println("Sending message 7...");
  // send_msg({0x07, 0x00});
  // These show up on RJ-11 and RJ-45
  // 10030 Desk: F2 F2 25 02  01 C3  EB 7E <- Memory height 1
  // 10051 Desk: F2 F2 26 02  01 23  4C 7E <- Memory height 2
  // 10072 Desk: F2 F2 27 02  00 FF  28 7E <- Memory height 3?
  // 10093 Desk: F2 F2 28 02  00 00  2A 7E <- Memory height 4 not set
  // These only show up on RJ-45
  // 10113 Desk: F2 F2 0E 01  01  10 7E    <- units = inches
  // 10120 Desk: F2 F2 19 01  00  1A 7E    <- memory mode = one-touch
  // 10128 Desk: F2 F2 17 01  01  19 7E    <- ? 17 = 1
  // 10135 Desk: F2 F2 1D 01  01  1F 7E    <- anti-collision sensitivity = high

  // Serial.println("Sending message 8...");
  // send_msg({0x08, 0x00});
  // 2015 Desk: F2 F2 05 02  FF FF          05 7E
  // or:
  // 9014 Desk: F2 F2 05 02  00 00          07 7E

  // Serial.println("Sending message 9...");
  // send_msg({0x09, 0x00});
  // 2014 Desk: F2 F2 06 01  01             08 7E

  // Serial.println("Sending message C...");
  // send_msg({0x0C, 0x00});
  // 13017 Desk: F2 F2 07 04  05 14 02 8A   B0 7E

  // Serial.println("Sending message 1C...");
  // send_msg({0x1C, 0x00});
  // 29013 Desk: F2 F2 1C 01  0A            27 7E

  // Serial.println("Sending message 1F...");
  // send_msg({0x1F, 0x00});
  // 32013 Desk: F2 F2 1F 01  00            20 7E

  // Serial.println("Sending message 20...");
  // send_msg({0x20, 0x00});
  // 33013 Desk: F2 F2 20 01  00            21 7E
  // or:
  // 32013 Desk: F2 F2 20 01  01            22 7E
  // 32034 Desk: F2 F2 21 02  01 54         78 7E

  // Serial.println("Sending message 21...");
  // send_msg({0x21, 0x00});
  // 34081 Desk: F2 F2 20 01  01            22 7E
  // 34102 Desk: F2 F2 21 02  01 54         78 7E

  // Serial.println("Sending message 22...");
  // send_msg({0x22, 0x00});
  // 35082 Desk: F2 F2 20 01  01            22 7E
  // 35103 Desk: F2 F2 21 02  01 54         78 7E

  // Serial.println("Sending message 23...");
  // send_msg({0x23, 0x00});
  // 36013 Desk: F2 F2 20 01  00            21 7E

  // Serial.println("Sending message 0x27...");
  // send_msg({0x27, 0x00});

  // Serial.println("Sending message 0x28...");
  // send_msg({0x28, 0x00});

  // Serial.printf("Sending message %x...\n", what);
  // send_msg({what, 0x00});

  pinMode(RJ45_HTX, INPUT);
}

std::vector<int> deskMsg, hsMsg;
long lastSend = 0;

void loop() {
  while (deskSerial.available()) {
    auto ch = deskSerial.read();
    deskMsg.push_back(ch);
    if(ch == 0x7e) {
      Serial.print(millis());
      Serial.print(" Desk:");
      for(int c : deskMsg) {
        Serial.printf(" %02X", c);
      }
      Serial.println();
      deskMsg.clear();
    }
  }

  while (hsSerial.available()) {
    auto ch = hsSerial.read();
    hsMsg.push_back(ch);
    if(ch == 0x7e) {
      Serial.print(millis());
      Serial.print(" HS:");
      for(int c : hsMsg) {
        Serial.printf(" %02X", c);
      }
      Serial.println();
      hsMsg.clear();
    }
  }

  if (millis() > 2000 && lastSend == 0) {
    send(0x20);
    lastSend = 1;
  }

  // long newSend = (int)(millis()/1000);
  // if (newSend > lastSend) {
  //   lastSend = newSend;
  //   send(newSend);
  // }
}