#include <ArduinoJson.h>
#include <Adafruit_ST7735.h>


DynamicJsonDocument incoming(2048);

#define TFT_CS   6
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);

  Serial.println("Initializing ST7735 display...");

  tft.initR(INITR_BLACKTAB); 
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setRotation(1);

  while (!Serial);
}


void loop() {

  if (!Serial.available()) {
    return;
  }

  String json = Serial.readStringUntil('\n');

  incoming.clear();

  DeserializationError error = deserializeJson(incoming, json);

  if (error) {
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  float c  = incoming["c"];
  float r  = incoming["r"];
  float g  = incoming["g"];
  float ru = incoming["ru"];
  float rt = incoming["rt"];
  float gu = incoming["gu"];
  float gt = incoming["gt"];
  float tg = incoming["tg"];
  float tc = incoming["tc"];

  Serial.flush();

  display(c, r, g);
}

void display(float cpu, float ram, float gpu){
  tft.fillRect(0, 0, 128, 160, ST77XX_BLACK);

  tft.setCursor(10, 10);
  tft.print("CPU: ");
  tft.print(cpu);
  tft.print("%");

  tft.setCursor(10, 20);
  tft.print("RAM: ");
  tft.print(ram);
  tft.print("%");
  
  tft.setCursor(10, 30);
  tft.print("GPU: ");
  tft.print(gpu);
  tft.print("%");
}
