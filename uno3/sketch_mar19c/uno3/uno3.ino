/*
 * UNO 3 – SLAVE I2C (Giao diện OLED Premium 2.0)
 */

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Wire.h>
#include <U8g2lib.h>

#define PIN_TRIG 3
#define PIN_ECHO 4

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE); // Xoay 180 chuẩn

volatile int khoangCach  = 0;
volatile bool sysMoKhoa  = false;
volatile byte sysCheDo   = 0;
volatile int  sysKhoangCach = 0;
volatile byte sysAlertBits  = 0;

enum ScreenState { SCR_FACE, SCR_DOOR, SCR_ALERT, SCR_ADD, SCR_DEL, SCR_MSG, SCR_SENSOR };
volatile ScreenState currentScreen = SCR_FACE;
volatile unsigned long screenTimer = 0; 
volatile char msgText[20] = "";
// Dữ liệu trang cảm biến (từ lệnh 0xC3)
volatile int  sensorDist  = 0;
volatile byte sensorAlert = 0xA0; // 0xA0=OK, 0xA1=GAS, 0xA2=RUNG, 0xA3=LỬA

void guiChoUno1() {
  int kc = khoangCach;
  Wire.write((kc >> 8) & 0xFF); Wire.write(kc & 0xFF);
}

void nhanTuUno1(int soBytes) {
  if (soBytes >= 2) {
    byte header = Wire.read();
    if (header == 0xC1) {
      bool mkPrev = sysMoKhoa;
      sysMoKhoa = (Wire.read() == 1);
      sysCheDo  = Wire.read();
      byte kcH  = Wire.read();
      byte kcL  = Wire.read();
      sysKhoangCach = (kcH << 8) | kcL;
      sysAlertBits  = Wire.read();
      
      if (currentScreen == SCR_FACE || currentScreen == SCR_DOOR || currentScreen == SCR_ALERT) {
        if (sysAlertBits & 0x02) { currentScreen = SCR_ALERT; screenTimer = millis(); }
        else if (sysMoKhoa != mkPrev) { currentScreen = SCR_DOOR; screenTimer = millis(); }
      }
    } 
    else if (header == 0xC2) {
      byte cmd = Wire.read();
      if (cmd == 1) { currentScreen = SCR_ADD; screenTimer = millis(); }
      else if (cmd == 2) { currentScreen = SCR_DEL; screenTimer = millis(); }
      else if (cmd == 3) { strcpy(msgText, "EXISTED"); currentScreen = SCR_MSG; screenTimer = millis(); }
      else if (cmd == 4) { strcpy(msgText, "SUCCESS"); currentScreen = SCR_MSG; screenTimer = millis(); }
      else if (cmd == 5) { strcpy(msgText, "DELETED"); currentScreen = SCR_MSG; screenTimer = millis(); }
      else if (cmd == 6) { strcpy(msgText, "NOT FOUND"); currentScreen = SCR_MSG; screenTimer = millis(); }
    }
    // ── LỆNH MỚI 0xC3: Hiển thị trang cảm biến ──────────────────────
    else if (header == 0xC3) {
      byte kcH  = Wire.read();
      byte kcL  = Wire.read();
      byte alrt = Wire.read();
      sensorDist  = (kcH << 8) | kcL;
      sensorAlert = alrt;
      currentScreen = SCR_SENSOR;
      screenTimer = millis();
    }
    while (Wire.available()) Wire.read();
  }
}

int docKhoangCach() {
  digitalWrite(PIN_TRIG, LOW); delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH); delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);
  long t = pulseIn(PIN_ECHO, HIGH, 20000);
  return (t == 0) ? khoangCach : (int)(t * 0.34f / 2.0f);
}

void taskDocKhoangCach(void *pvParameters) {
  for (;;) {
    int gia_tri = docKhoangCach();
    if (gia_tri > 0) khoangCach = gia_tri;
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

// Vẽ mặt cười cân đối
void drawFace(bool isBlinking, bool isAlert) {
  int cy = 24; 
  if (isBlinking) {
    u8g2.drawBox(35, cy, 14, 2); u8g2.drawBox(79, cy, 14, 2);
  } else {
    if (isAlert) {
      u8g2.drawDisc(42, cy, 9); u8g2.drawDisc(86, cy, 9);
      u8g2.setDrawColor(0); u8g2.drawDisc(42, cy, 4); u8g2.drawDisc(86, cy, 4); u8g2.setDrawColor(1);
      u8g2.drawLine(28, cy-12, 48, cy-6); u8g2.drawLine(80, cy-6, 100, cy-12);
    } else {
      u8g2.drawDisc(42, cy, 8); u8g2.drawDisc(86, cy, 8);
      u8g2.setDrawColor(0); u8g2.drawDisc(44, cy-2, 2); u8g2.drawDisc(88, cy-2, 2); u8g2.setDrawColor(1);
    }
  }
  if (isAlert) u8g2.drawCircle(64, 46, 12, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
  else u8g2.drawEllipse(64, 44, 15, 6, U8G2_DRAW_LOWER_RIGHT | U8G2_DRAW_LOWER_LEFT);
}

void taskOLED(void *pvParameters) {
  byte frame = 0;
  for (;;) {
    // SCR_SENSOR không tự reset (giữ 6 giây)
    unsigned long timeout = (currentScreen == SCR_SENSOR) ? 6000 : 4000;
    if (currentScreen != SCR_FACE && (millis() - screenTimer > timeout)) currentScreen = SCR_FACE;

    u8g2.firstPage();
    do {
      if (currentScreen == SCR_FACE) {
        drawFace((frame % 30 == 0), (sysKhoangCach < 300 || (sysAlertBits & 0x02)));
        u8g2.drawHLine(0, 52, 128); u8g2.setFont(u8g2_font_04b_03_tr); 
        u8g2.setCursor(2, 62); u8g2.print(sysCheDo == 2 ? "AUTO MODE" : "SECURE MODE");
        u8g2.setCursor(98, 62); u8g2.print(sysKhoangCach); u8g2.print("mm");
      } 
      else if (currentScreen == SCR_DOOR) {
        u8g2.setFont(u8g2_font_helvB10_tf); u8g2.drawStr(30, 25, "SECURITY");
        u8g2.drawFrame(30, 35, 68, 20); u8g2.setCursor(40, 50); u8g2.print(sysMoKhoa ? "OPENED" : "LOCKED");
      }
      else if (currentScreen == SCR_ADD || currentScreen == SCR_DEL) {
        u8g2.drawFrame(5, 5, 118, 54); u8g2.setFont(u8g2_font_6x12_tf);
        u8g2.drawStr(25, 25, currentScreen == SCR_ADD ? "ADMIN: ADD" : "ADMIN: DEL");
        u8g2.drawStr(15, 45, "SCAN YOUR CARD");
      }
      else if (currentScreen == SCR_MSG) {
        u8g2.drawRFrame(10, 15, 108, 35, 5); u8g2.setFont(u8g2_font_helvB10_tf);
        u8g2.drawStr(25, 38, msgText);
      }
      else if (currentScreen == SCR_ALERT) {
        u8g2.drawBox(0, 0, 128, 64); u8g2.setDrawColor(0); u8g2.setFont(u8g2_font_helvB12_tf);
        u8g2.drawStr(20, 38, "!! DANGER !!"); u8g2.setDrawColor(1);
      }
      else if (currentScreen == SCR_SENSOR) {
        // ── Trang SENSOR DATA ──────────────────────────────────────
        u8g2.setFont(u8g2_font_6x12_tf);
        u8g2.drawStr(20, 11, "[ SENSOR DATA ]");
        u8g2.drawHLine(0, 14, 128);

        // Khoảng cách
        u8g2.drawStr(0, 26, "DIST:");
        u8g2.setCursor(40, 26);
        u8g2.print(sensorDist); u8g2.print(" mm");

        // Gas
        u8g2.drawStr(0, 38, "GAS: ");
        u8g2.drawStr(40, 38, (sensorAlert == 0xA1) ? "!! CANH BAO !!" : "OK");

        // Lua
        u8g2.drawStr(0, 50, "LUA: ");
        u8g2.drawStr(40, 50, (sensorAlert == 0xA3) ? "!! CANH BAO !!" : "OK");

        // Rung
        u8g2.drawStr(0, 62, "RUNG:");
        u8g2.drawStr(40, 62, (sensorAlert == 0xA2) ? "!! CANH BAO !!" : "OK");
      }
    } while ( u8g2.nextPage() );
    frame++; vTaskDelay(pdMS_TO_TICKS(150));
  }
}

void setup() {
  Wire.begin(0x09); Wire.onRequest(guiChoUno1); Wire.onReceive(nhanTuUno1);
  u8g2.begin(); // Display rotation set via U8G2_R2 constructor above
  pinMode(PIN_TRIG, OUTPUT); pinMode(PIN_ECHO, INPUT);
  xTaskCreate(taskDocKhoangCach, "SONIC",  128,  NULL,  3,  NULL);
  xTaskCreate(taskOLED,          "OLED",   200,  NULL,  2,  NULL);
}

void loop() {}