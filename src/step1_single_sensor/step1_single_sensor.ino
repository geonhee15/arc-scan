// ARC Scan - 1단계: TCA9548A 채널 0의 VL53L1X 1개 읽기
// 보드: ESP32 Dev Module (WROOM-32)
// 라이브러리: VL53L1X by Pololu

#include <Wire.h>
#include <VL53L1X.h>

#define TCA_ADDR 0x70   // A0~A2 전부 GND일 때

VL53L1X sensor;

// 멀티플렉서 채널 선택
void tcaSelect(uint8_t ch) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Wire.begin();   // WROOM 기본: SDA=21, SCL=22

  tcaSelect(0);

  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("센서 초기화 실패! 배선 확인");
    while (1);
  }

  sensor.setDistanceMode(VL53L1X::Long);      // 최대 4m
  sensor.setMeasurementTimingBudget(50000);   // 측정당 50ms
  sensor.startContinuous(50);
  Serial.println("측정 시작");
}

void loop() {
  tcaSelect(0);
  uint16_t mm = sensor.read();
  if (sensor.timeoutOccurred()) {
    Serial.println("타임아웃");
  } else {
    Serial.println(mm);
  }
}
