/*
 * UNO 1 – MASTER (Khắc phục lỗi liệt bàn phím Keypad)
 */

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Keypad.h>
#include <Servo.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define SS_PIN    10
#define RST_PIN   4
#define SERVO_PIN 9
#define MAX_CARDS 10

const byte ROWS = 4, COLS = 3;
char keys[ROWS][COLS] = {{'1','2','3'},{'4','5','6'},{'7','8','9'},{'*','0','#'}};
byte rowPins[ROWS] = {2, 3, 5, 6}, colPins[COLS] = {7, 8, A3};

MFRC522 rfid(SS_PIN, RST_PIN);
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
Servo myServo;
SoftwareSerial btSerial(A0, A1);

struct UIDCard { byte data[4]; };
UIDCard userCards[MAX_CARDS];
int cardCount = 0;
char matKhauDung[5] = "1111";
char matKhauNhap[5] = "";

volatile bool dangMoKhoa = false;
volatile byte cheDoHeThong = 0x00; 
volatile bool uno2AlertActive = false;
volatile byte lastAlertCode   = 0xA0; // 0xA0=OK, 0xA1=GAS, 0xA2=RUNG, 0xA3=LỬA
volatile byte adminMode = 0; 
volatile unsigned long lastOpenTime = 0;
volatile int timedOpenDuration = 0;
volatile int khoangCach = 0;
volatile bool openedBySensor = false;

SemaphoreHandle_t xMutexI2C = NULL;

void loadCards() {
  cardCount = EEPROM.read(0);
  if (cardCount > MAX_CARDS || cardCount < 0) cardCount = 0;
  for (int i = 0; i < cardCount; i++) {
    for (int j = 0; j < 4; j++) userCards[i].data[j] = EEPROM.read(1 + i*4 + j);
  }
}

void saveCards() {
  EEPROM.write(0, cardCount);
  for (int i = 0; i < cardCount; i++) {
    for (int j = 0; j < 4; j++) EEPROM.write(1 + i*4 + j, userCards[i].data[j]);
  }
}

void forceAction(bool open) {
  myServo.attach(SERVO_PIN);
  if (open) { 
    Serial.println(F(">>> ACTION: OPENING...")); 
    myServo.write(120); 
  } else { 
    Serial.println(F(">>> ACTION: CLOSING...")); 
    myServo.write(60); 
  }
  vTaskDelay(pdMS_TO_TICKS(800)); 
  myServo.write(90); 
  vTaskDelay(pdMS_TO_TICKS(200));
  myServo.detach();
  Serial.println(F(">>> ACTION: DONE."));
}

void lenhMoCua(int duration, bool bySensor) {
  if (!dangMoKhoa) {
    forceAction(true); 
    dangMoKhoa = true;
    if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
      Wire.beginTransmission(0x08); Wire.write(0xB1); Wire.endTransmission();
      xSemaphoreGive(xMutexI2C);
    }
  }
  lastOpenTime = millis(); 
  timedOpenDuration = duration; 
  openedBySensor = bySensor;
}
void xuLyRFID() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    byte currentUID[4]; memcpy(currentUID, rfid.uid.uidByte, 4);
    int index = -1;
    for (int i = 0; i < cardCount; i++) {
      if (memcmp(currentUID, userCards[i].data, 4) == 0) { index = i; break; }
    }
    if (adminMode == 1) { // Add
       if (index >= 0) {
         if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
           Wire.beginTransmission(0x09); Wire.write(0xC2); Wire.write(3); Wire.endTransmission();
           xSemaphoreGive(xMutexI2C);
         }
       } else if (cardCount < MAX_CARDS) {
         memcpy(userCards[cardCount].data, currentUID, 4); cardCount++; saveCards();
         if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
           Wire.beginTransmission(0x09); Wire.write(0xC2); Wire.write(4); Wire.endTransmission();
           xSemaphoreGive(xMutexI2C);
         }
       }
       adminMode = 0;
    } else if (adminMode == 2) { // Del
       if (index >= 0) {
         for (int i = index; i < cardCount - 1; i++) userCards[i] = userCards[i+1];
         cardCount--; saveCards();
         if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
           Wire.beginTransmission(0x09); Wire.write(0xC2); Wire.write(5); Wire.endTransmission();
           xSemaphoreGive(xMutexI2C);
         }
       } else {
         if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
           Wire.beginTransmission(0x09); Wire.write(0xC2); Wire.write(6); Wire.endTransmission();
           xSemaphoreGive(xMutexI2C);
         }
       }
       adminMode = 0;
    } else {
      if (index >= 0) { Serial.println(F("RFID: OK")); lenhMoCua(5, false); }
      else {
        Serial.println(F("RFID: WRONG"));
        if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
          Wire.beginTransmission(0x08); Wire.write(0xB2); Wire.endTransmission();
          xSemaphoreGive(xMutexI2C);
        }
      }
    }
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }
}

void xuLyKeypad() {
  char key = keypad.getKey();
  if (!key) return;
  Serial.print(F("Key: ")); Serial.println(key);
  if (key == '*') { memset(matKhauNhap, 0, 5); Serial.println(F("PASS Cleared")); }
  else if (key != '#') {
    int len = strlen(matKhauNhap);
    if (len < 4) { matKhauNhap[len] = key; matKhauNhap[len+1] = '\0'; }
    if (strlen(matKhauNhap) == 4) {
      if (strcmp(matKhauNhap, matKhauDung) == 0) { Serial.println(F("Pass: OK")); lenhMoCua(5, false); }
      else {
        Serial.println(F("Pass: WRONG"));
        if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
          Wire.beginTransmission(0x08); Wire.write(0xB2); Wire.endTransmission();
          xSemaphoreGive(xMutexI2C);
        }
      }
      memset(matKhauNhap, 0, 5);
    }
  }
}

void xuLyBluetooth(char* buf, byte* idx) {
  while (btSerial.available()) {
    char c = btSerial.read();
    if (c == '\n' || c == '\r') {
      if (*idx > 0) {
        buf[*idx] = '\0';
        if (strcmp(buf, "STATUS") == 0) {
          btSerial.print(F("STATUS:")); btSerial.print(dangMoKhoa ? F("OPEN") : F("CLOSED"));
          btSerial.print(F(",DIST:")); btSerial.print(khoangCach);
          btSerial.print(F(",MODE:")); btSerial.print(cheDoHeThong);
          byte alertByte = uno2AlertActive ? lastAlertCode : 0xA0;
          btSerial.print(F(",ALERT:")); btSerial.println(alertByte, HEX);
        }
        else if (strcmp(buf, "OPEN") == 0) lenhMoCua(10, false);
        else if (strcmp(buf, "CLOSE") == 0) { if (dangMoKhoa) { forceAction(false); dangMoKhoa = false; } }
        else if (strcmp(buf, "RFID:ADD") == 0) {
           adminMode = 1; 
           if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
             Wire.beginTransmission(0x09); Wire.write(0xC2); Wire.write(1); Wire.endTransmission();
             xSemaphoreGive(xMutexI2C);
           }
        }
        else if (strcmp(buf, "RFID:DEL") == 0) {
           adminMode = 2;
           if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
             Wire.beginTransmission(0x09); Wire.write(0xC2); Wire.write(2); Wire.endTransmission();
             xSemaphoreGive(xMutexI2C);
           }
        }
        else if (strncmp(buf, "NEWPASS:", 8) == 0) { 
           memcpy(matKhauDung, buf + 8, 4); matKhauDung[4] = '\0';
           Serial.print(F("NEW PASS: ")); Serial.println(matKhauDung);
        }
        else if (strcmp(buf, "MODE:SECURITY") == 0) cheDoHeThong = 0x00;
        else if (strcmp(buf, "MODE:SAFE") == 0) cheDoHeThong = 0x01;
        else if (strcmp(buf, "MODE:AUTO") == 0) cheDoHeThong = 0x02;
        else if (strcmp(buf, "OLED:SENSOR") == 0) {
           if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
             Wire.beginTransmission(0x09);
             Wire.write(0xC3);
             Wire.write(khoangCach >> 8);
             Wire.write(khoangCach & 0xFF);
             Wire.write(uno2AlertActive ? lastAlertCode : 0xA0);
             Wire.endTransmission();
             xSemaphoreGive(xMutexI2C);
           }
           Serial.println(F("OLED: showing sensor page"));
        }
        *idx = 0;
      }
    } else if (*idx < 31) buf[(*idx)++] = c;
  }
}

void taskCore(void *pvParameters) {
  Serial.begin(9600); 
  Wire.begin(); 
  Wire.setWireTimeout(3000, true); 
  SPI.begin(); 
  rfid.PCD_Init(); 
  loadCards();
  forceAction(false); 
  btSerial.begin(9600);
  vTaskDelay(pdMS_TO_TICKS(2000));
  char btBuf[32]; byte btIdx = 0;
  Serial.println(F("SYSTEM READY!"));

  for (;;) {
    byte u2 = 0xA0;
    if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(100)) == pdTRUE) {
      if (Wire.requestFrom(0x09, 2)) { khoangCach = (Wire.read() << 8) | Wire.read(); }
      if (Wire.requestFrom(0x08, 1)) { u2 = Wire.read(); }
      xSemaphoreGive(xMutexI2C);
    }
    
    uno2AlertActive = (u2 >= 0xA1 && u2 <= 0xA3);
    if (uno2AlertActive) lastAlertCode = u2;

    if (!uno2AlertActive) { xuLyRFID(); xuLyKeypad(); }
    xuLyBluetooth(btBuf, &btIdx);

    // LOGIC MỞ CỬA
    bool isDanger = (u2 == 0xA1 || u2 == 0xA3); // GA hoặc LỬA
    bool isVib    = (u2 == 0xA2);               // RUNG
    bool autoOpen = (cheDoHeThong == 0x02 && khoangCach > 10 && khoangCach < 300);

    if (isDanger || isVib || autoOpen) {
      // Nếu là RUNG, chỉ mở trong 5 giây rồi cho phép đóng
      int dur = isVib ? 5 : 0; 
      lenhMoCua(dur, isVib || autoOpen);
    } 
    
    // LOGIC ĐÓNG CỬA
    if (dangMoKhoa) {
      // Điều kiện an toàn: Không có CHÁY/GA và (Cảm biến siêu âm thoáng HOẶC không ở chế độ Auto)
      bool safeToClose = !isDanger; 
      
      // Kiểm tra timeout cho Rung hoặc mở tự động
      unsigned long elapsed = (millis() - lastOpenTime) / 1000;
      bool isTimeout = (timedOpenDuration > 0 && elapsed >= (unsigned long)timedOpenDuration);
      
      if (safeToClose && isTimeout) {
        Serial.print(F("Auto Closing... Time: ")); Serial.println(elapsed);
        forceAction(false); 
        dangMoKhoa = false;
        if (xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(50)) == pdTRUE) {
           Wire.beginTransmission(0x08); Wire.write(0xB4); Wire.endTransmission();
           xSemaphoreGive(xMutexI2C);
        }
      }
    }

    // Cập nhật OLED
    if (adminMode == 0 && xSemaphoreTake(xMutexI2C, pdMS_TO_TICKS(100)) == pdTRUE) {
      Wire.beginTransmission(0x09); Wire.write(0xC1);
      Wire.write(dangMoKhoa ? 1 : 0); Wire.write(cheDoHeThong);
      Wire.write(khoangCach >> 8); Wire.write(khoangCach & 0xFF);
      Wire.write(uno2AlertActive ? 0x02 : 1); Wire.write(0); Wire.endTransmission();
      xSemaphoreGive(xMutexI2C);
    }
    vTaskDelay(pdMS_TO_TICKS(100)); 
  }
}

void setup() {
  xMutexI2C = xSemaphoreCreateMutex();
  // Tăng stack lên 512 để tránh treo mạch
  xTaskCreate(taskCore, "CORE", 512, NULL, 3, NULL);
}

void loop() {}