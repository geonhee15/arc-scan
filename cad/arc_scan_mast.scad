// ARC Scan - 센서 마스트 (7단 틸트 장착면)
// 채널 0 = 최상단 +30°(위쪽 응시), 채널 6 = 최하단 -30°(아래쪽 응시), 10° 간격
// 장착면은 세로판이며 면의 법선이 전방(+Y)에서 위아래로 틸트됨
// 센서 보드는 장착면에 M2 셀프태핑 2개로 고정, 렌즈가 전방을 바라봄
//
// ★ 출력 전 반드시 실측해서 아래 [실측 필수] 4개 값 수정할 것 ★

$fn = 40;

// ===== [실측 필수] 센서 보드 =====
sens_hole_span = 20.0;
sens_hole_d    = 1.8;
sens_board_len = 24.5;
sens_board_wid = 10.0;

// ===== [실측 권장] 서보 혼 =====
horn_dia          = 20.0;
horn_screw_circle = 14.0;
horn_screw_d      = 2.4;  // 마스트 관통용 (나사는 혼 구멍에 태핑됨)
horn_screw_n      = 4;

// ===== 마스트 형상 =====
tilts      = [30, 20, 10, 0, -10, -20, -30]; // 채널 0(위) → 6(아래)
level_step = 26;     // 장착면 세로 간격
spine_w    = 20;     // 기둥 폭
spine_d    = 10;     // 기둥 두께 (Y)
plate_w    = sens_board_len + 4;   // 장착면 폭
plate_h    = sens_board_wid + 6;   // 장착면 높이
plate_t    = 3.6;    // 장착면 두께
standoff   = 6;      // 기둥 전면에서 장착면까지 거리
base_dia   = 36;
base_t     = 6;
mast_h     = base_t + level_step * 7 + 6;

// --- 세로 장착판: 면 중심이 원점, 법선 +Y, 구멍은 Y로 관통
module face_plate() {
  difference() {
    translate([-plate_w/2, -plate_t, -plate_h/2])
      cube([plate_w, plate_t, plate_h]);
    for (s = [-1, 1])
      translate([s * sens_hole_span/2, -plate_t - 1, 0])
        rotate([-90, 0, 0]) cylinder(d = sens_hole_d, h = plate_t + 2);
  }
}

module mast_body() {
  // 바닥 디스크 (서보 혼 결합)
  difference() {
    cylinder(d = base_dia, h = base_t);
    translate([0, 0, -0.01]) cylinder(d = horn_dia + 0.6, h = 2);
    translate([0, 0, -1]) cylinder(d = 12, h = base_t + 2); // 허브 통과
    for (a = [0 : 360/horn_screw_n : 359])
      rotate([0, 0, a]) translate([horn_screw_circle/2, 0, -1])
        cylinder(d = horn_screw_d, h = base_t + 2);
  }
  // 기둥
  translate([-spine_w/2, -spine_d, base_t])
    cube([spine_w, spine_d, mast_h - base_t]);
  // 7단: 틸트 장착판 + 기둥과 잇는 hull 웨지
  for (i = [0 : 6]) {
    z = base_t + level_step * (7 - i) - level_step/2 + 3;
    hull() {
      translate([0, standoff + plate_t, z]) rotate([tilts[i], 0, 0]) face_plate();
      translate([-spine_w/2 + 2, -spine_d + 1, z - plate_h/2 - 9])
        cube([spine_w - 4, spine_d - 2, plate_h + 9]);  // 아래로 연장: 웨지 밑면 경사를 세워 오버행 완화
    }
  }
}

// hull이 구멍을 메우므로 전체에서 구멍 재차감
module full_mast() {
difference() {
  mast_body();
  for (i = [0 : 6]) {
    z = base_t + level_step * (7 - i) - level_step/2 + 3;
    // 센서 나사 구멍 (틸트 방향 그대로 관통)
    translate([0, standoff + plate_t, z]) rotate([tilts[i], 0, 0])
      for (s = [-1, 1])
        translate([s * sens_hole_span/2, -plate_t - 8, 0])
          rotate([-90, 0, 0]) cylinder(d = sens_hole_d, h = plate_t + 16);
    // 배선 통과 구멍 (기둥 좌우 관통)
    translate([0, -spine_d/2, z]) rotate([0, 90, 0])
      cylinder(d = 5, h = spine_w + 4, center = true);
  }
}
}

// ===== 분할 출력 =====
// part: "full" | "bottom" | "top"  — 상/하 분할로 출력 높이를 절반으로
part   = "bottom";
z_cut  = 87;          // 분할 높이 (4단과 5단 사이 기둥 구간)
plug_w = 10; plug_d = 6; plug_h = 10;  // 결합 플러그 (하단에 돌출)
gap    = 0.3;         // 소켓 여유 (한쪽)

module plug_shape(extra=0, hextra=0)
  translate([-(plug_w+extra)/2, -spine_d/2 - (plug_d+extra)/2, 0])
    cube([plug_w+extra, plug_d+extra, plug_h+hextra]);

if (part == "full") full_mast();

if (part == "bottom") {
  intersection() { full_mast(); translate([-100,-100,-1]) cube([200,200,z_cut+1]); }
  translate([0,0,z_cut]) plug_shape();          // 위로 솟은 플러그
}

if (part == "top") {
  // 절단면이 베드에 닿게 z_cut만큼 내림, 바닥면에 소켓 파기
  difference() {
    translate([0,0,-z_cut])
      intersection() { full_mast(); translate([-100,-100,z_cut]) cube([200,200,200]); }
    translate([0,0,-0.01]) plug_shape(extra=2*gap, hextra=0.4);
  }
}
