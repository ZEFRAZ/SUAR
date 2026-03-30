#include "src/screens/menu.h"
#include "src/algo/dda.h"
#include "src/algo/bresenham.h"
#include "src/algo/midellipse.h"
#include "src/algo/bresellipse.h"
#include "coords.h"
#include "screen_type.h"
#include "raylib.h"

void DrawMenu(void) {
    G_OriginX = 0; G_OriginY = 0; G_TickStep = 1;

    ClearBackground((Color){8,8,20,255});
    DrawRectangle(0, 0, SCREEN_W, 80, (Color){20,20,50,255});
    DrawText("Aplikasi Grafika Komputer", SCREEN_W/2-210, 12, 30, WHITE);
    DrawText("Algoritma DDA, Bresenham & Ellipse  |  Raylib + C", SCREEN_W/2-230, 48, 18, LIGHTGRAY);
    DDALine(0, 80, SCREEN_W, 80, (Color){60,80,160,255});

    int cW = 300, cH = 130;
    int gap = 20;
    int startX = (SCREEN_W - (3 * cW + 2 * gap)) / 2;
    int r1 = 95, r2 = 240, r3 = 385;

    // Row 1: Program 1, 2, 3
    DrawRectangleRounded((Rectangle){startX,r1,cW,cH}, 0.1f, 8, (Color){15,40,70,255});
    DrawRectangleRoundedLines((Rectangle){startX,r1,cW,cH}, 0.1f, 8, SKYBLUE);
    DrawText("PROGRAM 1", startX+18, r1+12, 20, SKYBLUE);
    DrawText("Diagram Kartesian - DDA", startX+18, r1+38, 14, WHITE);
    DDALine(startX+18, r1+60, startX+150, r1+60, RED);
    DDALine(startX+18, r1+74, startX+150, r1+74, YELLOW);
    DDALine(startX+18, r1+88, startX+130, r1+68, SKYBLUE);
    DrawText("Garis DDA pada bidang kartesian", startX+18, r1+100, 12, LIGHTGRAY);
    DrawText("[Tekan 1]", startX+cW-90, r1+cH-24, 14, SKYBLUE);

    int x2 = startX + cW + gap;
    DrawRectangleRounded((Rectangle){x2,r1,cW,cH}, 0.1f, 8, (Color){50,15,60,255});
    DrawRectangleRoundedLines((Rectangle){x2,r1,cW,cH}, 0.1f, 8, VIOLET);
    DrawText("PROGRAM 2", x2+18, r1+12, 20, VIOLET);
    DrawText("Style Garis - DDA", x2+18, r1+38, 14, WHITE);
    DDA_DashedLine(x2+18, r1+60, x2+150, r1+60, 12, 8, ORANGE);
    DDA_ThickLine(x2+18, r1+78, x2+150, r1+78, 4, PINK);
    DDA_DashDotLine(x2+18, r1+96, x2+150, r1+96, YELLOW);
    DrawText("Normal, dash, tebal, dash-dot", x2+18, r1+100, 12, LIGHTGRAY);
    DrawText("[Tekan 2]", x2+cW-90, r1+cH-24, 14, VIOLET);

    int x3 = x2 + cW + gap;
    DrawRectangleRounded((Rectangle){x3,r1,cW,cH}, 0.1f, 8, (Color){15,50,25,255});
    DrawRectangleRoundedLines((Rectangle){x3,r1,cW,cH}, 0.1f, 8, GREEN);
    DrawText("PROGRAM 3", x3+18, r1+12, 20, GREEN);
    DrawText("Diagram Kartesian - Bresenham", x3+18, r1+38, 14, WHITE);
    BresenhamLine(x3+18, r1+70, x3+150, r1+56, (Color){255,120,80,255});
    BresenhamLine(x3+18, r1+56, x3+150, r1+70, (Color){80,220,255,255});
    BresenhamLine(x3+18, r1+84, x3+150, r1+84, (Color){180,255,100,255});
    DrawText("Garis Bresenham pada bidang kartesian", x3+18, r1+100, 12, LIGHTGRAY);
    DrawText("[Tekan 3]", x3+cW-90, r1+cH-24, 14, GREEN);

    // Row 2: Program 4, 30, 31
    DrawRectangleRounded((Rectangle){startX,r2,cW,cH}, 0.1f, 8, (Color){50,30,10,255});
    DrawRectangleRoundedLines((Rectangle){startX,r2,cW,cH}, 0.1f, 8, ORANGE);
    DrawText("PROGRAM 4", startX+18, r2+12, 20, ORANGE);
    DrawText("Style Garis - Bresenham", startX+18, r2+38, 14, WHITE);
    Bres_DashedLine(startX+18, r2+60, startX+150, r2+60, 12, 8, ORANGE);
    Bres_ThickLine(startX+18, r2+78, startX+150, r2+78, 4, RED);
    Bres_DashDotLine(startX+18, r2+96, startX+150, r2+96, YELLOW);
    DrawText("Normal, dash, tebal, dash-dot", startX+18, r2+100, 12, LIGHTGRAY);
    DrawText("[Tekan 4]", startX+cW-90, r2+cH-24, 14, ORANGE);

    DrawRectangleRounded((Rectangle){x2,r2,cW,cH}, 0.1f, 8, (Color){40,20,60,255});
    DrawRectangleRoundedLines((Rectangle){x2,r2,cW,cH}, 0.1f, 8, (Color){180,100,220,255});
    DrawText("PROGRAM 30", x2+18, r2+12, 20, (Color){180,100,220,255});
    DrawText("Midpoint Ellipse", x2+18, r2+38, 14, WHITE);
    MidpointEllipse(x2+90, r2+75, 50, 30, (Color){180,100,220,255});
    MidpointEllipse(x2+90, r2+75, 30, 50, (Color){220,150,255,255});
    DrawText("Algoritma Midpoint untuk elips", x2+18, r2+100, 12, LIGHTGRAY);
    DrawText("[Tekan 0]", x2+cW-90, r2+cH-24, 14, (Color){180,100,220,255});

    DrawRectangleRounded((Rectangle){x3,r2,cW,cH}, 0.1f, 8, (Color){20,50,50,255});
    DrawRectangleRoundedLines((Rectangle){x3,r2,cW,cH}, 0.1f, 8, (Color){100,220,200,255});
    DrawText("PROGRAM 31", x3+18, r2+12, 20, (Color){100,220,200,255});
    DrawText("Bresenham Ellipse", x3+18, r2+38, 14, WHITE);
    BresenhamEllipse(x3+90, r2+75, 50, 30, (Color){100,220,200,255});
    BresenhamEllipse(x3+90, r2+75, 30, 50, (Color){150,255,230,255});
    DrawText("Algoritma Bresenham untuk elips", x3+18, r2+100, 12, LIGHTGRAY);
    DrawText("[Tekan 9]", x3+cW-90, r2+cH-24, 14, (Color){100,220,200,255});

    // Row 3: Program 41, About button
    DrawRectangleRounded((Rectangle){startX,r3,cW,cH}, 0.1f, 8, (Color){50,40,15,255});
    DrawRectangleRoundedLines((Rectangle){startX,r3,cW,cH}, 0.1f, 8, (Color){255,200,100,255});
    DrawText("PROGRAM 41", startX+18, r3+12, 20, (Color){255,200,100,255});
    DrawText("2D Transformations - Ellipse", startX+18, r3+38, 14, WHITE);
    BresenhamEllipse(startX+70, r3+75, 40, 25, (Color){255,100,100,255});
    BresenhamEllipse(startX+150, r3+85, 30, 20, (Color){100,255,100,255});
    BresenhamEllipse(startX+200, r3+65, 25, 35, (Color){100,100,255,255});
    DrawText("Translate, Scale, Rotate", startX+18, r3+100, 12, LIGHTGRAY);
    DrawText("[Tekan 5]", startX+cW-90, r3+cH-24, 14, (Color){255,200,100,255});

    int abBtnW = 200, abBtnH = 40;
    int abBtnX = SCREEN_W/2 - abBtnW/2 + 150, abBtnY = r3 + 45;
    Vector2 mouse = GetMousePosition();
    int abHover = CheckCollisionPointRec(mouse, (Rectangle){abBtnX, abBtnY, abBtnW, abBtnH});
    DrawRectangleRounded((Rectangle){abBtnX, abBtnY, abBtnW, abBtnH}, 0.3f, 6,
                         abHover ? (Color){60,100,180,230} : (Color){25,35,70,220});
    DrawRectangleRoundedLines((Rectangle){abBtnX, abBtnY, abBtnW, abBtnH}, 0.3f, 6,
                              abHover ? WHITE : (Color){80,110,200,255});
    DrawText("[ A ] About", abBtnX + 55, abBtnY + 11, 16, WHITE);

    DrawRectangle(0, SCREEN_H-45, SCREEN_W, 45, (Color){15,15,35,255});
    DDALine(0, SCREEN_H-45, SCREEN_W, SCREEN_H-45, (Color){60,80,160,255});
    DrawText("[1-4] Program Garis  |  [5] P41 Transform  |  [0] P30 MidEllipse  |  [9] P31 BresEllipse  |  [A] About  |  [ESC] Keluar",
             SCREEN_W/2-400, SCREEN_H-30, 14, GRAY);
}
