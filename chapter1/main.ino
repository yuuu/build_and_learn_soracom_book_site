#include <M5Stack.h>

#define LOOP_INTERVAL (10000)

void print_attendance(uint16_t distance) {
  M5.Lcd.fillRect(0, 0, 319, 40, BLACK);
  M5.Lcd.setTextSize(4);
  M5.Lcd.setCursor(0, 0);
  if (distance < 1000) {
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.println("Attended");
  } else {
    M5.Lcd.setTextColor(RED);
    M5.Lcd.println("Leaved");
  }
}

void print_distance(uint16_t distance) {
  M5.Lcd.fillRect(0, 140, 319, 239, BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 180);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("distance: %u[mm]", distance);
}

void setup() {
  Serial.begin(115200);
  Serial.println("begin setup.");
  M5.begin();
  M5.Power.begin();
  Wire.begin();
  Serial.println("end setup.");
}

void loop() {
  Serial.println("begin loop.");
  unsigned long start = millis();

  uint16_t distance = read_distance();
  if (distance != 20) {
    print_attendance(distance);
    print_distance(distance);
  }

  while((start + LOOP_INTERVAL) > millis()) {
    delay(1);
  }

  Serial.println("end loop.");
  Serial.println("");
}
