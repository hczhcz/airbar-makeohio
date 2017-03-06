#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

void setup() {
  Wire.begin();

  Serial.begin(38400);

  accelgyro.initialize();
}

void loop() {
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  Serial.write((uint8_t *) &gy, sizeof(gy));
}

