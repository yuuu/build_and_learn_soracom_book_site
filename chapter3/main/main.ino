#include <M5Stack.h>

#define TINY_GSM_MODEM_UBLOX
#include <TinyGsmClient.h> 

TinyGsm modem(Serial2);
TinyGsmClient ctx(modem);

#include <Arduino_JSON.h>
#include <PubSubClient.h>
PubSubClient MqttClient(ctx);
const char *THING_NAME = "tof";
const char *PUB_TOPIC = "distances";
const char *SUB_TOPIC = "messages";

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
  char payload[512];
  sprintf(payload, "{\"distance\":%u}", distance);
  char topic[256];
  sprintf(topic, "%s/%s", PUB_TOPIC, modem.getIMSI().c_str());
  Serial.print("topic>");
  Serial.println(topic);
  MqttClient.publish(topic, payload);
  Serial.println(payload);
  Serial.println("sent.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("subscribed.");

  String buf_t = String(topic);
  payload[length] = '\0'; /* https://hawksnowlog.blogspot.com/2017/06/convert-byte-array-to-string.html */
  String buf_p = String((char*) payload);

  Serial.print("Incoming topic: ");
  Serial.println(buf_t);
  Serial.print("Payload>");
  Serial.println(buf_p);

  JSONVar json = JSON.parse((char*) payload);
  if(json.hasOwnProperty("message")){
    M5.Lcd.fillRect(0, 70, 319, 90, BLACK);
    M5.Lcd.setTextSize(3);
    M5.Lcd.setCursor(0, 100);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.println((const char*)json["message"]);
  }
}

void setup_mqtt() {
  Serial.print("ThingName(mqtt_id): ");
  Serial.println(THING_NAME);
  MqttClient.setServer("beam.soracom.io", 1883);
  MqttClient.setCallback(callback);
  if (!MqttClient.connect(THING_NAME)) {
    Serial.println(MqttClient.state());
  }
  char topic[256];
  sprintf(topic, "%s/%s", SUB_TOPIC, modem.getIMSI().c_str());
  Serial.print("topic>");
  Serial.println(topic);
  MqttClient.subscribe(topic);
}

void check_connection() {
  if (!MqttClient.connected()) {
    // disconnect
    MqttClient.disconnect();
    modem.restart();

    // connect
    setup_modem();
    setup_mqtt();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("begin setup.");
  M5.begin();
  M5.Power.begin();
  Wire.begin();
  setup_modem();
  setup_mqtt();
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
    MqttClient.loop();
    delay(1);
  }

  Serial.println("end loop.");
  Serial.println("");
}
