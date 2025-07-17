#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pins
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
int counter = 0;

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
  display.println("LoRa TX Ready");
  display.display();
  delay(2000);
}

void loop() {
  String msg = "Ping " + String(counter);
  LoRa.beginPacket();
  LoRa.print(msg);
  LoRa.endPacket();

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Role: TX");
  display.setCursor(0, 20);
  display.print("Sent: ");
  display.println(msg);
  display.display();

  Serial.println("Sent: " + msg);
  counter++;
  delay(2000);
}
