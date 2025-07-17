#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCK     5
#define MISO    19
#define MOSI    27
#define SS      18
#define RST     14
#define DIO0    26
#define BAND    866E6

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

float estimateDistance(int rssi) {
  float txPower = -70;  // assumed RSSI at 1 meter
  float n = 2.0;         // path-loss exponent
  float dist = pow(10.0, (txPower - rssi) / (10 * n));
  if (dist > 10) dist = 10;
  if (dist < 1) dist = 1;
  return dist;
}

void setup() {
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW); delay(20); digitalWrite(OLED_RST, HIGH);

  Wire.begin(OLED_SDA, OLED_SCL);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay(); display.setTextColor(WHITE); display.setTextSize(1);

  Serial.begin(115200);
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    display.println("LoRa failed!"); display.display();
    while (1);
  }

  display.setCursor(0, 0);
  display.println("LoRa RX Ready");
  display.display();
  delay(2000);
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String msg = "";
    while (LoRa.available()) {
      msg += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();
    float distance = estimateDistance(rssi);

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Role: RX");
    display.setCursor(0, 15);
    display.print("Msg: ");
    display.println(msg);
    display.setCursor(0, 30);
    display.print("RSSI: ");
    display.print(rssi);
    display.setCursor(0, 45);
    display.print("Est Dist: ");
    display.print(distance, 1);
    display.println(" m");
    display.display();

    Serial.print("Received: ");
    Serial.print(msg);
    Serial.print(" | RSSI: ");
    Serial.print(rssi);
    Serial.print(" | Distance: ");
    Serial.println(distance);
  }
}
