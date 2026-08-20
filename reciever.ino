#include <ArduinoJson.h>
#include <Adafruit_ST7735.h>

DynamicJsonDocument incoming(2048);

#define TFT_CS 4
#define TFT_DC 6
#define TFT_RST 9

#define GRAY tft.color565(28, 29, 41)
#define GREEN tft.color565(25, 87, 36)
#define YELLOW tft.color565(138, 145, 32)
#define RED tft.color565(148, 40, 40)
#define RED_TEXT tft.color565(255, 79, 79)

Adafruit_ST7735 tft(TFT_CS, TFT_DC, TFT_RST);

unsigned long lastDataTime = 0;
bool displayOff = false;

void display(float cpu, float ram, float gpu, String ram_per, String gpu_per, float cpu_t, float gpu_t){
  display_clear();
  tft.drawLine(0, 40, 160, 40, ST77XX_WHITE);
  tft.drawLine(0, 90, 160, 90, ST77XX_WHITE);
  tft.drawLine(60, 140, 60, 0, ST77XX_WHITE);

  tft.drawRect(65, 5, 90, 30, ST77XX_WHITE);
  tft.fillRect(66, 6, (int)(89 * cpu / 100.0 - 1), 28, color_change(cpu));

  tft.drawRect(65, 51, 90, 30, ST77XX_WHITE);
  tft.fillRect(66, 52, (int)(89 * gpu / 100.0 - 1), 28, color_change(gpu));

  tft.drawRect(65, 95, 90, 30, ST77XX_WHITE);
  tft.fillRect(66, 96, (int)(89 * ram / 100.0 - 1), 28, color_change(ram));

  tft.setCursor(5, 5);
  tft.print("CPU");
  tft.setCursor(5, 15);
  tft.print(cpu, 1);
  tft.print("%");
  tft.setCursor(5, 25);
  tft.setTextColor(text_color(cpu_t, "c"));
  tft.print(cpu_t, 1);
  tft.print("\xF7");
  tft.print("C");
  tft.setTextColor(ST77XX_WHITE);

  tft.setCursor(5, 48);
  tft.print("GPU");
  tft.setCursor(5, 58);
  tft.print(gpu, 1);
  tft.print("%");
  tft.setCursor(5, 68);
  tft.print(gpu_per);
  tft.setCursor(5, 78);
  tft.setTextColor(text_color(gpu_t, "g"));
  tft.print(gpu_t, 1);
  tft.print("\xF7");
  tft.print("C");
  tft.setTextColor(ST77XX_WHITE);

  tft.setCursor(5, 95);
  tft.print("RAM");
  tft.setCursor(5, 105);
  tft.print(ram, 1);
  tft.print("%");
  tft.setCursor(5, 115);
  tft.print(ram_per);
}

void display_clear(){
  tft.fillRect(5, 15, 55, 25, GRAY);
  tft.fillRect(65, 5, 90, 30, GRAY);
  tft.fillRect(5, 58, 55, 30, GRAY);
  tft.fillRect(65, 48, 90, 40, GRAY);
  tft.fillRect(5, 105, 55, 25, GRAY);
  tft.fillRect(65, 95, 90, 30, GRAY);
}

String data_change(float used, float total){
  String used_s = String(used);
  int idx = used_s.length();
  used_s.remove(idx - 1);
  String total_s = String(total);
  idx = total_s.length();
  total_s.remove(idx - 1);
  return used_s + "/" + total_s;
}

uint16_t color_change(float usage){
  if (usage < 60)
    return GREEN;
  else if (usage < 80)
    return YELLOW;
  else
    return RED;
}

uint16_t text_color(float temp, char p){
  if (p == "c"){
    if (temp < 90)
      return ST77XX_WHITE;
    else
      return RED_TEXT;
  }else{
    if (temp < 80)
      return ST77XX_WHITE;
    else
      return RED_TEXT;
  }
}

void setup(){
  Serial.begin(115200);
  Serial.setTimeout(2000);
  Serial.println("Initializing ST7735 display...");
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(GRAY);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(1);
  tft.setRotation(1);
  lastDataTime = millis();
  while (!Serial);
}

void loop(){
  if (!displayOff && millis() - lastDataTime >= 5000){
    tft.fillScreen(ST77XX_BLACK);
    displayOff = true;
  }

  if (!Serial.available()){
    return;
  }

  String json = Serial.readStringUntil('\n');
  incoming.clear();

  DeserializationError error = deserializeJson(incoming, json);

  if (error){
    Serial.print("JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  lastDataTime = millis();

  if (displayOff){
    tft.fillScreen(GRAY);
    displayOff = false;
  }

  float c = incoming["c"];
  float r = incoming["r"];
  float g = incoming["g"];
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
