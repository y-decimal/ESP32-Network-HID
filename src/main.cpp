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
const uint8_t ping = 0xAA;
const uint8_t pong = 0xFF;

#define ROW_1_PIN 9
#define ROW_2_PIN 10
#define COL_1_PIN 17
#define COL_2_PIN 18

static constexpr int rowPins[2] = {ROW_1_PIN, ROW_2_PIN};
static constexpr int colPins[2] = {COL_1_PIN, COL_2_PIN};

volatile long unsigned lastSendTime = 0;
unsigned long lastKeyTime = 0;

const void pingCallback(const uint8_t *dataPtr, size_t len, DeviceID sender);
const void pongCallback(const uint8_t *dataPtr, size_t len, DeviceID sender);
const void keyDataCallback(const uint8_t *dataPtr, size_t len, DeviceID sender);
uint8_t *keyDataBuffer = nullptr;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812, RGB_LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(25);

  pinMode(COL_1_PIN, INPUT_PULLUP);
  pinMode(COL_2_PIN, INPUT_PULLUP);
  // Idle rows float high via pull-up to avoid sneaky current paths.
  pinMode(ROW_1_PIN, INPUT_PULLUP);
  pinMode(ROW_2_PIN, INPUT_PULLUP);

  leds[0] = CRGB::Red;
  FastLED.show();
  delay(2000);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  uint8_t selfMac[6] = {};
  WiFi.macAddress(selfMac);
  espHandler = new EspNowHandler<DeviceID, PacketType>(selfID, selfMac);
  espHandler->registry->deleteFlash(); // For testing, clear stored devices
  leds[0] = CRGB::Yellow;
  FastLED.show();
  // espHandler->registry->deleteFlash();
  bool beginSuccess = espHandler->begin();
  bool registerSuccess = espHandler->registerComms(targetID, true);
  bool pingCallbackSuccess =
      espHandler->registerCallback(PacketType::Ping, pingCallback);
  bool pongCallbackSuccess =
      espHandler->registerCallback(PacketType::Pong, pongCallback);
  bool keyDataCallbackSuccess =
      espHandler->registerCallback(PacketType::KeyDataHalf, keyDataCallback);
  if (registerSuccess && pingCallbackSuccess && beginSuccess &&
      keyDataCallbackSuccess && pongCallbackSuccess) {
    Serial.println("setup");
    leds[0] = CRGB::Green;
    FastLED.show();
  }
}

void loop() {

  bool topLeftPressed = false;
  bool topRightPressed = false;
  bool bottomLeftPressed = false;
  bool bottomRightPressed = false;

  for (int row = 0; row < 2; row++) {
    // Set all rows to high-Z, then drive only the active row low.
    for (int r = 0; r < 2; r++) {
      pinMode(rowPins[r], INPUT_PULLUP);
    }
    pinMode(rowPins[row], OUTPUT);
    digitalWrite(rowPins[row], LOW);
    delayMicroseconds(5); // settle time after driving row

    for (int col = 0; col < 2; col++) {
      int pin = colPins[col];
      bool pressed = digitalRead(pin) == LOW;
      if (pressed) {
        if (row == 0 && col == 0)
          topLeftPressed = true;
        if (row == 0 && col == 1)
          topRightPressed = true;
        if (row == 1 && col == 0)
          bottomLeftPressed = true;
        if (row == 1 && col == 1)
          bottomRightPressed = true;
        CRGB lastColor = leds[0];
        if (lastColor != CRGB::Blue) {
          leds[0] = CRGB::Blue;
          FastLED.show();
        }
        lastKeyTime = millis();
      }
    }
  }

  uint8_t keyState[4] = {0};
  memcpy(&keyState[0], &topLeftPressed, 1);
  memcpy(&keyState[1], &topRightPressed, 1);
  memcpy(&keyState[2], &bottomLeftPressed, 1);
  memcpy(&keyState[3], &bottomRightPressed, 1);

  if (topLeftPressed || topRightPressed || bottomLeftPressed ||
      bottomRightPressed) {
    espHandler->sendPacket(targetID, PacketType::KeyDataHalf, keyState,
                           sizeof(keyState));
    espHandler->sendPacket(targetID, PacketType::Ping, &ping, sizeof(ping));
    lastSendTime = millis();
    printf("Keys Sent: %d %d %d %d\n", keyState[0], keyState[1], keyState[2],
           keyState[3]);
  }

  delay(10);

  if (!topLeftPressed && !topRightPressed && !bottomLeftPressed &&
      !bottomRightPressed && millis() - lastKeyTime > 100) {
    leds[0] = CRGB::Green;
    FastLED.show();
  }
  // espHandler->sendPacket(targetID, PacketType::Ping, &ping, sizeof(ping));
  // lastSendTime = millis();
  // // printf("Data sent: %d\n", success);
  // delay(500);
}

const void pingCallback(const uint8_t *dataPtr, size_t len, DeviceID sender) {
  espHandler->sendPacket(sender, PacketType::Pong, &pong, sizeof(pong));
}

const void pongCallback(const uint8_t *dataPtr, size_t len, DeviceID sender) {
  // printf("Data Received: %d\n", *dataPtr);
  unsigned long roundTripTime = millis() - lastSendTime;

  printf(">RTT:");
  printf("%lu\n", roundTripTime);
}

const void keyDataCallback(const uint8_t *dataPtr, size_t len,
                           DeviceID sender) {
  // printf("Key Data Received from %d: ", static_cast<uint8_t>(sender));
  uint8_t keyData[4] = {};
  memcpy(keyData, dataPtr, len);
  if (keyData[0] || keyData[1] || keyData[2] || keyData[3]) {
    leds[0] = CRGB::Purple;
    FastLED.show();
    printf("Keys: %d %d %d %d\n", keyData[0], keyData[1], keyData[2],
           keyData[3]);
    lastKeyTime = millis();
  }
}