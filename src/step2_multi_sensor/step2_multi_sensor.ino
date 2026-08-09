// ARC Scan - 2단계: TCA9548A 다채널 순회 폴링
// 보드: ESP32 Dev Module (WROOM-32)
// 라이브러리: VL53L1X by Pololu
// 센서 추가 시 NUM_SENSORS만 변경 (채널 0부터 순서대로 배선 전제)

#include <Wire.h>
#include <VL53L1X.h>

#define TCA_ADDR 0x70
#define NUM_SENSORS 2        // 센서 추가할 때마다 이 숫자만 변경

VL53L1X sensors[NUM_SENSORS];

void tcaSelect(uint8_t ch) {
  Wire.beginTransmission(TCA_ADDR);
  Wire.write(1 << ch);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Wire.begin();   // WROOM 기본: SDA=21, SCL=22

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
}

void loop() {
  for (uint8_t i = 0; i < NUM_SENSORS; i++) {
    tcaSelect(i);
    uint16_t mm = sensors[i].read();
    Serial.print(mm);
    if (i < NUM_SENSORS - 1) Serial.print("\t");
  }
  Serial.println();
}
