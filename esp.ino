// Khai báo các macro để thư viện BLYNK sử dụng để truy cập
// vào ứng dụng và Web của bạn
#define BLYNK_TEMPLATE_ID "xxxxxx"
#define BLYNK_TEMPLATE_NAME "xxxxx"
#define BLYNK_AUTH_TOKEN "xxxxxx"
#define BLYNK_PRINT Serial

// Khai báo các thư viện sử dụng
// Thư viện chuyển String thành JSON và ngược lại
#include <ArduinoJson.h>

// Thư hiện hỗ trợ ESP8266 truy cập vào WIFI và BLYNK
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "P410";
char pass[] = "hoianhHung";

// Khai báo 2 chân digital nối với quạt
const byte fan_1 = 5;

// Nếu như giá trị tại chân ảo V3 thay đổi
// thì lệnh trong hàm này sẽ được thực thi
BLYNK_WRITE(V3) {
  int state = param.asInt();
  if (state) {
    digitalWrite(fan_1, 1);
  } else if (!state) {
    digitalWrite(fan_1, 0);
  }
}

void setup() {
  Serial.begin(115200);
  // Kết nối với BLYNK
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
  Blynk.run(); // Tạo kết nối liên tục tới BLYNK
  // Nếu nhận được dữ liệu từ Arduino
  if (Serial.available() > 0) {
    // Đảm bảo bộ đệm đủ lớn để chứa toàn bộ chuỗi JSON
    String stringData = Serial.readStringUntil('\n');
    if (stringData.length() > 0) {
      DynamicJsonDocument doc(512);
      // Kiểm tra tính hợp lệ của chuỗi JSON
      DeserializationError error = deserializeJson(doc, stringData);
      if (!error) {
        // Tiếp tục xử lý dữ liệu nếu chuỗi JSON hợp lệ
        String smoke_val_1 = doc["smoke_val_1"];
        String smoke_val_2 = doc["smoke_val_2"];
        String smoke_val_3 = doc["smoke_val_3"];
        String gas_val = doc["gas_val"];  

        // Ghi giá trị lên các Virtual Pin để hiển thị ra màn hình
        Blynk.virtualWrite(V0, smoke_val_1.toFloat());
        Blynk.virtualWrite(V1, smoke_val_2.toFloat());
        Blynk.virtualWrite(V2, smoke_val_3.toFloat());
        Blynk.virtualWrite(V4, gas_val.toFloat());
      }
    }
  }
}
