// ARC Scan - 3단계: WiFi 웹소켓으로 7채널 거리 스트리밍
// 보드: ESP32 Dev Module (WROOM-32)
// 라이브러리: VL53L1X by Pololu, WebSockets by Markus Sattler (links2004)
//
// 사용법:
// 1. 아래 WIFI_SSID / WIFI_PASS를 집 와이파이로 수정 (2.4GHz만 지원, 5GHz 안 됨)
// 2. 업로드 후 시리얼 모니터에 뜨는 IP 주소 확인
// 3. web/viewer.html을 브라우저로 열고 그 IP 입력
// ※ 실제 와이파이 비밀번호를 넣은 파일은 커밋하지 말 것 (리포엔 플레이스홀더 유지)

#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Wire.h>
#include <VL53L1X.h>

const char* WIFI_SSID = "여기에_와이파이_이름";
const char* WIFI_PASS = "여기에_비밀번호";

#define TCA_ADDR 0x70
#define NUM_SENSORS 7
#define SEND_INTERVAL_MS 50   // 브라우저로 20Hz 전송

VL53L1X sensors[NUM_SENSORS];
uint16_t latest[NUM_SENSORS] = {0};
WebSocketsServer webSocket(81);
unsigned long lastSend = 0;

void tcaSelect(uint8_t ch) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Wire.begin();

  // 센서 초기화
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    tcaSelect(i);
    sensors[i].setTimeout(500);
    if (!sensors[i].init()) {
      Serial.print("채널 ");
      Serial.print(i);
      Serial.println(" 초기화 실패!");
      while (1);
    }
    sensors[i].setDistanceMode(VL53L1X::Long);
    sensors[i].setMeasurementTimingBudget(50000);
    sensors[i].startContinuous(50);
  }
  Serial.println("전 센서 측정 시작");

  // WiFi 연결
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("WiFi 연결 중");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("연결됨! 뷰어에 입력할 IP: ");
  Serial.println(WiFi.localIP());

  webSocket.begin();
}

void loop() {
  webSocket.loop();

  // 준비된 센서만 논블로킹으로 읽어 최신값 갱신
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    tcaSelect(i);
    if (sensors[i].dataReady()) {
      latest[i] = sensors[i].read(false);
    }
  }

  // 20Hz로 JSON 브로드캐스트: {"d":[d0,...,d6]}
  if (millis() - lastSend >= SEND_INTERVAL_MS) {
    lastSend = millis();
    String json = "{\"d\":[";
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
      json += latest[i];
      if (i < NUM_SENSORS - 1) json += ",";
    }
    json += "]}";
    webSocket.broadcastTXT(json);
  }
}
