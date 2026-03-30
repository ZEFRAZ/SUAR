#include "src/screens/program41.h"
#include "src/algo/bresellipse.h"
#include "src/algo/dda.h"
#include "src/ui/back_button.h"
#include "src/ui/cartesian.h"
#include "src/ui/primitives.h"
#include "coords.h"
#include "screen_type.h"
#include "raylib.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif

// Transformasi 2D: Translasi
void TranslateEllipse(int *cx, int *cy, int tx, int ty) {
    *cx += tx;
    *cy += ty;
}

// Transformasi 2D: Scaling (relative to origin)
void ScaleEllipse(int *rx, int *ry, float sx, float sy) {
    *rx = (int)(*rx * sx);
    *ry = (int)(*ry * sy);
}

// Transformasi 2D: Rotasi titik (cx, cy) terhadap origin
void RotatePoint(int *cx, int *cy, float angle_deg) {
    float angle_rad = angle_deg * PI / 180.0f;
    float cos_a = cosf(angle_rad);
    float sin_a = sinf(angle_rad);
    int new_x = (int)(*cx * cos_a - *cy * sin_a);
    int new_y = (int)(*cx * sin_a + *cy * cos_a);
    *cx = new_x;
    *cy = new_y;
}

// Gambar elips dengan transformasi (semua dalam koordinat Kartesian)
void DrawTransformedEllipse(int cx, int cy, int rx, int ry, Color color) {
    int scx = CS_X(cx);
    int scy = CS_Y(cy);
    int srx = rx * G_TickStep;
    int sry = ry * G_TickStep;
    BresenhamEllipse(scx, scy, srx, sry, color);
    DrawDot(scx, scy, 3, color);
}

void DrawProgram41(void) {
    G_OriginX  = SCREEN_W / 2;
    G_OriginY  = SCREEN_H / 2 + 30;
    G_TickStep = 30;

    ClearBackground((Color){15,15,25,255});
    DrawCartesianGrid(DDALine);

    // Elips asli (referensi)
    int orig_cx = 0, orig_cy = 0;
    int orig_rx = 3, orig_ry = 2;
    DrawTransformedEllipse(orig_cx, orig_cy, orig_rx, orig_ry, (Color){100,100,100,255});

    // === TRANSFORMASI 1: TRANSLASI ===
    int t1_cx = orig_cx, t1_cy = orig_cy;
    int t1_rx = orig_rx, t1_ry = orig_ry;
    TranslateEllipse(&t1_cx, &t1_cy, 5, 3);  // Translasi (+5, +3)
    DrawTransformedEllipse(t1_cx, t1_cy, t1_rx, t1_ry, (Color){255,100,100,255});

    // === TRANSFORMASI 2: SCALING ===
    int s1_cx = -6, s1_cy = 2;
    int s1_rx = orig_rx, s1_ry = orig_ry;
    ScaleEllipse(&s1_rx, &s1_ry, 1.5f, 2.0f);  // Scale (1.5x, 2.0x)
    DrawTransformedEllipse(s1_cx, s1_cy, s1_rx, s1_ry, (Color){100,255,100,255});

    // === TRANSFORMASI 3: ROTASI ===
    int r1_cx = 0, r1_cy = -4;
    int r1_rx = 4, r1_ry = 2;
    // Gambar elips sebelum rotasi (ghost)
    DrawTransformedEllipse(r1_cx, r1_cy, r1_rx, r1_ry, (Color){60,60,80,255});
    // Rotasi pusat elips 45 derajat
    RotatePoint(&r1_cx, &r1_cy, 45);
    // Untuk rotasi elips, kita perlu menggambar titik-titik elips yang dirotasi
    // Karena BresenhamEllipse tidak mendukung rotasi intrinsik, kita tunjukkan
    // rotasi dengan menggambar elips di posisi yang dirotasi
    int scx = CS_X(r1_cx);
    int scy = CS_Y(r1_cy);
    BresenhamEllipse(scx, scy, r1_rx * G_TickStep, r1_ry * G_TickStep, (Color){100,100,255,255});
    DrawDot(scx, scy, 3, (Color){100,100,255,255});

    // === TRANSFORMASI 4: KOMBINASI (Translate + Scale) ===
    int k1_cx = -5, k1_cy = -3;
    int k1_rx = 2, k1_ry = 1;
    ScaleEllipse(&k1_rx, &k1_ry, 2.0f, 3.0f);  // Scale dulu
    // Translasi sudah dihitung di posisi (-5, -3)
    DrawTransformedEllipse(k1_cx, k1_cy, k1_rx, k1_ry, (Color){255,200,100,255});

    // Header
    DrawRectangle(0, 0, SCREEN_W, 56, (Color){0,0,0,210});
    DrawBackButton();
    DrawText("PROGRAM 41 - 2D Transformations with Bresenham Ellipse", 135, 10, 18, WHITE);
    DrawText("Translasi: (x+tx, y+ty) | Scaling: (x*sx, y*sy) | Rotasi: (x*cos(a)-y*sin(a), x*sin(a)+y*cos(a))", 135, 33, 12, LIGHTGRAY);

    // Legenda
    DrawRectangle(SCREEN_W-300, 60, 292, 140, (Color){0,0,0,185});
    DrawText("LEGENDA (koordinat Kartesian):", SCREEN_W-293, 66, 12, WHITE);
    
    int ly = 84;
    DrawText("Elips Asli: (0,0) rx=3, ry=2", SCREEN_W-290, ly, 11, (Color){100,100,100,255}); ly += 20;
    DrawText("Translasi (+5,+3): merah", SCREEN_W-290, ly, 11, (Color){255,100,100,255}); ly += 20;
    DrawText("Scale (1.5x,2x) @(-6,2): hijau", SCREEN_W-290, ly, 11, (Color){100,255,100,255}); ly += 20;
    DrawText("Rotasi 45deg @(0,-4): biru", SCREEN_W-290, ly, 11, (Color){100,100,255,255}); ly += 20;
    DrawText("Kombinasi T+S: kuning", SCREEN_W-290, ly, 11, (Color){255,200,100,255});

    DrawCoordInfo("2D Transform");
    DrawText("[ESC/BACKSPACE] Kembali  |  Klik BACK", 12, SCREEN_H-22, 13, DARKGRAY);
}
