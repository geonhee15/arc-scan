// ARC Scan - I2C 진단 스케치
// 메인 버스 스캔 후 TCA 채널 0을 열고 센서(0x29) 응답 확인

#include <Wire.h>
#define TCA_ADDR 0x70

void tcaSelect(uint8_t ch) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

bool ping(uint8_t addr) {
  Wire.beginTransmission(addr);
  return (Wire.endTransmission() == 0);
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Wire.begin();

  Serial.println("=== 1. 메인 버스 스캔 ===");
  bool foundAny = false;
  for (uint8_t a = 1; a < 127; a++) {
    if (ping(a)) {
      Serial.print("발견: 0x");
      Serial.println(a, HEX);
      foundAny = true;
    }
  }
  if (!foundAny) Serial.println("아무것도 없음");

  Serial.println("=== 2. 채널 0 열고 재스캔 ===");
  tcaSelect(0);
  delay(10);
  if (ping(0x29)) Serial.println("0x29 응답함 -> 센서 살아있음");
  else Serial.println("0x29 응답 없음 -> 센서 라인 문제");
}

void loop() {}
