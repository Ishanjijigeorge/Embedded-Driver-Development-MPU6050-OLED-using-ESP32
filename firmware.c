#include <Wire.h>
#include <math.h>

#define OLED_ADDR 0x3C

  void mpu_init() {
  Wire.beginTransmission(0x68);
  Wire.write(0x6B);   // Power management register
  Wire.write(0x00);   // Wake up MPU6050
  Wire.endTransmission();
}

void read_mpu(int16_t &ax, int16_t &ay, int16_t &az) {
  Wire.beginTransmission(0x68);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);

  Wire.requestFrom(0x68, 6, true);

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

uint8_t mpu_whoami() {
  Wire.beginTransmission(0x68);
  Wire.write(0x75);          // WHO_AM_I register
  Wire.endTransmission(false);

  Wire.requestFrom(0x68, 1, true);
  return Wire.read();
}

void oled_cmd(uint8_t cmd) {
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x00);   // Command mode
  Wire.write(cmd);
  Wire.endTransmission();
}

void oled_data(uint8_t data) {
  Wire.beginTransmission(OLED_ADDR);
  Wire.write(0x40);   // Data mode
  Wire.write(data);
  Wire.endTransmission();
}

void oled_init() {
  oled_cmd(0xAE); // Display OFF

  oled_cmd(0xA8); oled_cmd(0x3F); // Multiplex ratio (64)
  oled_cmd(0xD3); oled_cmd(0x00); // Display offset
  oled_cmd(0x40);                // Start line
  oled_cmd(0xA1);                // Segment remap
  oled_cmd(0xC8);                // COM scan direction
  oled_cmd(0xDA); oled_cmd(0x12); // COM pins
  oled_cmd(0x81); oled_cmd(0x7F); // Contrast
  oled_cmd(0xA4);                // Display RAM
  oled_cmd(0xA6);                // Normal display
  oled_cmd(0xD5); oled_cmd(0x80); // Clock divide
  oled_cmd(0x8D); oled_cmd(0x14); // Charge pump

  oled_cmd(0xAF); // Display ON
}

void oled_set_cursor(uint8_t page, uint8_t col) {
  oled_cmd(0xB0 + page);              // Set page address
  oled_cmd(0x00 + (col & 0x0F));      // Lower column
  oled_cmd(0x10 + (col >> 4));        // Upper column
}

void oled_clear() {
  for (uint8_t page = 0; page < 8; page++) {
    oled_set_cursor(page, 0);
    for (uint8_t col = 0; col < 128; col++) {
      oled_data(0x00);
    }
  }
}

const uint8_t font5x7[][5] = {
  // '0'
  {0x3E,0x51,0x49,0x45,0x3E},
  // '1'
  {0x00,0x42,0x7F,0x40,0x00},
  // '2'
  {0x42,0x61,0x51,0x49,0x46},
  // '3'
  {0x21,0x41,0x45,0x4B,0x31},
  // '4'
  {0x18,0x14,0x12,0x7F,0x10},
  // '5'
  {0x27,0x45,0x45,0x45,0x39},
  // '6'
  {0x3C,0x4A,0x49,0x49,0x30},
  // '7'
  {0x01,0x71,0x09,0x05,0x03},
  // '8'
  {0x36,0x49,0x49,0x49,0x36},
  // '9'
  {0x06,0x49,0x49,0x29,0x1E},
  // '.'
  {0x00,0x60,0x60,0x00,0x00},
  // '-'
  {0x08,0x08,0x08,0x08,0x08},
  // 'X'
  {0x63,0x14,0x08,0x14,0x63},
  // 'Y'
  {0x07,0x08,0x70,0x08,0x07},
  // 'Z'
  {0x61,0x51,0x49,0x45,0x43},
  // 'A'
  {0x7E,0x11,0x11,0x11,0x7E},
  // ':'
  {0x00,0x36,0x36,0x00,0x00}
};


void oled_draw_char(char c) {
  uint8_t i;
  if (c >= '0' && c <= '9') i = c - '0';
  else if (c == '.') i = 10;
  else if (c == '-') i = 11;
  else if (c == 'X') i = 12;
  else if (c == 'Y') i = 13;
  else if (c == 'Z') i = 14;
  else if (c == 'A') i = 15;
  else if (c == ':') i = 16;
  else return;

  for (int j = 0; j < 5; j++) oled_data(font5x7[i][j]);
  oled_data(0x00);
}

void oled_print(const char *s) {
  while (*s) oled_draw_char(*s++);
}

void oled_draw_bubble(int pos) {
  oled_set_cursor(7, 0);
  for (int i = 0; i < 128; i++) oled_data(0x00);
  oled_set_cursor(7, pos);
  oled_data(0xFF);
}

float angle_filt = 0;
unsigned long last_time = 0;

void setup() {
  Wire.begin(21, 22);
  mpu_init();
  oled_init();
  oled_clear();

  uint8_t id = mpu_whoami();
  if (id != 0x68) {
    oled_set_cursor(0, 0);
    oled_print("MPU FAIL");
    while (1);   // Stop execution
  }
}


void loop() {

  if (millis() - last_time < 40) return;
  last_time = millis();

  int16_t ax, ay, az;
  read_mpu(ax, ay, az);

  float xg = ax / 16384.0;
  float yg = ay / 16384.0;
  float zg = az / 16384.0;

  float angle_raw = atan2(xg, zg) * 57.3;
  angle_filt = 0.9 * angle_filt + 0.1 * angle_raw;

  float angle = angle_filt;
  if (angle > 45) angle = 45;
  if (angle < -45) angle = -45;

  int pos = map(angle, -45, 45, 0, 127);

  char buf[10];

  oled_set_cursor(0, 0);
  oled_print("X:");
  dtostrf(xg, 4, 2, buf);
  oled_print(buf);

  oled_set_cursor(2, 0);
  oled_print("Y:");
  dtostrf(yg, 4, 2, buf);
  oled_print(buf);

  oled_set_cursor(4, 0);
  oled_print("Z:");
  dtostrf(zg, 4, 2, buf);
  oled_print(buf);

  oled_set_cursor(6, 0);
  oled_print("A:");
  dtostrf(angle, 4, 1, buf);
  oled_print(buf);

  oled_draw_bubble(pos);
}
