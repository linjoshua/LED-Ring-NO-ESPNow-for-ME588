/*
=====================================================
🛠️ LED Ring Standalone Controller (No ESP-NOW)
📅 Version: 2025-04-07
=====================================================

📘 USER GUIDE:
1️⃣ Color Button (GPIO 14):
    - Press to cycle through predefined team colors on the LED ring.

2️⃣ Enable Button (GPIO 26):
    - Press to start the robot and timer.
    - The LED animation runs according to selected FSM state.
    - After timeout (default 2.5 minutes), LEDs revert to base color.

3️⃣ FSM State Control (Manual):
    - Type a number (0–6) into the Serial Monitor to change the robot's state.

🧠 FSM State Mapping:
    0: Drive Forward     → Clockwise rotation
    1: Drive Backward    → Counter-clockwise rotation
    2: Launch Left       → Slow blinking
    3: Launch Right      → Fast blinking
    4: Stop              → Always bright
    5: To Home           → Wipe fill effect
    6: At Home           → Comet/pulse forward

=====================================================
*/

#include <Adafruit_NeoPixel.h>

#define LED_PIN 13
#define COLOR_BUTTON 14
#define ENABLE_BUTTON 26
#define NUM_LEDS 8

Adafruit_NeoPixel ring(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

bool enabled = false;
bool lastEnableButtonState = HIGH;
int colorIndex = 0;
int motionCommand = '0';
int lastColorButtonState = HIGH;

int startTime;
int duration = 150000;  // 2.5 minutes in milliseconds

uint32_t colors[] = {
  ring.Color(255, 0, 0),     // Red
  ring.Color(0, 255, 0),     // Green
  ring.Color(0, 0, 255),     // Blue
  ring.Color(255, 255, 0),   // Yellow
  ring.Color(255, 255, 255)  // White
};
int totalColors = sizeof(colors) / sizeof(colors[0]);

void setup() {
  pinMode(COLOR_BUTTON, INPUT_PULLUP);
  pinMode(ENABLE_BUTTON, INPUT_PULLUP);
  Serial.begin(115200);

  ring.begin();
  ring.setBrightness(8);
  ring.show();
}

void loop() {
  // Color selection via button
  int colorButtonState = digitalRead(COLOR_BUTTON);
  if (colorButtonState == LOW && lastColorButtonState == HIGH) {
    delay(50); // Debounce
    colorIndex = (colorIndex + 1) % totalColors;
    alwaysBright(colors[colorIndex]);
  }
  lastColorButtonState = colorButtonState;

  // Enable robot via button
  bool enableState = digitalRead(ENABLE_BUTTON);
  if (enableState == LOW && lastEnableButtonState == HIGH) {
    delay(50); // Debounce
    if (digitalRead(ENABLE_BUTTON) == LOW) {
      enabled = true;
      startTime = millis();
      Serial.println("Robot Enabled!");
    }
  }
  lastEnableButtonState = enableState;

  // Manual FSM state input via Serial
  if (Serial.available() > 0) {
    int newCommand = Serial.read();
    if (newCommand >= '0' && newCommand <= '6') {
      motionCommand = newCommand;
      Serial.print("Mode: ");
      Serial.println(motionCommand);
    }
  }

  // LED control logic while robot is active
  if (enabled) {
    switch (motionCommand) {
      case '0': rotatingEffectCW(colors[colorIndex]); Serial.println("DF"); break; //Drive Forward
      case '1': rotatingEffectCCW(colors[colorIndex]); Serial.println("DB"); break; //Drive Backward
      case '2': slowBlinkingEffect(colors[colorIndex]); Serial.println("LL"); break; //Launch Left
      case '3': fastBlinkingEffect(colors[colorIndex]); Serial.println("LR"); break; //Launch Right
      case '4': alwaysBright(colors[colorIndex]); Serial.println("STOP"); break; //STOP
      case '5': wipeFillEffect(colors[colorIndex]); Serial.println("TH"); break; //To Home
      case '6': pulseForward(colors[colorIndex]); Serial.println("AT"); break; //At Home
    }

    if ((millis() - startTime) > duration) {
      enabled = false;
      Serial.println("Time's up!");
    }
  } else {
    alwaysBright(colors[colorIndex]);
  }
}

// LED Effect: Rotate clockwise
void rotatingEffectCW(uint32_t color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.clear();
    ring.setPixelColor(i, color);
    ring.show();
    delay(100);
  }
}

// LED Effect: Rotate counter-clockwise
void rotatingEffectCCW(uint32_t color) {
  for (int i = NUM_LEDS - 1; i >= 0; i--) {
    ring.clear();
    ring.setPixelColor(i, color);
    ring.show();
    delay(100);
  }
}

// LED Effect: Slow blink
void slowBlinkingEffect(uint32_t color) {
  ring.fill(color);
  ring.show();
  delay(300);
  ring.clear();
  ring.show();
  delay(300);
}

// LED Effect: Fast blink
void fastBlinkingEffect(uint32_t color) {
  ring.fill(color);
  ring.show();
  delay(40);
  ring.clear();
  ring.show();
  delay(40);
}

// LED Effect: Constant brightness
void alwaysBright(uint32_t color) {
  ring.fill(color);
  ring.show();
}

// LED Effect: Wipe fill
void wipeFillEffect(uint32_t color) {
  ring.clear();
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.setPixelColor(i, color);
    ring.show();
    delay(80);
  }
  delay(300);
  ring.clear();
  ring.show();
  delay(200);
}

// LED Effect: Pulse forward (comet)
void pulseForward(uint32_t color) {
  ring.clear();
  for (int i = 0; i < NUM_LEDS; i++) {
    ring.clear();
    ring.setPixelColor(i, color);
    if (i > 0) ring.setPixelColor(i - 1, dimColor(color, 100));
    if (i > 1) ring.setPixelColor(i - 2, dimColor(color, 40));
    ring.show();
    delay(80);
  }
}

// Dim color helper (for comet tail)
uint32_t dimColor(uint32_t color, uint8_t brightness) {
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)color;
  r = (r * brightness) / 255;
  g = (g * brightness) / 255;
  b = (b * brightness) / 255;
  return ring.Color(r, g, b);
}

