/**
 * @file draw_helpers.c
 * @brief Inti arsitektur perenderan matriks transformasi grafis 2D (2D Graphic Transformation Matrix).
 * 
 * Pusat kendali dan rekayasa manipulasi vertex (Vertex Manipulation Engine) bagi ruang lingkup koordinat Raylib. 
 * Dilengkapi fungsi penjembatan algoritma interpolasi polinomial Bezier
 */

#include "draw_helpers.h"
#include "bresenham.h"
#include "dda.h"
#include "midcircle.h"
#include <math.h>

/**
 * @brief Algoritma Pewarnaan Bidang Trapesium Berbasis Pemindaian Garis (Scan-Line Trapezoid Fill).
 * 
 * Melakukan pengisian rasterisasi pada bangun trapesium horizontal dengan melakukan iterasi dari batas
 * atas (yt) ke batas bawah (yb). Menggunakan perhitungan rasio/interpolasi linear `s` untuk menemukan 
 * titik ordinat (x) di kiri dan di kanan, lalu memanggil fungsi BresenhamLine untuk mengarsir setiap barisnya secara solid.
 * 
 * @param xl_t Koordinat X ujung kiri atas.
 * @param xr_t Koordinat X ujung kanan atas.
 * @param yt   Koordinat Y batas langit-langit (atas).
 * @param xl_b Koordinat X ujung kiri bawah.
 * @param xr_b Koordinat X ujung kanan bawah.
 * @param yb   Koordinat Y batas lantai (bawah).
 * @param c    Warna arsiran solid Trapesium.
 */
              int xl_b, int xr_b, int yb, Color c) {
    if (yt > yb) {
        int t;
        t=yt;yt=yb;yb=t; t=xl_t;xl_t=xl_b;xl_b=t; t=xr_t;xr_t=xr_b;xr_b=t;
    }
    int h = yb - yt; if (h == 0) h = 1;
    for (int y = yt; y <= yb; y++) {
        float s = (float)(y - yt) / h;
        int xl = xl_t + (int)(s * (xl_b - xl_t));
        int xr = xr_t + (int)(s * (xr_b - xr_t));
        if (xl > xr) { int t = xl; xl = xr; xr = t; }
        BresenhamLine(xl, y, xr, y, c);
    }
}

/**
 * @brief Algoritma Pewarnaan Persegi (Solid Rectangle Fill).
 * 
 * Mengeksekusi penarikan garis horizontal (sepanjang `w`) berulang kali kebawah sebanyak `h` baris
 * menggunakan algoritma pewarnaan stabil DDALine.
 * 
 * @param x Posisi pojok kiri atas (X).
 * @param y Posisi pojok kiri atas (Y).
 * @param w Lebar (Width) dari persegi.
 * @param h Tinggi (Height) vertikal ke bawah dari persegi.
 * @param c Warna arsiran bidang persegi.
 */
void FillRect(int x, int y, int w, int h, Color c) {
    for (int row = y; row <= y + h; row++)
        DDALine(x, row, x + w, row, c);
}

/**
 * @brief Rasterisasi Segitiga Solid Menggunakan Pembagian Trapesium (Standard Triangle Rasterization).
 * 
 * Membongkar bentukan Segitiga 3 Titik menjadi kombinasi dua bilah trapesium datar (Flat-Top & Flat-Bottom).
 * Melakukan pengurutan koordinat vertikal (Y-Sort) dari puncak ke dasar, mengkalkulasi titik potong 
 * garis tengah (Cut Point), dan mendistribusikan piksel pekat melalui `FillTrap`.
 */
void FillTri(int x1,int y1,int x2,int y2,int x3,int y3, Color c) {
    int t;
    if (y1>y2){t=x1;x1=x2;x2=t; t=y1;y1=y2;y2=t;}
    if (y1>y3){t=x1;x1=x3;x3=t; t=y1;y1=y3;y3=t;}
    if (y2>y3){t=x2;x2=x3;x3=t; t=y2;y2=y3;y3=t;}
    int th = y3 - y1; if (th == 0) return;
    for (int y = y1; y <= y3; y++) {
        int second = (y >= y2) || (y2 == y1);
        int sh = second ? (y3-y2) : (y2-y1); if (sh == 0) sh = 1;
        float a = (float)(y-y1)/th;
        float b = (float)(y-(second?y2:y1))/sh;
        int xa = x1+(int)(a*(x3-x1));
        int xb = second ? x2+(int)(b*(x3-x2)) : x1+(int)(b*(x2-x1));
        if (xa>xb){t=xa;xa=xb;xb=t;}
        BresenhamLine(xa, y, xb, y, c);
    }
}

// =============================================================================
// MODUL MATEMATIKA TRANSFORMATIF DAN TRIANGULASI (TRANSFORMATIVE GEOMETRY MODULE)
// =============================================================================
static bool g_useBresenhamAlgo = false;

void SetDrawHelperAlgorithm(bool useBresenham) {
    g_useBresenhamAlgo = useBresenham;
}

void AlgoLine(int x1, int y1, int x2, int y2, Color c) {
    if (g_useBresenhamAlgo) BresenhamLine(x1, y1, x2, y2, c);
    else DDALine(x1, y1, x2, y2, c);
}

Vector2 TransformPola(float px, float py, float tx, float ty, float scale, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    float rx = (px * cosA - py * sinA) * scale;
    float ry = (px * sinA + py * cosA) * scale;
    return (Vector2){ rx + tx, ry + ty };
}

void DrawCustomPoly(Vector2* localPts, int count, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    Vector2 worldPts[50];
    for (int i = 0; i < count; i++) {
        worldPts[i] = TransformPola(localPts[i].x, localPts[i].y, tx, ty, scale, angle);
    }
    if (!isWire && count >= 3 && fill.a > 0) {
        for (int i = 1; i < count - 1; i++) {
            DrawTriangle(worldPts[0], worldPts[i], worldPts[i+1], fill);
            DrawTriangle(worldPts[0], worldPts[i+1], worldPts[i], fill); 
        }
    }
    if (outline.a > 0) {
        for (int i = 0; i < count; i++) {
            Vector2 a = worldPts[i];
            Vector2 b = worldPts[(i+1)%count];
            AlgoLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, outline);
        }
    }
}

void DrawCustomBezier(Vector2 p0, Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    Vector2 t0 = TransformPola(p0.x, p0.y, tx, ty, scale, angle);
    Vector2 t1 = TransformPola(p1.x, p1.y, tx, ty, scale, angle);
    Vector2 t2 = TransformPola(p2.x, p2.y, tx, ty, scale, angle);

    int segments = 12; 
    Vector2 prev = t0;
    
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        float invT = 1.0f - t;
        float px = invT*invT*t0.x + 2*invT*t*t1.x + t*t*t2.x;
        float py = invT*invT*t0.y + 2*invT*t*t1.y + t*t*t2.y;
        Vector2 current = (Vector2){px, py};

        if (!isWire && fill.a > 0) {
            DrawTriangle(t0, prev, current, fill);
            DrawTriangle(t0, current, prev, fill);
        }
        AlgoLine((int)prev.x, (int)prev.y, (int)current.x, (int)current.y, outline);
        prev = current;
    }
}

void DrawCustomLine(Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color col) {
    Vector2 t1 = TransformPola(p1.x, p1.y, tx, ty, scale, angle);
    Vector2 t2 = TransformPola(p2.x, p2.y, tx, ty, scale, angle);
    AlgoLine((int)t1.x, (int)t1.y, (int)t2.x, (int)t2.y, col);
}

void DrawCustomCircle(Vector2 center, float radius, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    Vector2 c = TransformPola(center.x, center.y, tx, ty, scale, angle);
    float r = radius * scale;
    float strokeR = r + (2.5f * scale); 

    if (!isWire) {
        if (outline.a > 0) DrawCircleV(c, strokeR, outline);
        if (fill.a > 0) DrawCircleV(c, r, fill);
    } else {
        if (outline.a > 0) Midcircle((int)c.x, (int)c.y, (int)strokeR, GREEN);
        if (fill.a > 0) Midcircle((int)c.x, (int)c.y, (int)r, GREEN);
    }
}

void DrawCustomRing(Vector2 center, float radius, float thickness, float tx, float ty, float scale, float angle, Color color, bool isWire) {
    Vector2 c = TransformPola(center.x, center.y, tx, ty, scale, angle);
    float r = radius * scale;
    float th = thickness * scale;
    int segments = 24;
    Vector2 prevIn = {c.x + (r - th/2.0f), c.y};
    Vector2 prevOut = {c.x + (r + th/2.0f), c.y};
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments * 2.0f * PI;
        Vector2 currIn = {c.x + cosf(t) * (r - th/2.0f), c.y + sinf(t) * (r - th/2.0f)};
        Vector2 currOut = {c.x + cosf(t) * (r + th/2.0f), c.y + sinf(t) * (r + th/2.0f)};
        if (!isWire) {
            DrawTriangle(prevOut, prevIn, currIn, color);
            DrawTriangle(prevOut, currIn, currOut, color);
        } else {
            AlgoLine((int)prevIn.x, (int)prevIn.y, (int)currIn.x, (int)currIn.y, GREEN);
            AlgoLine((int)prevOut.x, (int)prevOut.y, (int)currOut.x, (int)currOut.y, GREEN);
        }
        prevIn = currIn; prevOut = currOut;
    }
}

void DrawCustomRect(float rx, float ry, float rw, float rh, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    float cx = rx + rw / 2.0f;
    float cy = ry + rh / 2.0f;
    Vector2 center = TransformPola(cx, cy, tx, ty, scale, angle);
    float w = rw * scale;
    float h = rh * scale;
    
    if (!isWire && fill.a > 0) {
        Rectangle rec = { center.x, center.y, w, h };
        DrawRectanglePro(rec, (Vector2){ w / 2.0f, h / 2.0f }, angle * RAD2DEG, fill);
    }
    
    Color outCol = isWire ? GREEN : outline;
    if (outCol.a > 0 || isWire) {
        Vector2 p1 = TransformPola(rx, ry, tx, ty, scale, angle);
        Vector2 p2 = TransformPola(rx + rw, ry, tx, ty, scale, angle);
        Vector2 p3 = TransformPola(rx + rw, ry + rh, tx, ty, scale, angle);
        Vector2 p4 = TransformPola(rx, ry + rh, tx, ty, scale, angle);
        AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, outCol);
        AlgoLine((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, outCol);
        AlgoLine((int)p3.x, (int)p3.y, (int)p4.x, (int)p4.y, outCol);
        AlgoLine((int)p4.x, (int)p4.y, (int)p1.x, (int)p1.y, outCol);
    }
}

void DrawClippedBeamTriangle(Vector2 p0, Vector2 p1, Vector2 p2, float minX, float minY, float maxX, float maxY, Color col) {
    Vector2 poly[10]; int polyCount = 3;
    poly[0] = p0; poly[1] = p1; poly[2] = p2;
    Vector2 temp[10]; int tempCount;
    
    // Kliping Sutherland-Hodgman
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { Vector2 curr = poly[i]; Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; bool currIn = curr.x >= minX; bool prevIn = prev.x >= minX; if (currIn != prevIn) temp[tempCount++] = (Vector2){ minX, prev.y + (minX - prev.x) / (curr.x - prev.x) * (curr.y - prev.y) }; if (currIn) temp[tempCount++] = curr; }
    polyCount = tempCount; for(int i=0;i<polyCount;i++) poly[i]=temp[i];
    
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { Vector2 curr = poly[i]; Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; bool currIn = curr.x <= maxX; bool prevIn = prev.x <= maxX; if (currIn != prevIn) temp[tempCount++] = (Vector2){ maxX, prev.y + (maxX - prev.x) / (curr.x - prev.x) * (curr.y - prev.y) }; if (currIn) temp[tempCount++] = curr; }
    polyCount = tempCount; for(int i=0;i<polyCount;i++) poly[i]=temp[i];
    
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { Vector2 curr = poly[i]; Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; bool currIn = curr.y >= minY; bool prevIn = prev.y >= minY; if (currIn != prevIn) temp[tempCount++] = (Vector2){ prev.x + (minY - prev.y) / (curr.y - prev.y) * (curr.x - prev.x), minY }; if (currIn) temp[tempCount++] = curr; }
    polyCount = tempCount; for(int i=0;i<polyCount;i++) poly[i]=temp[i];
    
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { Vector2 curr = poly[i]; Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; bool currIn = curr.y <= maxY; bool prevIn = prev.y <= maxY; if (currIn != prevIn) temp[tempCount++] = (Vector2){ prev.x + (maxY - prev.y) / (curr.y - prev.y) * (curr.x - prev.x), maxY }; if (currIn) temp[tempCount++] = curr; }
    polyCount = tempCount; for(int i=0;i<polyCount;i++) poly[i]=temp[i];
    
    if (polyCount >= 3) {
        for (int i=1; i<polyCount-1; i++) {
            DrawTriangle(poly[0], poly[i], poly[i+1], col);
            DrawTriangle(poly[0], poly[i+1], poly[i], col); 
        }
    }
}