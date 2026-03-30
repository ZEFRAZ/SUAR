#include "src/screens/program31.h"
#include "src/algo/bresellipse.h"
#include "src/algo/dda.h"
#include "src/ui/back_button.h"
#include "src/ui/cartesian.h"
#include "src/ui/primitives.h"
#include "coords.h"
#include "screen_type.h"
#include "raylib.h"

void DrawProgram31(void) {
    G_OriginX  = SCREEN_W / 2;
    G_OriginY  = SCREEN_H / 2 + 30;
    G_TickStep = 38;

    ClearBackground((Color){12,18,12,255});
    DrawCartesianGrid(DDALine);

    // Definisi elips: cx, cy (Kartesian), rx, ry (dalam unit Kartesian)
    struct { int cx, cy, rx, ry; Color c; const char *lbl; } ellipses[] = {
        { 0,  0, 5, 3, (Color){255,120,80,255}, "Elips 1: rx=5, ry=3 (pusat)" },
        {-4,  2, 2, 3, (Color){80,220,255,255}, "Elips 2: rx=2, ry=3 (-4,2)" },
        { 5, -3, 3, 2, (Color){255,230,60,255}, "Elips 3: rx=3, ry=2 (5,-3)" },
        {-3, -4, 4, 3, (Color){180,255,100,255}, "Elips 4: rx=4, ry=3 (-3,-4)" },
    };

    for (int i = 0; i < 4; i++) {
        // Konversi pusat Kartesian ke layar
        int scx = CS_X(ellipses[i].cx);
        int scy = CS_Y(ellipses[i].cy);
        // Konversi radius ke piksel
        int srx = ellipses[i].rx * G_TickStep;
        int sry = ellipses[i].ry * G_TickStep;

        BresenhamEllipse(scx, scy, srx, sry, ellipses[i].c);
        // Gambar titik pusat
        DrawDot(scx, scy, 3, ellipses[i].c);
    }

    DrawRectangle(0, 0, SCREEN_W, 56, (Color){0,0,0,210});
    DrawBackButton();
    DrawText("PROGRAM 31 - Bresenham Ellipse Algorithm", 135, 10, 18, WHITE);
    DrawText("d1=ry²-rx²ry+rx²/4  |  d2=ry²(x+0.5)²+rx²(y-1)²-rx²ry²  [integer-only]", 135, 33, 12, LIGHTGRAY);

    DrawRectangle(SCREEN_W-280, 60, 272, 115, (Color){0,0,0,185});
    DrawText("LEGENDA (koordinat Kartesian):", SCREEN_W-273, 66, 12, WHITE);
    for (int i = 0; i < 4; i++) {
        int ly = 84 + i*22;
        DrawText(ellipses[i].lbl, SCREEN_W-270, ly-6, 11, ellipses[i].c);
    }

    DrawCoordInfo("Bresenham Ellipse");
    DrawText("[ESC/BACKSPACE] Kembali  |  Klik BACK", 12, SCREEN_H-22, 13, DARKGRAY);
}
