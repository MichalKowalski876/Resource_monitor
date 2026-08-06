#include <ArduinoJson.h>
#include <Adafruit_ST7735.h>

DynamicJsonDocument incoming(2048);

#define TFT_CS   6
#define TFT_DC   9
#define TFT_RST  8

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

void display(float cpu, float ram, float gpu, String ram_per, String gpu_per, float cpu_t, float gpu_t){
  display_clear();

  tft.drawLine(0, 40, 160, 40, ST77XX_WHITE);
  tft.drawLine(0, 90, 160, 90, ST77XX_WHITE);
  tft.drawLine(60, 140, 60, 0, ST77XX_WHITE);

  tft.fillRect(65, 5, 90 *(cpu/100), 30, ST77XX_GREEN);
  tft.fillRect(65, 48, 90 *(gpu/100), 30, ST77XX_GREEN);
  tft.fillRect(65, 95, 90 *(ram/100), 30, ST77XX_GREEN);

  tft.setCursor(5, 5);
  tft.print("CPU");
  tft.setCursor(5, 15);
  tft.print(cpu, 1);
  tft.print("%");
  tft.setCursor(5, 25);
  tft.print(cpu_t, 1);
  tft.print("\xF7");
  tft.print("C");


  tft.setCursor(5, 48);
  tft.print("GPU");
  tft.setCursor(5, 58);
  tft.print(gpu, 1);
  tft.print("%");
  tft.setCursor(5, 68);
  tft.print(gpu_per);
  tft.setCursor(5, 78);
  tft.print(gpu_t, 1);
  tft.print("\xF7");
  tft.print("C");

  tft.setCursor(5, 95);
  tft.print("RAM");
  tft.setCursor(5, 105);
  tft.print(ram, 1);
  tft.print("%");
  tft.setCursor(5, 115);
  tft.print(ram_per);

}

void display_clear(){
  //CPU
  tft.fillRect(5, 15, 55, 25, ST77XX_BLACK);
  tft.fillRect(65,5,90,30, ST77XX_BLACK);
  //GPU
  tft.fillRect(5,58, 55, 30, ST77XX_BLACK);
  tft.fillRect(65, 48, 90, 30, ST77XX_BLACK);
  //RAM
  tft.fillRect(5, 105, 55, 25, ST77XX_BLACK);
  tft.fillRect(65, 95, 90, 30, ST77XX_BLACK);
}

String data_change(float used, float total){
  String used_s = String(used);
  int idx = used_s.length();
  used_s.remove(idx-1);

  String total_s = String(total);
  idx = total_s.length();
  total_s.remove(idx-1);

  String output = used_s + "/" + total_s;

  return output;
}

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
  String ram_per = data_change(ru, rt);

  float gu = incoming["gu"];
  float gt = incoming["gt"];
  String gpu_per = data_change(gu, gt);

  float tg = incoming["tg"];
  float tc = incoming["tc"];

  Serial.flush();

  display(c, r, g, ram_per, gpu_per, tc, tg);
  
}
