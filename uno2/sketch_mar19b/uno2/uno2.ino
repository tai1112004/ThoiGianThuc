/*
 * UNO 2 – SLAVE I2C (địa chỉ 0x08)
 * Vai trò: Cảm biến khí ga MQ-2, cảm biến rung SW-420, Buzzer
 * FreeRTOS: Arduino_FreeRTOS library (Phillip Stevens)
 *
 * Cài thư viện: Sketch → Include Library → Manage Libraries
 *   → tìm "FreeRTOS" → cài "Arduino_FreeRTOS Library" by Phillip Stevens
 *
 * Phân chia task:
 *   taskDocGas   – đọc MQ-2 mỗi 1 s          (priority 2)
 *   taskXuLyLenh – xử lý lệnh từ master       (priority 3)
 *   taskXuLyRung – xử lý cờ rung SW-420       (priority 3)
 *   taskWarmup   – đếm thời gian khởi động    (priority 1, tự xóa)
 *
 * I2C callback (ngắt) chỉ set flag/biến – KHÔNG xử lý nặng bên trong.
 */

#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <Wire.h>

// ============ CHÂN ============
#define PIN_MQ2_ANALOG  A0
#define PIN_MQ2_DIGITAL 7
#define PIN_FLAME       4      // Chân D0 của Cảm biến Lửa
#define PIN_SW420       2
#define PIN_BUZZER      8

// ============ CÀI ĐẶT ============
#define NGUONG_GAS       200   // Cài đặt mức 200 theo yêu cầu người dùng
#define TAN_SO_BUZZER    3000
#define THOI_GIAN_WARMUP 30000UL   // 30 giây

// ============ BIẾN TOÀN CỤC ============
volatile byte trangThai    = 0x00;   // Master hỏi → slave trả về giá trị này
volatile byte lenhNhan     = 0x00;   // Lệnh nhận từ Master
volatile bool phatHienRung = false;  // Cờ set bởi ISR

volatile bool warmupDone   = false;
unsigned long warmupStart  = 0;

// ============ MUTEX ============
SemaphoreHandle_t xMutexState = NULL;  // Bảo vệ trangThai, lenhNhan

// ============ SEMAPHORE (binary) – báo hiệu task ============
SemaphoreHandle_t xSemRung = NULL;
SemaphoreHandle_t xSemLenh = NULL;

// ============ KHAI BÁO TASK ============
void taskDocGas   (void *pvParameters);
void taskXuLyLenh (void *pvParameters);
void taskXuLyRung (void *pvParameters);
void taskWarmup   (void *pvParameters);

// ===================================================
// I2C CALLBACKS  (chạy trong ngắt – phải cực ngắn)
// ===================================================

// Master hỏi → gửi trangThai
void guiTrangThai() {
  Wire.write(trangThai);
  // Không reset trangThai ngay lập tức nếu đang trong báo động kéo dài
}

// Master gửi lệnh
void nhanLenh(int soBytes) {
  while (Wire.available()) {
    lenhNhan = Wire.read();
  }
  // Báo task từ ISR context (AVR port: portYIELD_FROM_ISR không có tham số)
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR(xSemLenh, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) portYIELD_FROM_ISR();
}

// ============ NGẮT RUNG (ISR) ============
void ngatRung() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  // Chỉ gửi semaphore, không đặt biến trạng thái ở đây
  xSemaphoreGiveFromISR(xSemRung, &xHigherPriorityTaskWoken);
  if (xHigherPriorityTaskWoken) portYIELD_FROM_ISR();
}

// ===================================================
// TIỆN ÍCH: BUZZER  (gọi từ task – dùng vTaskDelay)
// ===================================================
void baoHieu(int soLan, int thoiGian) {
  for (int i = 0; i < soLan; i++) {
    tone(PIN_BUZZER, TAN_SO_BUZZER);
    vTaskDelay(pdMS_TO_TICKS(thoiGian));
    noTone(PIN_BUZZER);
    vTaskDelay(pdMS_TO_TICKS(thoiGian));
  }
}

// ===================================================
// TASK: WARMUP  (priority 1 – chạy một lần rồi tự xóa)
// ===================================================
void taskWarmup(void *pvParameters) {
  (void)pvParameters;

  // Báo hiệu khởi động (phải nằm trong task vì có vTaskDelay)
  baoHieu(1, 100);

  unsigned long start = millis();
  Serial.println(F("Dang ham nong MQ-2 (30 giay)..."));

  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xInterval = pdMS_TO_TICKS(5000);

  while (millis() - start < THOI_GIAN_WARMUP) {
    unsigned long con = (THOI_GIAN_WARMUP - (millis() - start)) / 1000;
    Serial.print(F("Con lai: ")); Serial.print(con); Serial.println(F("s"));
    vTaskDelayUntil(&xLastWakeTime, xInterval);
  }
  warmupDone = true;
  Serial.println(F("=== Uno 2 san sang! ==="));
  baoHieu(2, 100);
  vTaskDelete(NULL);  // Tự xóa task sau khi xong
}

// ===================================================
// TASK: ĐỌC GAS mỗi 1 s  (priority 2)
// ===================================================
void taskDocGas(void *pvParameters) {
  (void)pvParameters;
  TickType_t xLastWakeTime = xTaskGetTickCount();

  for (;;) {
    // Chờ warmup xong
    if (!warmupDone) {
      vTaskDelay(pdMS_TO_TICKS(500));
      continue;
    }

    int giaTriGas  = analogRead(PIN_MQ2_ANALOG);
    int lua        = digitalRead(PIN_FLAME);

    Serial.print(F("MQ2 Analog: ")); Serial.print(giaTriGas);
    Serial.print(F(" | LUA: ")); Serial.println(lua == LOW ? F("CO LUA!") : F("An toan"));

    if (giaTriGas > NGUONG_GAS) {
      trangThai = 0xA1; // Cờ GAS
      baoHieu(1, 200);
    } else if (lua == LOW) {
      trangThai = 0xA3; // Cờ LỬA
      baoHieu(2, 100);
    } else {
      // Nếu không có gas/lửa, và task Rung cũng không báo, thì mới về 0
      if (!phatHienRung) trangThai = 0xA0;
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));
  }
}

// ===================================================
// TASK: XỬ LÝ LỆNH từ Master  (priority 3)
// ===================================================
void taskXuLyLenh(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    // Chờ semaphore báo có lệnh mới (timeout 100 ms để không treo mãi)
    if (xSemaphoreTake(xSemLenh, pdMS_TO_TICKS(100)) == pdTRUE) {
      byte lenh = lenhNhan;
      lenhNhan  = 0x00;

      if (!warmupDone || lenh == 0x00) continue;

      Serial.print(F("Lenh tu Master: 0x")); Serial.println(lenh, HEX);

      if      (lenh == 0xB1) { Serial.println(F("→ Mo khoa"));               baoHieu(1, 200); }
      else if (lenh == 0xB2) { Serial.println(F("→ Sai mat khau / the sai")); baoHieu(3, 100); }
      else if (lenh == 0xB3) { Serial.println(F("→ Bao dong khan cap!"));     baoHieu(5, 500); }
      else if (lenh == 0xB4) { Serial.println(F("→ Dong khoa"));              baoHieu(1, 100); }
    }
  }
}

// ===================================================
// TASK: XỬ LÝ RUNG  (priority 3)
// ===================================================
void taskXuLyRung(void *pvParameters) {
  (void)pvParameters;
  for (;;) {
    // Đợi tín hiệu từ cảm biến rung (ngắt)
    if (xSemaphoreTake(xSemRung, portMAX_DELAY) == pdTRUE) {
      if (!warmupDone) continue;
      
      // Nếu đang trong quá trình báo động (đã đặt phatHienRung = true) thì mới bỏ qua
      // Còn đây là bắt đầu một báo động mới
      if (phatHienRung) continue; 

      Serial.println(F("⚡ CANH BAO: Phat hien rung/pha cua!"));
      
      phatHienRung = true;   // Bắt đầu trạng thái báo động
      trangThai = 0xA2;
      baoHieu(2, 200);
      
      // Giữ trạng thái báo động trong 3 giây (đủ để Master mở cửa)
      vTaskDelay(pdMS_TO_TICKS(3000));
      
      phatHienRung = false;  // Kết thúc trạng thái báo động
      
      // Đợi 2 giây "yên lặng" để servo dừng hẳn trước khi cho phép báo động tiếp
      vTaskDelay(pdMS_TO_TICKS(2000));
      
      // Xóa sạch các tín hiệu rung "rác" phát sinh trong lúc delay (do servo rung)
      while(xSemaphoreTake(xSemRung, 0) == pdTRUE); 
      Serial.println(F("Vibration sensor ready again."));
    }
  }
}

// ===================================================
// SETUP  &  LOOP
// ===================================================
void setup() {
  Serial.begin(9600);

  Wire.begin(0x08);
  Wire.onRequest(guiTrangThai);
  Wire.onReceive(nhanLenh);

  pinMode(PIN_MQ2_DIGITAL, INPUT);
  pinMode(PIN_FLAME, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_SW420, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_SW420), ngatRung, FALLING);

  // Tạo synchronization objects
  xMutexState = xSemaphoreCreateMutex();
  xSemRung    = xSemaphoreCreateBinary();
  xSemLenh    = xSemaphoreCreateBinary();

  warmupStart = millis();

  // Tạo task
  xTaskCreate(taskWarmup,    "WARMUP",  128,  NULL,  1,  NULL);
  xTaskCreate(taskDocGas,    "GAS",     128,  NULL,  2,  NULL);
  xTaskCreate(taskXuLyLenh,  "LENH",   128,  NULL,  3,  NULL);
  xTaskCreate(taskXuLyRung,  "RUNG",   128,  NULL,  3,  NULL);

  // Scheduler tự khởi động bởi thư viện (variantHooks.cpp) – KHÔNG gọi lại ở đây
}

void loop() {
  // Để trống – FreeRTOS điều khiển từ variantHooks → vApplicationIdleHook
}