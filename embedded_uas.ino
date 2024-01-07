#include <DHT.h>
#include <MQ135.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define DHTTYPE DHT22
#define buzzer D5
uint8_t DHTPin = D4;
DHT dht(DHTPin, DHTTYPE);
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
float temperatureCorrection = -3.0;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define AMONIA_THRESHOLD 85 // Ambang deteksi gas amonia (ppm)

String getTemperatureStatus(float suhu) {
  if (suhu <= 15) return "berbahaya";
  if (suhu > 15 && suhu < 20) return "kurang baik";
  if (suhu >= 20 && suhu < 26) return "baik";
  if (suhu >= 26 && suhu < 30) return "kurang baik";
  if (suhu >= 30) return "berbahaya";
  return "tidak diketahui";
}

void setup() {
  Serial.begin(9600);
  pinMode(DHTPin, INPUT);
  pinMode(buzzer, OUTPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.display();  // Make sure to call display() after clearing
  display.setTextColor(WHITE);

  dht.begin();
}

void loop() {
  float suhu = dht.readTemperature() + temperatureCorrection;
  float humi = dht.readHumidity();
  String tempStatus = getTemperatureStatus(suhu);

  int mq135 = analogRead(0);
  Serial.print("Gas=");
  Serial.print(mq135, DEC);
  Serial.println(" ppm");

  Serial.print("Temperature = ");
  Serial.print(suhu);
  Serial.println(tempStatus);

  Serial.print("Kelembapan = ");
  Serial.println(String(humi) + "%");

  if (tempStatus == "berbahaya" || mq135 >= AMONIA_THRESHOLD) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("GAS: "));
  display.print(mq135);
  display.print(F(" ppm"));
  display.setCursor(0, 16);
  display.print(F("Temperature: "));
  display.print(suhu);
  display.print(F("°C"));
  display.print(tempStatus);
  display.setCursor(0, 32);
  display.print(F("Humidity: "));
  display.print(humi);
  display.print(F("%"));
  display.display();

  delay(2000);
}
