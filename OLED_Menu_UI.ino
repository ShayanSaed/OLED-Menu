#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

#define BTN_UP D5
#define BTN_DOWN D6
#define BTN_SELECT D7

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct Button {
  uint8_t pin;
  bool stableState;
  bool lastReading;
  bool pressed;
  unsigned long lastDebounceTime;
};

Button btnUp = { BTN_UP, HIGH, HIGH, false, 0 };
Button btnDown = { BTN_DOWN, HIGH, HIGH, false, 0 };
Button btnSelect = { BTN_SELECT, HIGH, HIGH, false, 0 };

const unsigned long debounceDelay = 50;

const char *menuItems[] = {
  "Status",
  "Network",
  "Settings",
  "About"
};

const uint8_t menuLength = sizeof(menuItems) / sizeof(menuItems[0]);
uint8_t selectedIndex = 0;

void readButton(Button &btn);
void readButtons();

void printMenu();

void setup() {
  Serial.begin(115200);

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);

  Wire.begin(4, 5);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed");
    while (true);
  }

  printMenu();
}

void loop() {
  readButtons();

  bool menuChanged = false;

  if (btnUp.pressed) {
    btnUp.pressed = false;

    if (selectedIndex == 0) {
      selectedIndex = menuLength - 1;
    } else {
      selectedIndex--;
    }

    menuChanged = true;
  }

  if (btnDown.pressed) {
    btnDown.pressed = false;

    selectedIndex = (selectedIndex + 1) % menuLength;
    menuChanged = true;
  }

  if (btnSelect.pressed) {
    btnSelect.pressed = false;

    Serial.print("Selected: ");
    Serial.println(menuItems[selectedIndex]);
  }

  if (menuChanged) {
    printMenu();
  }
}

void readButton(Button &btn) {
  bool reading = digitalRead(btn.pin);

  if (reading != btn.lastReading) {
    btn.lastDebounceTime = millis();
  }

  if ((unsigned long)(millis() - btn.lastDebounceTime) > debounceDelay) {
    if (reading != btn.stableState) {
      btn.stableState = reading;

      if (btn.stableState == LOW) {
        btn.pressed = true;
      }
    }
  }

  btn.lastReading = reading;
}

void readButtons() {
  readButton(btnUp);
  readButton(btnDown);
  readButton(btnSelect);
}

void printMenu() {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);

  const char *title = "Main Menu";

  int16_t x1, y1;
  uint16_t textWidth, textHeight;

  display.getTextBounds(title, 0, 0, &x1, &y1, &textWidth, &textHeight);
  display.setCursor((128 - textWidth) / 2, 0);
  display.print(title);

  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  for (uint8_t i = 0; i < menuLength; i++) {
    uint8_t y = 14 + i * 12;

    if (i == selectedIndex) {
      display.fillRect(0, y - 1, SCREEN_WIDTH, 11, SSD1306_WHITE);
      display.setTextColor(SSD1306_BLACK);
    } else {
      display.setTextColor(SSD1306_WHITE);
    }

    display.setCursor(6, y);
    display.println(menuItems[i]);
  }

  display.display();
}