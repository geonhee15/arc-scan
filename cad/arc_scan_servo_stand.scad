// ARC Scan - MG996R 서보 스탠드
// 서보를 출력축이 위를 보게 세워서 고정하는 받침대
// MG996R 표준 치수 기반. 출력 전 본인 서보 실측으로 아래 변수 확인 권장

$fn = 48;

// ===== 실측 확인 변수 =====
servo_len   = 40.7;   // 서보 몸통 길이 (플랜지 제외)
servo_wid   = 20.0;   // 서보 몸통 폭
clearance   = 0.4;    // 끼움 여유
hole_span_l = 49.5;   // 플랜지 나사구멍 간격 (긴 방향, 대각 아님)
hole_span_w = 10.0;   // 플랜지 나사구멍 간격 (짧은 방향)
flange_h    = 27.0;   // 몸통 바닥에서 플랜지 아랫면까지 높이
screw_d     = 2.2;    // 나사 구멍 지름 (M2.5 셀프태핑 기준)

// ===== 스탠드 형상 =====
base_w = 84; base_d = 54; base_t = 4;   // 바닥판
body_w = servo_len + clearance;
body_d = servo_wid + clearance;
wall   = 5;

difference() {
  union() {
    // 바닥판
    translate([-base_w/2, -base_d/2, 0]) cube([base_w, base_d, base_t]);
    // 서보 포켓 벽체 (플랜지가 벽 윗면에 얹힘)
    translate([-(body_w/2+wall), -(body_d/2+wall), 0])
      cube([body_w + 2*wall, body_d + 2*wall, base_t + flange_h]);
  }
  // 서보 몸통 관통 포켓
  translate([-body_w/2, -body_d/2, base_t])
    cube([body_w, body_d, flange_h + 1]);
  // 케이블 빠져나갈 홈
  translate([-body_w/2 - wall - 1, -5, base_t])
    cube([wall + 2, 10, flange_h + 1]);
  // 플랜지 나사 파일럿 구멍 4개
  for (sx = [-1, 1]) for (sy = [-1, 1])
    translate([sx*hole_span_l/2, sy*hole_span_w/2, base_t + flange_h - 12])
      cylinder(d = screw_d, h = 13);
  // 바닥 고정용 구멍 (책상 클램프/나사, 선택)
  for (sx = [-1, 1])
    translate([sx*(base_w/2 - 8), 0, -1]) cylinder(d = 4.2, h = base_t + 2);
}
