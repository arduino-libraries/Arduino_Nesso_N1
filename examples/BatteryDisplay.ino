/*
  Arduino Nesso N1 - Display Battery Status

  This example will enable battery charging and display its charging state.

  created: December 11 2025
  by: Ubi de Feo

  This example code is in the public domain.
*/

#include <Arduino_Nesso_N1.h>
NessoBattery battery;
NessoDisplay display;

const int DISPLAY_WIDTH = 240;
const int DISPLAY_HEIGHT = 135;
const uint16_t COLOR_TEAL = 0x0410;
const uint16_t COLOR_BLACK = 0x0000;
const uint16_t COLOR_GREEN = 0x1e85;
const uint16_t COLOR_ORANGE = 0xed03;
const uint16_t COLOR_RED = 0xe841;
const int ANIMATION_DELAY = 30;
const int COLS = 20;
const int ROWS = 1;
const int REGION_WIDTH = 12;
const int REGION_HEIGHT = 135;


LGFX_Sprite statusSprite(&display);

float batteryVoltage = 0.0;
int counter = 0;
char uptimeString[26];
bool ledStatus = false;
unsigned long lastLEDflip = 0;
int progressEdge = 240;
bool progressExpanding = true;

void setup() {
  Serial.begin(115200);

  display.begin();

  battery.begin();
  battery.enableCharge();

  pinMode(LED_BUILTIN, OUTPUT);

  display.setRotation(1);
  display.setEpdMode(epd_mode_t::epd_fastest);

  display.fillScreen(TFT_WHITE);
  display.setTextColor(COLOR_TEAL);
  display.setTextSize(5);
  display.drawString("Nesso N1", 6, 11);

  statusSprite.createSprite(240, 81);
  lastLEDflip = millis();
}

void loop() {
  unsigned long msNow = millis();

  float chargeLevel = battery.getChargeLevel();
  batteryVoltage = battery.getVoltage();

  char batteryStatusTicker[16];
  sprintf(batteryStatusTicker, "%4.2f %6.2f%%", batteryVoltage, chargeLevel);
  if (msNow - lastLEDflip > 1000) {
    ledStatus = !ledStatus;
    // LED_BUILTIN currently disabled for failures
    digitalWrite(LED_BUILTIN, ledStatus);
    lastLEDflip = msNow;
    Serial.print(batteryVoltage);
    Serial.print(" ");
    Serial.print(chargeLevel);
    Serial.println("%");
    sprintf(uptimeString, "uptime:\n%012d\n", millis() / 1000);
    Serial.println(uptimeString);
  }
  renderstatusSprite();
}

void renderstatusSprite() {
  int offsetY = 18;
  statusSprite.fillSprite(TFT_WHITE);
  statusSprite.setColor(COLOR_ORANGE);
  if (progressExpanding) {
    statusSprite.fillRect(progressEdge, 0, 240, 8);
  } else {
    statusSprite.fillRect(0, 0, progressEdge, 8);
  }

  progressEdge -= 1;
  if (progressEdge <= 0) {
    progressEdge = 240;
    progressExpanding = !progressExpanding;
  }
  statusSprite.setTextSize(3);
  statusSprite.setTextColor(COLOR_BLACK);
  statusSprite.drawString("Battery:", 6, offsetY);
  uint16_t textColor = 0x0000;
  if (batteryVoltage > 3.7) {
    textColor = COLOR_GREEN;
  } else if (batteryVoltage <= 3.7 && batteryVoltage >= 3.3) {
    textColor = COLOR_ORANGE;
  } else {
    textColor = COLOR_RED;
  }
  char batteryString[6];
  sprintf(batteryString, "%4.2f", batteryVoltage);
  statusSprite.setTextColor(textColor);
  statusSprite.drawString(batteryString, 165, offsetY);
  statusSprite.setTextColor(COLOR_BLACK);
  statusSprite.setTextSize(2);

  statusSprite.setTextColor(COLOR_TEAL);
  statusSprite.drawString(uptimeString, 6, offsetY + 38);
  statusSprite.pushSprite(0, 54);
  //
}
