// ARC Scan - 센서 스페이서 (헤더 핀 간섭 해결)
// 보드 뒷면과 마스트 장착면 사이에 끼움. 헤더 핀 줄이 지나는 자리에 골.
// 조립: 센서 → M2로 스페이서에 고정 (스페이서 파일럿에 태핑),
//       스페이서 뒷면 → 마스트 장착면에 글루건 접착

$fn = 32;

// 마스트와 동일 실측값
sens_hole_span = 20.0;
sens_board_len = 24.5;
sens_board_wid = 10.0;

spacer_t   = 10;    // 두께 (헤더 핀 길이보다 크게)
pad        = 4;     // 보드보다 큰 여유
chan_w     = 7;     // 헤더 골 폭
pilot_d    = 1.8;   // M2 태핑 파일럿

w = sens_board_len + pad;   // 28.5
h = sens_board_wid + 6;     // 16 (장착면과 동일)

// 7개 한 판에
for (ix = [0:3]) for (iy = [0:1]) {
  n = ix + iy*4;
  if (n < 7)
    translate([ix*(w+6), iy*(h+6), 0])
    difference() {
      cube([w, h, spacer_t]);
      // 헤더 골: 아래쪽 긴 변을 따라 관통
      translate([-1, -1, -1]) cube([w+2, chan_w+1, spacer_t+2]);
      // 파일럿 구멍 2개 (보드 구멍 위치 = 세로 중앙 기준)
      for (s = [-1, 1])
        translate([w/2 + s*sens_hole_span/2, chan_w + (h-chan_w)/2, -1])
          cylinder(d = pilot_d, h = spacer_t+2);
    }
}
