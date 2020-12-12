#include <M5Stack.h>

#define TINY_GSM_MODEM_UBLOX
#include <TinyGsmClient.h> 

TinyGsm modem(Serial2);
TinyGsmClient ctx(modem);

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

void setup_modem() {
  M5.Lcd.print("modem.restart()");
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  modem.restart();
  M5.Lcd.println("done");

  M5.Lcd.print("getModemInfo:");
  String modemInfo = modem.getModemInfo();
  M5.Lcd.println(modemInfo);

  M5.Lcd.print("waitForNetwork()");
  while (!modem.waitForNetwork()) M5.Lcd.print(".");
  M5.Lcd.println("Ok");

  M5.Lcd.print("gprsConnect(soracom.io)");
  modem.gprsConnect("soracom.io", "sora", "sora");
  M5.Lcd.println("done");

  M5.Lcd.print("isNetworkConnected()");
  while (!modem.isNetworkConnected()) M5.Lcd.print(".");
  M5.Lcd.println("Ok");

  M5.Lcd.print("My IP addr: ");
  IPAddress ipaddr = modem.localIP();
  M5.Lcd.print(ipaddr);
  delay(2000);
  M5.Lcd.fillScreen(BLACK);
}

void send_distance(uint16_t distance) {
  if (!ctx.connect("uni.soracom.io", 23080)) {
    Serial.println("Connect failed.");
    return;
  }
  Serial.println("connected.");

  ctx.write((const uint8_t*)&distance, sizeof(distance));
  Serial.println("sent.");

  char buf[1 * 1024] = {0};
  ctx.readBytes(buf, sizeof(buf));
  ctx.stop();
  Serial.println(buf);
}

void setup() {
  Serial.begin(115200);
  Serial.println("begin setup.");
  M5.begin();
  M5.Power.begin();
  Wire.begin();
  setup_modem();
  Serial.println("end setup.");
}

void loop() {
  Serial.println("begin loop.");
  unsigned long start = millis();

  uint16_t distance = read_distance();
  if (distance != 20) {
    print_attendance(distance);
    print_distance(distance);
    send_distance(distance);
  }

  while((start + LOOP_INTERVAL) > millis()) {
    delay(1);
  }

  Serial.println("end loop.");
  Serial.println("");
}
