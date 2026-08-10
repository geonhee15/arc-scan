// ARC Scan - 4c단계: 고품질 스캔 (step4b 대비 변경점)
//  1) ANGLE_STEP 2 -> 1 : 각도 해상도 2배 (한 스윕 180 x 7 = 1260포인트)
//  2) ROI 16x16 -> 4x4  : 빔 폭 약 27도 -> 약 15-20도, 모서리가 날카로워짐
//  3) 측정 버짓 50ms    : 좁아진 ROI의 신호 감소 보상
// 트레이드오프: 스캔이 느려짐 (편도 약 30-40초), ROI 축소로 최대 사거리 소폭 감소
// WIFI_SSID / WIFI_PASS 수정 필수 (커밋 시 플레이스홀더 유지)

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <VL53L1X.h>
#include <ESP32Servo.h>

const char* WIFI_SSID = "여기에_와이파이_이름";
const char* WIFI_PASS = "여기에_비밀번호";

#define TCA_ADDR 0x70
#define NUM_SENSORS 7
#define SERVO_PIN 13
#define ANGLE_STEP 1
#define SETTLE_MS 30

VL53L1X sensors[NUM_SENSORS];
WebSocketsServer webSocket(81);
Servo servo;

bool scanning = false;
int angle = 0, dir = 1;

void tcaSelect(uint8_t ch) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

void onWsEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t len) {
  if (type == WStype_TEXT) {
    String cmd = String((char*)payload).substring(0, len);
    if (cmd == "start")  scanning = true;
    if (cmd == "stop")   scanning = false;
    if (cmd == "center") { scanning = false; servo.write(90); }
  }
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Wire.begin();

  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    tcaSelect(i);
    sensors[i].setTimeout(500);
    if (!sensors[i].init()) {
      Serial.print("채널 "); Serial.print(i); Serial.println(" 초기화 실패!");
      while (1);
    }
    sensors[i].setDistanceMode(VL53L1X::Long);
    sensors[i].setROISize(4, 4);              // 빔 좁히기 (핵심 변경)
    sensors[i].setMeasurementTimingBudget(50000);
    sensors[i].startContinuous(50);
  }

  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(0);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi 연결 중");
  while (WiFi.status() != WL_CONNECTED) { delay(500); Serial.print("."); }
  Serial.println();
  Serial.print("뷰어에 입력할 IP: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(onWsEvent);
}

void loop() {
  webSocket.loop();
  if (!scanning) { delay(5); return; }

  servo.write(angle);
  delay(SETTLE_MS);

  uint16_t d[NUM_SENSORS];
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    tcaSelect(i);
    d[i] = sensors[i].read();
    if (sensors[i].timeoutOccurred()) d[i] = 0;
  }

  String json = "{\"a\":" + String(angle) + ",\"d\":[";
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    json += d[i];
    if (i < NUM_SENSORS - 1) json += ",";
  }
  json += "]}";
  webSocket.broadcastTXT(json);

  angle += dir * ANGLE_STEP;
  if (angle >= 180) { angle = 180; dir = -1; }
  if (angle <= 0)   { angle = 0;   dir = 1; }
}
