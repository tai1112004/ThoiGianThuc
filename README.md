# He Thong Khoa Cua Thong Minh Da Phuong Thuc

## Tong Quan

Day la project he thong khoa cua thong minh ket hop giua:

- Ung dung di dong React Native de dieu khien va giam sat.
- Cum vi dieu khien Arduino de xu ly khoa cua, cam bien va man hinh hien thi.
- Cac co che xac thuc da phuong thuc nhu khuon mat, van tay, QR cho khach va lenh giong noi.
- Ket noi Bluetooth de gui lenh mo khoa, dong khoa, doi che do va quan ly RFID/mat khau.

Muc tieu cua he thong la tao ra mot mo hinh smart lock co kha nang:

- Mo khoa bang nhieu cach khac nhau.
- Theo doi trang thai cua, khoang cach va canh bao moi truong theo thoi gian thuc.
- Dieu khien truc tiep tu dien thoai.
- Ket hop AI de tuong tac bang ngon ngu tu nhien va ho tro xac thuc khuon mat.

## Chuc Nang Chinh

### 1. Dieu khien khoa cua tu app di dong

App cho phep nguoi dung:

- Ket noi toi thiet bi khoa bang Bluetooth LE.
- Gui cac lenh:
  - `OPEN`
  - `CLOSE`
  - `MODE:SECURITY`
  - `MODE:SAFE`
  - `MODE:AUTO`
  - `RFID:ADD`
  - `RFID:DEL`
  - `NEWPASS:<mat_khau>`
  - `STATUS`
  - `OLED:SENSOR`
- Xem log lenh gui di va phan hoi tra ve tu thiet bi.

### 2. Giam sat trang thai he thong theo thoi gian thuc

App hien thi cac thong tin:

- Trang thai ket noi Bluetooth.
- Trang thai cua:
  - Dang mo
  - Da khoa
- Che do he thong:
  - Bao mat
  - An toan
  - Tu dong
- Khoang cach do tu cam bien sieu am.
- Cac canh bao tu cam bien:
  - Gas
  - Lua
  - Rung/pha cua

### 3. Xac thuc bang van tay / sinh trac hoc

He thong co chuc nang:

- Kiem tra thiet bi co ho tro biometrics hay khong.
- Dang ky du lieu sinh trac hoc tren may.
- Xac thuc nguoi dung bang van tay / face ID cua he dieu hanh.
- Sau khi xac thuc thanh cong, app co the gui lenh `OPEN`.

Luu y: trong code hien tai, co che nay dang hoat dong theo huong xac thuc local tren thiet bi di dong.

### 4. Xac thuc bang khuon mat

He thong cho phep:

- Mo camera truoc/sau.
- Quet QR trong cung man hinh camera.
- Dang ky khuon mat moi bang cach chup anh va dat ten.
- Luu danh sach khuon mat da dang ky trong bo nho may.
- Chup anh moi va so sanh voi danh sach da luu de quyet dinh mo cua.

Che do khuon mat gom:

- `check`: kiem tra khuon mat de mo cua.
- `add`: dang ky khuon mat moi.
- `manage`: xem va xoa danh sach khuon mat da dang ky.

### 5. Tao ma QR tam thoi cho khach

App co the:

- Tao QR token cho khach.
- Gan timestamp vao token.
- Gioi han hieu luc trong 3 phut.
- Quet lai QR trong man hinh camera de mo cua neu hop le.

### 6. Dieu khien bang giong noi co AI

Nguoi dung co the:

- Noi truc tiep vao micro.
- Hoac nhap text tu ban phim.
- Gui cau lenh len Groq AI.
- AI tra ve JSON gom:
  - Noi dung phan hoi bang tieng Viet.
  - Lenh dieu khien BLE neu can.

Vi du:

- "Mo cua" -> `OPEN`
- "Dong cua" -> `CLOSE`
- "Bat che do bao mat" -> `MODE:SECURITY`
- "Them the" -> `RFID:ADD`
- "Doi mat khau thanh 5678" -> `NEWPASS:5678`

Ngoai cac lenh dieu khien, AI con co the tra loi hoi dap thong thuong.

### 7. Quan ly RFID va mat khau

He thong phan cung cho phep:

- Them the RFID moi.
- Xoa the RFID.
- Mo cua bang the hop le.
- Mo cua bang keypad neu nhap dung mat khau.
- Doi mat khau tu app.

### 8. Hien thi OLED va canh bao ngoai vi

Board OLED hien thi:

- Mat giao dien chinh cua he thong.
- Trang thai cua dang mo hay da khoa.
- Huong dan them/xoa the RFID.
- Thong bao ket qua thao tac.
- Trang thai nguy hiem.
- Trang thong tin cam bien chi tiet.

## Kien Truc Tong The

Project duoc chia thanh 2 khoi lon:

### A. Ung dung di dong

Thu muc chinh:

- `app/KhoaCuaApp`

Thanh phan chinh:

- `App.tsx`
  - Man hinh tong.
  - Dieu phoi ket noi BLE.
  - Dieu huong sang camera va voice screen.
  - Hien thi dashboard, log va cac nut dieu khien.
- `components/CameraScreen.tsx`
  - Camera.
  - Quet QR.
  - Dang ky va xac thuc khuon mat.
- `components/VoiceCommandScreen.tsx`
  - Chat UI cho AI.
  - Thu am.
  - Chuyen doi speech -> text -> AI -> lenh BLE.
- `components/WebViewServices.tsx`
  - Tao WebView an.
  - Nap `face-api.js`.
  - Xu ly compare khuon mat.
  - Xu ly thu am bang Web Speech API / Media flow.
- `services/biometricService.ts`
  - Bao goi sinh trac hoc.
- `services/faceRecognitionService.ts`
  - Luu khuon mat va so sanh khuon mat.
- `services/groqService.ts`
  - Goi Groq Chat API.
  - Goi Groq Whisper transcription.

### B. Firmware Arduino

Thu muc chinh:

- `uno1/sketch_mar19a/uno1/uno1.ino`
- `uno2/sketch_mar19b/uno2/uno2.ino`
- `uno3/sketch_mar19c/uno3/uno3.ino`

Mo hinh phan tach vai tro:

- `UNO 1`: board master
  - Dieu khien servo khoa.
  - Doc keypad.
  - Doc RFID.
  - Nhan lenh tu Bluetooth.
  - Giao tiep I2C voi 2 board con.
- `UNO 2`: board canh bao
  - Xu ly MQ-2.
  - Xu ly cam bien lua.
  - Xu ly cam bien rung.
  - Dieu khien buzzer.
- `UNO 3`: board giao dien
  - Do khoang cach bang sieu am.
  - Hien thi OLED.
  - Nhan du lieu trang thai qua I2C.

## Luong Hoat Dong He Thong

### 1. Luong dieu khien bang app

1. Nguoi dung thao tac tren app.
2. App ket noi Bluetooth den module khoa.
3. App gui chuoi lenh den `UNO 1`.
4. `UNO 1` xu ly lenh:
   - Mo/dong khoa.
   - Doi che do.
   - Bat admin mode de them/xoa RFID.
   - Cap nhat mat khau.
5. `UNO 1` doc du lieu tu `UNO 2` va `UNO 3`.
6. `UNO 1` gui chuoi `STATUS:*` tra lai app.
7. App cap nhat dashboard va log.

### 2. Luong canh bao cam bien

1. `UNO 2` lien tuc doc gas, lua, rung.
2. Khi co su kien bat thuong:
   - Dat ma canh bao.
   - Bat buzzer.
3. `UNO 1` doc ma canh bao qua I2C.
4. `UNO 1` dua ra logic mo khoa tu dong trong mot so tinh huong.
5. App nhan chuoi `STATUS` co thong tin `ALERT`.
6. `UNO 3` hien thi man hinh canh bao tren OLED.

### 3. Luong mo khoa bang khuon mat

1. Nguoi dung mo man hinh camera.
2. App chup anh.
3. Danh sach khuon mat tham chieu duoc tai tu bo nho may.
4. `WebViewServices` dung `face-api.js` de so sanh descriptor.
5. Neu trung khop:
   - App thong bao thanh cong.
   - Gui lenh `OPEN`.

### 4. Luong dieu khien bang giong noi

1. Nguoi dung noi vao micro.
2. WebView hoac Groq transcription chuyen am thanh thanh text.
3. Text duoc gui len Groq Chat API.
4. AI tra ve JSON chua:
   - `text`
   - `command`
5. App doc phan hoi bang TTS.
6. Neu co `command`, app gui lenh BLE den khoa.

## Cong Nghe Dang Su Dung

Phan nay mo ta theo tung lop de thay ro project dang dung gi va dung de lam gi.

### 1. Frontend mobile

#### React Native 0.84.1

Dung de xay dung ung dung di dong da nen tang.

Vai tro trong project:

- Tao giao dien nguoi dung.
- Quan ly state.
- Xu ly dieu huong man hinh don gian bang state noi bo.
- Goi API, Bluetooth, camera, microphone, biometrics.

#### React 19.2.3

Dung cho:

- Component model.
- `useState`, `useEffect`, `useRef`.
- Quan ly state giao dien va luong xu ly bat dong bo.

#### TypeScript 5.x

Dung de:

- Dinh nghia type cho component, props, ket qua service.
- Giam loi runtime khi phat trien.
- Tang kha nang bao tri va mo rong.

### 2. Ket noi thiet bi

#### react-native-ble-manager

Dung de:

- Khoi tao BLE manager.
- Connect vao thiet bi theo MAC.
- Retrieve services / characteristics.
- Bat notify.
- Ghi du lieu xuong thiet bi.

Trong project, day la cau noi chinh giua app va khoa cua.

#### react-native-bluetooth-classic

Thu vien nay da duoc khai bao dependency. Trong code hien tai, luong chinh dang su dung `react-native-ble-manager`.

No cho thay project da tung can nhac hoac dang du phong cho Bluetooth Classic.

### 3. Camera va nhan dien

#### react-native-vision-camera

Dung de:

- Truy cap camera truoc/sau.
- Chup anh.
- Quet QR code trong luong camera.

No la thanh phan trung tam cho man hinh khuon mat.

#### face-api.js

Dung de:

- Phat hien khuon mat.
- Tim facial landmarks.
- Tao face descriptor.
- So sanh descriptor giua anh moi va anh da luu.

Trong project nay, `face-api.js` dang chay ben trong mot `WebView` an.

#### react-native-webview

Dung de:

- Chay mot trang HTML/JS an trong app.
- Nap `face-api.js`.
- Thuc thi so sanh khuon mat.
- Xu ly mot phan luong voice trong moi truong web.

Day la giai phap cau noi giua React Native va cac thu vien JS web khong chay truc tiep tot trong native runtime.

#### react-native-fs

Dung de:

- Tao file HTML cho WebView.
- Luu anh khuon mat da dang ky.
- Doc/ghi `index.json` cua danh sach khuon mat.
- Luu metadata dang ky biometrics.
- Tao file tam audio khi transcription.

### 4. Sinh trac hoc

#### react-native-biometrics

Dung de:

- Kiem tra sensor biometrics.
- Tao keypair.
- Yeu cau nguoi dung xac thuc bang sinh trac hoc he dieu hanh.

Trong project nay no duoc dung cho xac thuc local truoc khi gui lenh `OPEN`.

### 5. AI va xu ly ngon ngu tu nhien

#### Groq API

Project dang dung Groq cho 2 viec:

- Chat completion de hieu lenh ngon ngu tu nhien.
- Audio transcription de chuyen audio thanh text.

#### Axios

Dung de:

- Goi HTTP request toi Groq API.
- Gui payload JSON cho chat completion.
- Gui `multipart/form-data` cho transcription.

#### Mo hinh AI trong code

Theo cau hinh hien tai:

- Chat model: `llama-3.1-8b-instant`
- Speech transcription: `whisper-large-v3-turbo`

Vai tro:

- Phan tich cau noi tieng Viet.
- Map cau lenh tu nhien thanh lenh BLE.
- Tra loi cac cau hoi thong thuong.

### 6. Xu ly giong noi va am thanh

#### Web Speech API

Trong `WebViewServices`, project uu tien su dung Web Speech API neu kha dung de lay text tu giong noi nhanh hon.

#### react-native-tts

Dung de:

- Doc phan hoi AI bang giong noi.
- Tao trai nghiem tro ly ao ngay trong app.

#### @react-native-voice/voice

Thu vien nay dang co trong dependency va co service rieng `speechService.ts`, tuy nhien luong voice screen hien tai dang di qua `WebViewServices` va Groq transcription la chinh.

### 7. QR va giao dien bo tro

#### react-native-qrcode-svg

Dung de:

- Tao ma QR cho khach.
- Hien thi token xac thuc tam thoi tren app.

#### react-native-svg

La dependency phu tro de render QR code va cac thanh phan SVG.

#### react-native-safe-area-context

Dung de xu ly safe area tren cac thiet bi co notch / khu vuc he thong.

### 8. Native mobile

#### Android

Project co:

- `AndroidManifest.xml`
- `build.gradle`
- `MainActivity.kt`
- `MainApplication.kt`

Vai tro:

- Cap permission:
  - Internet
  - Camera
  - Record audio
  - Bluetooth
  - Bluetooth scan/connect
  - Location
  - Biometric
- Cau hinh React Native host va native app shell.

#### iOS

Project co:

- `Info.plist`
- `AppDelegate.swift`
- `Podfile`

Vai tro:

- Cau hinh bundle iOS.
- Cau hinh native startup.
- Quan ly dependency CocoaPods.

### 9. Firmware va Embedded

#### Arduino C/C++

Toan bo phan firmware duoc viet bang Arduino C/C++.

#### Arduino_FreeRTOS

Dung de:

- Tach he thong thanh nhieu task.
- Giu cho doc cam bien, xu ly lenh, buzzer va OLED khong chan nhau.
- Quan ly semaphore / mutex trong giao tiep va ngat.

#### Wire

Dung cho giao tiep I2C giua:

- `UNO 1` <-> `UNO 2`
- `UNO 1` <-> `UNO 3`

#### SoftwareSerial

Dung tren `UNO 1` de giao tiep voi module Bluetooth.

#### Servo

Dung de dieu khien co cau khoa cua.

#### MFRC522

Dung de doc the RFID.

#### Keypad

Dung de nhap mat khau truc tiep tu ban phim.

#### EEPROM

Dung de:

- Luu danh sach UID the RFID.
- Giu du lieu sau khi mat nguon.

#### U8g2

Dung tren `UNO 3` de dieu khien man hinh OLED.

## Cau Truc Thu Muc

```text
.
|-- app/
|   `-- KhoaCuaApp/
|       |-- App.tsx
|       |-- components/
|       |   |-- CameraScreen.tsx
|       |   |-- VoiceCommandScreen.tsx
|       |   `-- WebViewServices.tsx
|       |-- services/
|       |   |-- biometricService.ts
|       |   |-- faceRecognitionService.ts
|       |   |-- groqService.ts
|       |   |-- speechService.ts
|       |   `-- textToSpeechService.ts
|       |-- android/
|       `-- ios/
|-- uno1/
|   `-- sketch_mar19a/uno1/uno1.ino
|-- uno2/
|   `-- sketch_mar19b/uno2/uno2.ino
|-- uno3/
|   `-- sketch_mar19c/uno3/uno3.ino
`-- README.md
```

## Du Lieu Va Giao Thuc

### Bluetooth app <-> khoa

App gui chuoi command text xuong thiet bi, vi du:

```text
OPEN
CLOSE
STATUS
MODE:SECURITY
RFID:ADD
NEWPASS:1234
```

Thiet bi tra ve chuoi dang:

```text
STATUS:OPEN,DIST:120,MODE:2,ALERT:A0
```

Y nghia:

- `STATUS:OPEN` hoac `STATUS:CLOSED`
- `DIST:<mm>`
- `MODE:0|1|2`
- `ALERT:A0|A1|A2|A3`

Quy uoc canh bao:

- `A0`: binh thuong
- `A1`: gas
- `A2`: rung
- `A3`: lua

### I2C giua cac Arduino

- `UNO 2` dia chi `0x08`
- `UNO 3` dia chi `0x09`

`UNO 1` la master, doc va gui lenh den 2 board con.

## Uu Diem Ky Thuat Cua Project

- Tich hop nhieu phuong thuc mo khoa trong cung mot he thong.
- Tach biet ro giua app mobile va firmware.
- Co dashboard theo doi trang thai real-time.
- Co AI chat va voice command.
- Co co che quan ly RFID, password, QR va biometrics.
- Phan firmware co chia task FreeRTOS thay vi viet theo loop don.

## FreeRTOS Tu Ly Thuyet Den Ung Dung Trong Project

### 1. FreeRTOS la gi

FreeRTOS la mot he dieu hanh thoi gian thuc sieu nhe danh cho vi dieu khien va he thong nhung.

Muc tieu cua no la:

- Chia chuong trinh thanh nhieu task.
- Quan ly task nao duoc chay truoc, task nao chay sau.
- Dam bao cac cong viec quan trong duoc xu ly dung luc.
- Giam tinh trang viet tat ca logic trong mot `loop()` dai va kho bao tri.

No phu hop voi project cua ban vi he thong cua ban khong chi co 1 cong viec duy nhat. Ban dang co:

- Doc cam bien gas.
- Doc cam bien rung.
- Xu ly buzzer.
- Xu ly servo.
- Giao tiep Bluetooth.
- Giao tiep I2C.
- Cap nhat OLED.

Neu viet tat ca trong mot `loop()` thuong, chuong trinh de bi:

- Cham phan hoi.
- Block nhau.
- Kho chia uu tien.
- Kho xu ly ngat va giao tiep nhieu thiet bi cung luc.

### 2. Khai niem ly thuyet co ban trong FreeRTOS

#### Task

Task la mot don vi cong viec doc lap.

Co the hieu don gian:

- Moi task giong nhu mot "viec can lam".
- Scheduler cua FreeRTOS se quyet dinh task nao duoc CPU chay tai moi thoi diem.

Vi du:

- Task doc gas moi 1 giay.
- Task xu ly rung khi co ngat.
- Task OLED refresh man hinh lien tuc.

#### Scheduler

Scheduler la bo lap lich cua FreeRTOS.

Vai tro:

- Chon task nao se duoc chay.
- Dua tren priority.
- Dua tren trang thai task:
  - ready
  - running
  - blocked
  - suspended

#### Priority

Moi task co muc uu tien.

Nguyen tac:

- Task priority cao hon se duoc uu tien chay truoc.
- Task priority thap hon chay khi task cao hon dang blocked hoac da yield.

Day la ly do vi sao trong he thong nhung, nhung task lien quan su kien quan trong nhu:

- rung/bao dong
- nhan lenh
- cap nhat cam bien khan

thuong dat uu tien cao hon cac task giao dien.

#### Tick

FreeRTOS van hanh dua tren "tick", co the hieu la xung nhip he dieu hanh.

Moi tick:

- scheduler co the duoc kich hoat
- task delay duoc dem thoi gian

Lenh thuong gap:

- `vTaskDelay(...)`
- `vTaskDelayUntil(...)`

#### Blocking

FreeRTOS rat manh o cho task khong can "ban CPU" lien tuc.

Task co the vao trang thai blocked khi:

- doi semaphore
- doi queue
- doi timeout
- delay mot khoang thoi gian

Nho vay CPU duoc nhuong cho task khac.

#### Semaphore

Semaphore duoc dung de dong bo giua cac task hoac giua ISR va task.

Co 2 y nghia hay dung:

- Bao hieu su kien.
- Bao ve tai nguyen dung chung.

Trong project cua ban:

- `xSemRung` dung de bao cho task xu ly rung biet la vua co ngat.
- `xSemLenh` dung de bao cho task xu ly lenh I2C biet la vua nhan lenh.

#### Mutex

Mutex la semaphore dac biet dung de bao ve tai nguyen dung chung.

Vi du:

- nhieu task cung muon giao tiep I2C
- neu khong khoa lai, du lieu de xung dot

Trong project cua ban:

- `xMutexI2C` duoc dung de bao ve bus I2C tren `UNO 1`

#### ISR va giao tiep voi task

ISR la ham xu ly ngat.

Nguyen tac quan trong:

- ISR phai ngan gon.
- Khong nen xu ly nang ben trong ISR.
- Thuong chi dat co, give semaphore, roi de task xu ly tiep.

Day la mot mau thiet ke dung chuan RTOS.

### 3. Tai sao FreeRTOS quan trong voi he thong thoi gian thuc

He thong thoi gian thuc khong phai luc nao cung co nghia "cuc nhanh", ma la:

- Xu ly dung luc.
- Co tinh du doan duoc.
- Khong de mot viec phu lam tre viec quan trong.

Trong khoa cua thong minh, day la diem rat quan trong vi:

- Bao dong rung/gas/lua phai phan hoi nhanh.
- Servo khong duoc giat do logic bi block.
- OLED va giao tiep I2C khong duoc lam hong data cua nhau.
- Bluetooth phai nhan/tra lenh on dinh.

### 4. FreeRTOS dang duoc dung the nao trong project nay

Project nay khong dung FreeRTOS theo kieu ly thuyet chung chung, ma da ung dung thuc te tren ca 3 board.

#### UNO 1: board master dieu phoi

File:

- `uno1/sketch_mar19a/uno1/uno1.ino`

Cach dung FreeRTOS:

- Tao mutex I2C:
  - `xMutexI2C`
- Tao task trung tam:
  - `taskCore`

Vai tro cua `taskCore`:

- Khoi tao Serial, Wire, SPI, RFID, Bluetooth, servo.
- Doc du lieu tu `UNO 2` va `UNO 3`.
- Xu ly RFID.
- Xu ly keypad.
- Xu ly command Bluetooth.
- Ap logic mo cua/dong cua theo alert va mode.
- Cap nhat OLED qua `UNO 3`.

Y nghia kien truc:

- `UNO 1` dang dung 1 task lon de dieu phoi toan bo nghiep vu trung tam.
- FreeRTOS o day chu yeu giup co `vTaskDelay`, mutex va mo hinh task on dinh hon `loop()`.
- `xMutexI2C` giup tranh xung dot khi nhieu doan logic cung can truy cap `Wire`.

#### UNO 2: board canh bao dung FreeRTOS ro nhat

File:

- `uno2/sketch_mar19b/uno2/uno2.ino`

Day la noi ung dung FreeRTOS ro va dep nhat trong project.

Task hien co:

- `taskWarmup`
  - Xu ly giai doan ham nong MQ-2.
  - Sau khi xong thi tu xoa bang `vTaskDelete(NULL)`.
- `taskDocGas`
  - Doc gas va lua dinh ky.
  - Chay theo chu ky bang `vTaskDelayUntil`.
- `taskXuLyLenh`
  - Xu ly lenh tu master qua I2C.
  - Doi semaphore `xSemLenh`.
- `taskXuLyRung`
  - Xu ly canh bao rung.
  - Doi semaphore `xSemRung`.

Dong bo trong `UNO 2`:

- `xSemRung`
  - ISR rung se `give` semaphore.
  - Task rung se `take` semaphore va xu ly.
- `xSemLenh`
  - I2C receive callback se `give` semaphore.
  - Task xu ly lenh se `take`.

Ngat va FreeRTOS:

- Cam bien rung dung `attachInterrupt(...)`.
- ISR `ngatRung()` khong xu ly nang.
- ISR chi `xSemaphoreGiveFromISR(...)`.
- Sau do `taskXuLyRung` xu ly logic canh bao, delay, loc rung gia.

Day la cach dung RTOS rat dung ly thuyet:

- ISR ngan gon.
- Viec nang day xuong task.
- Co uu tien ro rang.

#### UNO 3: board OLED va sieu am

File:

- `uno3/sketch_mar19c/uno3/uno3.ino`

Task hien co:

- `taskDocKhoangCach`
  - Doc cam bien sieu am theo chu ky.
- `taskOLED`
  - Refresh giao dien OLED lien tuc.

Y nghia:

- Tach doc cam bien ra khoi task ve man hinh.
- OLED duoc ve dinh ky, khong can chan logic doc khoang cach.
- Kien truc nay lam man hinh muot hon va de mo rong them screen state.

### 5. Cac API FreeRTOS dang xuat hien trong code cua ban

#### `xTaskCreate`

Dung de tao task.

Trong project:

- `UNO 1`: tao `taskCore`
- `UNO 2`: tao 4 task rieng
- `UNO 3`: tao 2 task rieng

#### `vTaskDelay`

Dung de:

- tam dung task trong mot khoang thoi gian
- nhuong CPU cho task khac

Ban dang dung cho:

- delay servo
- delay buzzer
- delay anti-rung
- chu ky refresh OLED

#### `vTaskDelayUntil`

Dung khi muon chay task theo chu ky deu.

Trong project:

- `taskWarmup`
- `taskDocGas`

Day la cach tot hon `delay()` khi muon co chu ky on dinh.

#### `xSemaphoreTake` / `xSemaphoreGiveFromISR`

Dung de:

- dong bo task voi ISR
- cho task doi su kien thay vi polling vo ich

Trong project:

- `xSemRung`
- `xSemLenh`

#### `portYIELD_FROM_ISR`

Dung sau khi ISR danh thuc task uu tien cao hon.

No giup task can xu ly gap co the chay som hon.

### 6. Loi ich thuc te cua FreeRTOS trong project nay

Neu khong dung FreeRTOS, project nay van co the chay bang `loop()`, nhung se kho giu duoc cac dac tinh sau:

- Tach logic canh bao khoi logic giao tiep.
- Xu ly ngat sach se.
- Giu chu ky doc cam bien on dinh.
- Giu OLED khong lag khi co su kien khac.
- De mo rong them task ve sau.

FreeRTOS trong project cua ban giup he thong:

- de doc
- de mo rong
- de chia trach nhiem
- on dinh hon khi co nhieu thanh phan cung hoat dong

### 7. Gioi han cua cach dung FreeRTOS hien tai

Project dang dung FreeRTOS tot, nhung van con mot vai diem co the nang cap:

- `UNO 1` hien van gom rat nhieu logic vao 1 task lon, co the tach them:
  - task BLE
  - task RFID/keypad
  - task system status
- `UNO 2` khai bao `xMutexState` nhung chua khai thac ro net.
- Chua thay queue/message buffer; hien tai chu yeu dung semaphore va bien global.
- Co the them watchdog / health monitoring neu can demo muc on dinh cao hon.

## AI Trong Project Nay

### 1. AI cua ban thuc chat gom may lop

Trong project nay, "AI" khong chi la 1 model duy nhat. No la mot cum chuc nang thong minh gom:

- AI hieu lenh ngon ngu tu nhien va tra loi hoi dap.
- AI chuyen giong noi thanh van ban.
- AI so sanh khuon mat.
- Sinh trac hoc he dieu hanh de xac thuc nguoi dung.

Noi cach khac, he thong cua ban dang co 4 nhanh thong minh:

- Voice AI
- Chat AI
- Face recognition AI
- Biometric authentication

### 2. AI voice/chat dang dung cong nghe gi

#### Groq Chat API

File:

- `app/KhoaCuaApp/services/groqService.ts`

Vai tro:

- Nhan text tu nguoi dung.
- Dua vao `SYSTEM_PROMPT`.
- Goi model chat:
  - `llama-3.1-8b-instant`
- Yeu cau model tra ve JSON:
  - `text`
  - `command`

Y nghia:

- Day la lop "AI suy nghi".
- No chuyen cau noi tu nhien thanh lenh co cau truc cho he thong khoa.

Vi du:

- "Cho toi vao nha" -> `OPEN`
- "Khoa cua lai" -> `CLOSE`
- "Doi pass thanh 5678" -> `NEWPASS:5678`

#### Groq Audio Transcription

Van trong:

- `app/KhoaCuaApp/services/groqService.ts`

Model:

- `whisper-large-v3-turbo`

Vai tro:

- Nhan audio base64.
- Ghi file tam `.webm`.
- Gui audio len Groq.
- Nhan text transcript tra ve.

No la lop "AI nghe".

#### Web Speech API

File:

- `app/KhoaCuaApp/components/WebViewServices.tsx`

Vai tro:

- Neu Web Speech API kha dung, project dung no de nhan text nhanh hon.
- Neu khong lay duoc text truc tiep, moi fallback sang Groq transcription.

Day la co che toi uu toc do.

#### react-native-tts

File lien quan:

- `app/KhoaCuaApp/components/VoiceCommandScreen.tsx`
- `app/KhoaCuaApp/services/textToSpeechService.ts`

Vai tro:

- Doc phan hoi cua AI bang giong noi.
- Day khong phai AI suy nghi, nhung la thanh phan AI experience.

### 3. Luong AI voice/chat trong project

Luong chinh:

1. Nguoi dung bam nut micro trong `VoiceCommandScreen`.
2. `webEngine.startVoice()` gui lenh vao WebView an.
3. Trong WebView:
   - Neu co `SpeechRecognition`, he thong nghe va tra text bang su kien `SPEECH_TEXT`.
   - Neu can audio thuan, WebView tra du lieu `VOICE_RESULT`.
4. `VoiceCommandScreen` nhan ket qua:
   - Neu da co text, bo qua transcription.
   - Neu moi co audio, goi `transcribeAudioToText(...)`.
5. Sau khi co text, app goi `askAI(...)`.
6. `askAI(...)` gui text va `SYSTEM_PROMPT` len Groq chat.
7. Groq tra ve JSON:
   - `text`: cau tra loi tieng Viet
   - `command`: lenh BLE hoac `null`
8. App hien thi message chat.
9. App doc cau tra loi bang TTS.
10. Neu co `command` va thiet bi da ket noi, app gui lenh sang khoa.

Y nghia kien truc:

- Speech -> NLP -> command mapping -> BLE execution

### 4. AI khuon mat dang dung cong nghe gi

#### face-api.js

File:

- `app/KhoaCuaApp/components/WebViewServices.tsx`
- `app/KhoaCuaApp/services/faceRecognitionService.ts`

Vai tro:

- Load model nhan dien khuon mat.
- Tao descriptor cho anh khuon mat.
- So sanh descriptor giua anh moi va anh da dang ky.

Cach chay:

- Chay ben trong WebView.
- App React Native truyen duong dan anh vao WebView.
- WebView xu ly va tra ket qua ve cho app.

#### react-native-vision-camera

Dung de:

- Chup anh tu camera.
- Quet QR song song trong man hinh camera.

#### react-native-fs

Dung de:

- Luu anh reference cua tung khuon mat.
- Doc index khuon mat da dang ky.

### 5. Luong AI khuon mat trong project

1. Nguoi dung mo `CameraScreen`.
2. App chup anh.
3. `verifyFaceWithAI(...)` tai danh sach khuon mat da luu.
4. Voi moi khuon mat reference:
   - Goi `webEngine.compareFace(newImagePath, face.imagePath)`.
5. Trong WebView:
   - `face-api.js` nap model.
   - Doc 2 anh.
   - Thu detect khuon mat.
   - Neu can, xoay anh de detect lai.
   - Tao descriptor.
   - Tinh khoang cach Euclidean giua 2 descriptor.
6. Neu dist nam trong nguong hop le, danh dau matched.
7. App lay ket qua tot nhat.
8. Neu co matched:
   - Thong bao thanh cong.
   - Gui `OPEN`.

Day la AI thi giac may tinh trong project cua ban.

### 6. Biometric co phai AI khong

Ve mat thuc te san pham, nguoi dung thuong xep chung vao "AI / thong minh".
Ve mat ky thuat, no khong phai la model AI do ban tu train hay tu goi cloud.

No la:

- nang luc sinh trac hoc cua he dieu hanh
- thong qua thu vien `react-native-biometrics`

Luong:

1. App kiem tra sensor biometrics.
2. Tao keypair local.
3. Khi nguoi dung xac thuc:
   - he dieu hanh mo prompt van tay/face ID
4. Neu thanh cong:
   - app cho phep gui lenh `OPEN`

No la lop xac thuc an toan local, khong phai NLP hay computer vision cloud.

### 7. Thu vien va cong nghe AI trong project, tom tat nhanh

- `axios`
  - Goi HTTP den Groq API.
- `react-native-webview`
  - Tao runtime web de chay `face-api.js` va Web Speech API.
- `face-api.js`
  - Nhung descriptor khuon mat.
- `react-native-vision-camera`
  - Camera, chup anh, quet QR.
- `react-native-fs`
  - Luu anh, file HTML, audio tam, metadata.
- `react-native-tts`
  - Doc phan hoi AI.
- `react-native-biometrics`
  - Xac thuc sinh trac hoc.
- `Groq llama-3.1-8b-instant`
  - NLP / command understanding.
- `Groq whisper-large-v3-turbo`
  - speech-to-text.
- `Web Speech API`
  - speech-to-text nhanh trong WebView neu co san.

### 8. Diem manh cua kien truc AI hien tai

- Co nhieu lop fallback.
- Voice co the di theo text truc tiep hoac transcription.
- Face recognition tach rieng khoi app runtime va chay trong WebView.
- Chat AI khong chi dieu khien ma con tra loi hoi dap.
- Toan bo AI duoc gan truc tiep voi tac vu thuc te cua khoa cua.

### 9. Han che cua AI hien tai

- Groq API key dang hardcode trong source.
- Face-api.js va weights dang tai qua internet, chua hoan toan offline.
- Luong voice dang ket hop WebView + cloud, chua phai native pipeline thuần nhat.
- Matched face hien dua tren nguong descriptor, chua co co che anti-spoofing.
- AI command parsing chua co backend xac thuc lenh nhay cam.

## Han Che Hien Tai

Day la cac diem can luu y khi tiep tuc phat trien:

- API key Groq dang duoc dat truc tiep trong source code.
- Face recognition hien tai phu thuoc WebView va tai resource tu internet.
- App README cu trong `app/KhoaCuaApp/README.md` van la README mac dinh cua React Native.
- Mot so chuoi tieng Viet trong code dang loi encoding.
- Logic trong `App.tsx` dang kha lon, nen co the tach them thanh module.
- iOS permission mo ta chua day du cho camera/microphone.

## Dinh Huong Mo Rong

Project co the mo rong theo cac huong:

- Tach backend de xac thuc biometrics/signature tren server.
- Dong bo log su kien len cloud.
- Bo sung lich su mo cua.
- Quan ly nhieu nguoi dung / nhieu vai tro.
- Ma hoa token QR va co che cap quyen chat che hon.
- Dong goi model/weights khuon mat local de giam phu thuoc internet.
- Tach state management cua app thanh cac module/doc lap hon.

## Phu Hop Cho Muc Dich Nao

Project nay phu hop cho:

- Do an nhung.
- De tai IoT.
- Do an tot nghiep ve smart home / smart lock.
- Mo hinh demo ket hop mobile + AI + embedded + sensor.
- Tai lieu trinh bay kien truc he thong thoi gian thuc.

## Ghi Chu

README nay duoc viet dua tren code hien dang co trong repo, nghia la no mo ta dung tinh trang implementation hien tai, khong chi la y tuong thiet ke.
