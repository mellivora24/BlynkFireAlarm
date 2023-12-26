#include <Servo.h>
#include <Arduino.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>

// Định nghĩa các chân Analog để đọc tín hiệu cảm biến
#define smoke_1 A3
#define smoke_2 A1
#define smoke_3 A2
#define gas_sensor A0
// Định nghĩa các chân Digital để đọc tín hiệu cảm biến
#define fire_1 8
#define fire_2 5
#define fire_3 12
// Định nghĩa các chân TX RX Module SIM
#define SIM_TX 10
#define SIM_RX 11
// Định nghĩa các chân Digital xuất tín hiệu
#define buzzer 3
#define servo_1 4
#define servo_2 9
#define servo_3 6

const String phoneNum = "0376560548";

// Khởi tạo Module SIM
SoftwareSerial SIM(SIM_TX, SIM_RX);
// Khởi tạo servo
Servo SV_1, SV_2, SV_3;

// Hàm lấy thông tin phòng cháy để tạo nội dung tin nhắn
String where_fire() {
  String res = "phong";
  if (!digitalRead(fire_1)) {
    SV_1.attach(servo_1);
    SV_1.write(180);
    delay(500);
    SV_1.detach();
    res += " 1";
  }
  if (!digitalRead(fire_2)) {
    SV_2.attach(servo_2);
    SV_2.write(180);
    delay(500);
    SV_2.detach();
    res += " 2";
  }
  if (!digitalRead(fire_3)) {
    SV_3.attach(servo_3);
    SV_3.write(180);
    delay(500);
    SV_3.detach();
    res += " 3";
  }
  return res;
}
// Hàm lấy thông tin phòng có khói để tạo nội dung tin nhắn
String where_smoke() {
  String res = "phong";
  if (analogRead(smoke_1) > 750) res += " 1";
  if (analogRead(smoke_2) > 750) res += " 2";
  if (analogRead(smoke_3) > 750) res += " 3";
  return res;
}
// Hàm gọi điện trong thời gian second nào đó
void call(int second) {
  SIM.println("AT");
  delay(300);
  SIM.print(F("ATD"));
  SIM.print(phoneNum);
  SIM.print(F(";\r\n"));
  delay(second * 1000);
  SIM.print(F("ATH;\r\n"));
}
// Hàm nhắn tin với nội dung SMS
void send(String SMS, int mode) {
  if (!mode) SMS += where_smoke();
  else if (mode == 1) SMS += where_fire();
  Serial.println(SMS);
  SIM.println("AT+CMGF=1");
  delay(1000);
  SIM.println("AT+CMGS=\"" + phoneNum + "\"\r");
  delay(1000);
  SIM.println(SMS);
  delay(100);
  SIM.println((char)26);
  delay(1000);
}

// Hàm setup() chỉ chạy một lần khi cấp nguồn
void setup() {
  // Thiết lập tốc độ cho cổng nối tiếp
  Serial.begin(115200);
  SIM.begin(9600);

  // Thiết lập chức năng cho các chân IO
  pinMode(smoke_1, INPUT);
  pinMode(smoke_2, INPUT);
  pinMode(smoke_3, INPUT);

  pinMode(fire_1, INPUT);
  pinMode(fire_2, INPUT);
  pinMode(fire_3, INPUT);

  pinMode(gas_sensor, INPUT);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, 0);  // 0 -> Tắt
}
void loop() {
  // Khai báo file JSON doc
  DynamicJsonDocument doc(512);

  // Thêm dữ liệu vào doc
  doc["smoke_val_1"] = map(analogRead(smoke_1), 460, 1023, 0, 100);
  doc["smoke_val_2"] = map(analogRead(smoke_2), 490, 1023, 0, 100);
  doc["smoke_val_3"] = map(analogRead(smoke_3), 580, 1023, 0, 100);
  doc["gas_val"] = map(analogRead(gas_sensor), 60, 1023, 0, 100);
  doc["end"] = 1;

  // Chuyển đổi file JSON thành String
  String data;
  serializeJson(doc, data);
  // Gửi dữ liệu qua UART
  Serial.println(data);

  // Nếu phát hiện khói thì nhắn tin khói ở phòng nào
  if (doc["smoke_val_1"] > 50 || doc["smoke_val_2"] > 50 || doc["smoke_val_3"] > 50) {
    send("Co khoi o ", 0);
  }
  // Nếu phát hiện cháy
  if (!digitalRead(fire_1) || !digitalRead(fire_2) || !digitalRead(fire_3)) {
    // call(20);
    send("Co chay o ", 1);
    digitalWrite(buzzer, 1);
  } else if (doc["gas_val"] > 15) {  // Nếu phát hiện khí gas thì gọi và nhắn tin
    // call(8);
    digitalWrite(buzzer, 1);
    send("Ro ri khi GAS", -1);
  } else {
    digitalWrite(buzzer, 0);
  }
}