#include "src/screens/program30.h"
#include "src/algo/midellipse.h"
#include "src/algo/dda.h"
#include "src/ui/back_button.h"
#include "src/ui/cartesian.h"
#include "src/ui/primitives.h"
#include "coords.h"
#include "screen_type.h"
#include "raylib.h"

void DrawProgram30(void) {
    G_OriginX  = SCREEN_W / 2;
    G_OriginY  = SCREEN_H / 2 + 30;
    G_TickStep = 38;

    ClearBackground((Color){12,12,28,255});
    DrawCartesianGrid(DDALine);

    // Definisi elips: cx, cy (Kartesian), rx, ry (dalam unit Kartesian)
    struct { int cx, cy, rx, ry; Color c; const char *lbl; } ellipses[] = {
        { 0,  0, 6, 4, RED,     "Elips 1: rx=6, ry=4 (pusat)" },
        {-5,  3, 3, 2, SKYBLUE, "Elips 2: rx=3, ry=2 (-5,3)" },
        { 4, -2, 2, 3, YELLOW,  "Elips 3: rx=2, ry=3 (4,-2)" },
        { 0, -4, 4, 2, MAGENTA, "Elips 4: rx=4, ry=2 (0,-4)" },
    };

    for (int i = 0; i < 4; i++) {
        // Konversi pusat Kartesian ke layar
        int scx = CS_X(ellipses[i].cx);
        int scy = CS_Y(ellipses[i].cy);
        // Konversi radius ke piksel
        int srx = ellipses[i].rx * G_TickStep;
        int sry = ellipses[i].ry * G_TickStep;

        MidpointEllipse(scx, scy, srx, sry, ellipses[i].c);
        // Gambar titik pusat
        DrawDot(scx, scy, 3, ellipses[i].c);
    }

    DrawRectangle(0, 0, SCREEN_W, 56, (Color){0,0,0,210});
    DrawBackButton();
    DrawText("PROGRAM 30 - Midpoint Ellipse Algorithm", 135, 10, 18, WHITE);
    DrawText("Region1: p1=ry²x²+rx²(y+0.5)²-rx²ry²  |  Region2: p2=ry²(x+0.5)²+rx²y²-rx²ry²", 135, 33, 12, LIGHTGRAY);

    DrawRectangle(SCREEN_W-280, 60, 272, 115, (Color){0,0,0,185});
    DrawText("LEGENDA (koordinat Kartesian):", SCREEN_W-273, 66, 12, WHITE);
    for (int i = 0; i < 4; i++) {
        int ly = 84 + i*22;
        DrawText(ellipses[i].lbl, SCREEN_W-270, ly-6, 11, ellipses[i].c);
    }

    DrawCoordInfo("Midpoint Ellipse");
    DrawText("[ESC/BACKSPACE] Kembali  |  Klik BACK", 12, SCREEN_H-22, 13, DARKGRAY);
}
