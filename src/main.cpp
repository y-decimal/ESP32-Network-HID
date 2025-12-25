#include <Arduino.h>
#include <EspNowHandler.h>
#include <FastLED.h>
#include <SharedTypes.h>
#include <WiFi.h>

#define RGB_LED_PIN 48
#define NUM_LEDS 1

CRGB leds[NUM_LEDS];
static EspNowHandler<DeviceID, PacketType> *espHandler;
static constexpr DeviceID selfID = DeviceID::KeyboardRight;
static constexpr DeviceID targetID = DeviceID::KeyboardLeft;
const uint8_t data = 0xAC;

const void callBack(const uint8_t *dataPtr, size_t len, DeviceID sender);

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, RGB_LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(25);
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  uint8_t selfMac[6] = {};
  WiFi.macAddress(selfMac);
  espHandler = new EspNowHandler<DeviceID, PacketType>(selfID, selfMac);
  leds[0] = CRGB::Yellow;
  FastLED.show();
  // espHandler->registry->deleteFlash();
  bool beginSuccess = espHandler->begin();
  bool registerSuccess = espHandler->registerComms(targetID, true);
  bool callbackSuccess =
      espHandler->registerCallback(PacketType::KeyDataHalf, callBack);
  if (registerSuccess && callbackSuccess && beginSuccess) {
    printf("ESP-NOW initialized successfully\n");
    leds[0] = CRGB::Green;
    FastLED.show();
  }
}

void loop() {

  bool success = espHandler->sendPacket(targetID, PacketType::KeyDataHalf,
                                        &data, sizeof(data));

  printf("Data sent: %d\n", success);
  delay(3000);
}

const void callBack(const uint8_t *dataPtr, size_t len, DeviceID sender) {
  printf("Data Received: %d\n", *dataPtr);
  CRGB prevColor = leds[0];
  leds[0] = CRGB::Blue;
  FastLED.show();
  delay(100);
  leds[0] = prevColor;
  FastLED.show();
}