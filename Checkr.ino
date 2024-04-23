#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <DacESP32.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

#define SDA_PIN GPIO_NUM_13
#define SCL_PIN GPIO_NUM_15

#define RED_PIN GPIO_NUM_4
#define GREEN_PIN GPIO_NUM_16
#define BLUE_PIN GPIO_NUM_17

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

DacESP32 dac1(GPIO_NUM_25);

MFRC522DriverPinSimple ss_pin(GPIO_NUM_5);

MFRC522DriverSPI driver{ ss_pin };

MFRC522 reader{ driver };

const unsigned char ns_icon[] PROGMEM = {
  0x0f, 0x10, 0x10, 0x88, 0x20, 0x44, 0x7c, 0x3e, 0x22, 0x04, 0x11, 0x08, 0x08, 0xf0, 0x00, 0x00
};

const unsigned char rrreis_icon[] PROGMEM = {
  0x73, 0x9c, 0x08, 0x42, 0x08, 0x42, 0x73, 0x9c, 0x63, 0x18, 0x52, 0x94, 0x4a, 0x52, 0x00, 0x00
};

const unsigned char blauwnet_icon[] PROGMEM = {
  0x08, 0x00, 0x08, 0x00, 0x0b, 0xc0, 0x08, 0x60, 0x08, 0x20, 0x08, 0x20, 0x04, 0x20, 0x03, 0xa0
};

const unsigned char sbb_icon[] PROGMEM = {
  0x09, 0x20, 0x11, 0x10, 0x21, 0x08, 0x7f, 0xfc, 0x21, 0x08, 0x11, 0x10, 0x09, 0x20, 0x00, 0x00
};

const unsigned char tram_icon[] PROGMEM = {
  0x04, 0x20, 0x0a, 0x50, 0x04, 0x20, 0x3f, 0xfc, 0x6a, 0x56, 0x4a, 0x52, 0x7f, 0xfe, 0x28, 0x14
};

const unsigned char ret_icon[] PROGMEM = {
  0xbb, 0xfe, 0x88, 0x08, 0x88, 0x08, 0xbb, 0xc8, 0x90, 0x08, 0x88, 0x08, 0x8f, 0xc8, 0x00, 0x00
};

const unsigned char syntus_icon[] PROGMEM = {
  0x07, 0x00, 0x05, 0x00, 0x0d, 0x00, 0x11, 0xf0, 0x1f, 0x10, 0x01, 0x60, 0x01, 0x40, 0x01, 0xc0
};

#define ICON ns_icon
/**
 * Initialize.
 */
void setup() {
  Serial.begin(115200);  // Initialize serial communications with the PC for debugging.
  while (!Serial)
    ;  // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).

  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);

  reader.PCD_Init();
  Serial.print(F("Reader: "));
  MFRC522Debug::PCD_DumpVersionToSerial(reader, Serial);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  idleScherm();
}

void idleScherm() {
  setNone();
  display.clearDisplay();
  display.invertDisplay(false);
  display.setCursor(0, 4);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F(" In-/uitchecken"));
  display.println();
  display.println(F(" Bied je pas los aan"));
  //  display.println(F(" Scan your card separately"));
  display.drawBitmap(108, 52, ICON, 16, 8, WHITE);
  display.display();
}

void inScherm() {
  setGreen();
  display.clearDisplay();
  display.invertDisplay(true);
  display.setCursor(0, 4);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F(" Ingecheckt"));
  display.println(F(" Reizen op Saldo NS"));
  display.println();
  display.println(F(" 2e    klas"));
  display.drawBitmap(108, 52, ICON, 16, 8, WHITE);
  display.display();
}

void inOvPayScherm() {
  setGreen();
  display.clearDisplay();
  display.invertDisplay(true);
  display.setCursor(0, 4);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F(" OK"));
  display.drawBitmap(108, 52, ICON, 16, 8, WHITE);
  display.display();
}

void inRailrunnerScherm() {
  setGreen();
  display.clearDisplay();
  display.invertDisplay(true);
  display.setCursor(0, 4);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F(" Ingecheckt"));
  display.println(F(" Railrunner"));
  display.drawBitmap(108, 52, ICON, 16, 8, WHITE);
  display.display();
}

void uitScherm() {
  setBlue();
  display.clearDisplay();
  display.invertDisplay(true);
  display.setCursor(0, 4);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F(" Uitgecheckt"));
  display.drawBitmap(108, 52, ICON, 16, 8, WHITE);
  display.display();
}

void errScherm() {
  setRed();
  display.clearDisplay();
  display.invertDisplay(true);
  display.setCursor(0, 4);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F(" Probeer opnieuw"));
  display.drawBitmap(108, 52, ICON, 16, 8, WHITE);
  display.display();
}

void setGreen() {
  analogWrite(RED_PIN, LOW);
  analogWrite(GREEN_PIN, 122);
  analogWrite(BLUE_PIN, LOW);
}

void setBlue() {
  analogWrite(RED_PIN, LOW);
  analogWrite(GREEN_PIN, LOW);
  analogWrite(BLUE_PIN, 122);
}

void setRed() {
  analogWrite(RED_PIN, 122);
  analogWrite(GREEN_PIN, LOW);
  analogWrite(BLUE_PIN, LOW);
}

void setNone() {
  analogWrite(RED_PIN, LOW);
  analogWrite(GREEN_PIN, LOW);
  analogWrite(BLUE_PIN, LOW);
}

// Vul hier je eigen kaart-UID in.
byte ov2eKlasBytes[] = { 0x61, 0x69, 0x2E, 0x80 };
bool 2eKlasCheckedIn = false;

byte railRunnerCheckins = 0x00;

unsigned long lastStateMillis = 0;

enum State {
  Idle,
  InfoDelay,
  In,
  Uit0,
  Uit1,
  Uit2,
  Err0,
  Err1,
  Err2
};

State currentState = Idle;

#define FREQ_ERR0 1865
#define FREQ_ERR1 1245
#define FREQ_UIT 902
#define FREQ_IN 948

void audioLoop() {
  unsigned long currentMillis = millis();
  switch (currentState) {
    case InfoDelay:
      if (currentMillis - lastStateMillis > 1000UL) {
        currentState = Idle;
        lastStateMillis = currentMillis;
        idleScherm();
      }
      break;
    case In:
      if (currentMillis - lastStateMillis > 300UL) {
        dac1.disable();
        currentState = InfoDelay;
        lastStateMillis = currentMillis;
      }
      break;
    case Uit0:
      if (currentMillis - lastStateMillis > 200UL) {
        dac1.disable();
        currentState = Uit1;
        lastStateMillis = currentMillis;
      }
      break;
    case Uit1:
      if (currentMillis - lastStateMillis > 100UL) {
        dac1.outputCW(FREQ_UIT);
        currentState = Uit2;
        lastStateMillis = currentMillis;
      }
      break;
    case Uit2:
      if (currentMillis - lastStateMillis > 200UL) {
        dac1.disable();
        currentState = InfoDelay;
        lastStateMillis = currentMillis;
      }
      break;
    case Err0:
      if (currentMillis - lastStateMillis > 350UL) {
        dac1.outputCW(FREQ_ERR1);
        currentState = Err1;
        lastStateMillis = currentMillis;
      }
      break;
    case Err1:
      if (currentMillis - lastStateMillis > 350UL) {
        dac1.outputCW(FREQ_ERR0);
        currentState = Err2;
        lastStateMillis = currentMillis;
      }
      break;
    case Err2:
      if (currentMillis - lastStateMillis > 350UL) {
        dac1.disable();
        currentState = InfoDelay;
        lastStateMillis = currentMillis;
      }
      break;
    default:
      break;
  }
}

void in() {
  dac1.outputCW(FREQ_IN);
  currentState = In;
  lastStateMillis = millis();
  inScherm();
}

void inOVPay() {
  dac1.outputCW(FREQ_IN);
  currentState = In;
  lastStateMillis = millis();
  inOvPayScherm();
}

void inRailRunner() {
  dac1.outputCW(FREQ_IN);
  currentState = In;
  lastStateMillis = millis();
  inRailrunnerScherm();
}

void uit() {
  dac1.outputCW(FREQ_UIT);
  currentState = Uit0;
  lastStateMillis = millis();
  uitScherm();
}

void err() {
  dac1.outputCW(FREQ_ERR0);
  currentState = Err0;
  lastStateMillis = millis();
  errScherm();
}

void scanLoop() {
  if (reader.PICC_IsNewCardPresent() && reader.PICC_ReadCardSerial()) {
    Serial.print(F("Reader "));
    static uint8_t i = 0;
    i++;
    Serial.print(i);

    // Show some details of the PICC (that is: the tag/card).
    Serial.print(F(": Card UID:"));
    MFRC522Debug::PrintUID(Serial, reader.uid);

    if (reader.uid.size == 4 && memcmp(reader.uid.uidByte, ov2eKlasBytes, 4) == 0) {
      2eKlasCheckedIn = !2eKlasCheckedIn;
      if (2eKlasCheckedIn) {
        in();
      } else {
        uit();
      }
    } else if (reader.uid.size == 4 && reader.uid.uidByte[0] == 0x08) {
      inOVPay();
    } else if (reader.uid.size == 7 && reader.uid.uidByte[0] == 0x04 && reader.uid.uidByte[1] <= 0xEE && reader.uid.uidByte[1] >= 0xEA) {
      railRunnerCheckins ^= 0x01 << (reader.uid.uidByte[1] - 0xEA);
      if ((railRunnerCheckins >> (reader.uid.uidByte[1] - 0xEA)) & 0x01) {
        inRailRunner();
      } else {
        uit();
      }
    } else {
      err();
    }

    Serial.println();

    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = reader.PICC_GetType(reader.uid.sak);
    Serial.println(MFRC522Debug::PICC_GetTypeName(piccType));

    // Halt PICC.
    reader.PICC_HaltA();
    // Stop encryption on PCD.
    reader.PCD_StopCrypto1();
  }
}

/**
 * Main loop.
 */
void loop() {
  scanLoop();
  audioLoop();
}
