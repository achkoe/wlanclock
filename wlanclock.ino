/*
 * Hello World
 * Simply prints Hello World to the serial monitor.
 */

#include <Wire.h>
#include <U8g2lib.h>
#include "BME280I2C.h"
#include <OneWire.h>
#include <DallasTemperature.h>


uint8_t second = 0;
uint8_t hour = 0;
uint8_t minute = 0;
int8_t cnt = 0;
char time_str[16];
char bme_temperature_str[8];  // '-13.1°C'   -> maximum 7 characters
char bs_temperature_str[8];   // '-13.1°C'   -> maximum 7 characters
char bme_humidity_str[10];    // '100.0% RH' -> maximum 9 characters
char bme_pressure_str[10];    // '1234.5hPa' -> maximum 9 characters


// SH1106 ADC5/PC5/SCL, PC4/ADC4/SDA
// U8G2_SH1106_128X64_NONAME_F_SW_I2C(rotation, clock, data [, reset]) [full framebuffer, size = 1024 bytes]
U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2);
BME280I2C bme;

// Data wire is plugged into Arduino port 14 (D5)
#define ONE_WIRE_BUS 14

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


void setup() {
  // initialize serial communications at 9600 bps
  Serial.begin(9600);
  Wire.begin();
  u8g2.begin();
  sensors.begin();
  while (!bme.begin()) {
      delay(1000);
  }
}


void loop() {
  getBME280Data();
  getBS18B20Data();
  Serial.println(bme_temperature_str);
  Serial.println(bs_temperature_str);
  Serial.println(bme_humidity_str);
  Serial.println(bme_pressure_str);

  hour = 66;
  minute = 66;
  second = (second + 1) % 60;
  sprintf(time_str, "%02d:%02d:%02d", hour, minute, second);

  u8g2.firstPage();
  do {
      u8g2.setFont(u8g2_font_crox4hb_tn);
      u8g2.drawStr(1, 24, time_str);
      u8g2.setFont(u8g2_font_crox1cb_tf);

      if (cnt == 0) {
        u8g2.drawStr(0, 24 + 2 * 16, bme_temperature_str);
      } else if (cnt == 1) {
        u8g2.drawStr(0, 24 + 2 * 16, bs_temperature_str);
      } else if (cnt == 2) {
        u8g2.drawStr(0, 24 + 2 * 16, bme_humidity_str);
      } else if (cnt == 3) {
        u8g2.drawStr(0, 24 + 2 * 16, bme_pressure_str);
      }
  } while ( u8g2.nextPage() );

  cnt = cnt + 1 > 3 ? 0 : cnt + 1;

  delay(1000);
}


void getBME280Data() {
  float temp(NAN), hum(NAN), pres(NAN);

  BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
  BME280::PresUnit presUnit(BME280::PresUnit_hPa);

  bme.read(pres, temp, hum, tempUnit, presUnit);

  dtostrf(temp, 4, 1, bme_temperature_str);
  strcat(bme_temperature_str, "\xb0\x43");

  dtostrf(hum, 5, 1, bme_humidity_str);
  strcat(bme_humidity_str, "% RH");

  dtostrf(pres, 6, 1, bme_pressure_str);
  strcat(bme_pressure_str, "hPa");
}


void getBS18B20Data() {
  sensors.requestTemperatures(); // Send the command to get temperatures
  dtostrf(sensors.getTempCByIndex(0), 4, 1, bs_temperature_str);
  strcat(bs_temperature_str, "\xb0\x43");
}
