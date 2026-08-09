# ARC Scan — Claude Code 운영 지침

## 프로젝트

회전식 ToF 라이다 3D 룸 스캐너. ESP32 WROOM + TCA9548A + VL53L1X x7 + MG996R.
회로 설계와 코드 작성은 claude.ai 대화에서 진행하고, 이 저장소는 그 결과물의 기록이다.

## 네 역할

사용자가 다운로드한 파일(보통 ~/Downloads의 zip 또는 개별 파일)을 이 리포에
반영하고 커밋/푸시하는 것. 요청 예: "다운로드 폴더의 arc-scan zip 풀어서 반영하고 커밋해".

## 규칙

- 파일 반영 시 기존 파일은 덮어쓰되, docs/troubleshooting.md는 **덮어쓰지 말고
  내용 비교 후 새 항목만 추가** (기록 유실 방지)
- 커밋 메시지는 한국어로, "단계: 한 일" 형식
  예) `1단계: 센서 2개 채널 순회 폴링 성공`
  예) `docs: SDA/SCL 교차 트러블슈팅 추가`
- 사용자가 커밋 메시지를 지정하면 그걸 그대로 사용
- push는 origin main으로. force push 금지
- 스케치(.ino)는 src/ 아래 단계별 폴더 구조 유지 (Arduino IDE가 폴더명=파일명을
  요구하므로 폴더명과 .ino 파일명은 항상 일치시킬 것)
- 리포 구조나 코드 내용을 임의로 리팩터링하지 말 것. 반영 요청받은 파일만 처리

## 구조

- src/ — 단계별 스케치 (step1_single_sensor, step2_multi_sensor, ...)
- diagnostics/ — 디버깅 스케치
- docs/ — wiring.md(배선표), troubleshooting.md(증상→원인→해결 기록)
