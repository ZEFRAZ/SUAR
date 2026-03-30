#include "src/algo/bresellipse.h"

/*
 * Bresenham Ellipse Algorithm
 *
 * Variasi Bresenham untuk elips menggunakan decision parameter integer.
 *
 * Region 1 (gradien landai):
 *   - Decision parameter: d1 = ry² - rx²*ry + rx²/4
 *   - Iterasi x++ sampai ry²*x >= rx²*y
 *
 * Region 2 (gradien curam):
 *   - Decision parameter: d2 = ry²*(x+0.5)² + rx²*(y-1)² - rx²*ry²
 *   - Iterasi y-- sampai y < 0
 *
 * Keunggulan: Semua perhitungan menggunakan operasi integer,
 * cocok untuk sistem tanpa FPU.
 */

static void DrawEllipsePoints(int cx, int cy, int x, int y, Color color) {
    DrawPixel(cx + x, cy + y, color);  // Kuadran I
    DrawPixel(cx - x, cy + y, color);  // Kuadran II
    DrawPixel(cx + x, cy - y, color);  // Kuadran IV
    DrawPixel(cx - x, cy - y, color);  // Kuadran III
}

void BresenhamEllipse(int cx, int cy, int rx, int ry, Color color) {
    if (rx == 0 || ry == 0) return;

    long rx2 = (long)rx * rx;
    long ry2 = (long)ry * ry;
    long two_rx2 = 2 * rx2;
    long two_ry2 = 2 * ry2;

    int x = 0;
    int y = ry;

    // Region 1: iterasi berdasarkan x
    long d1 = ry2 - rx2 * ry + rx2 / 4;

    while (ry2 * x < rx2 * y) {
        DrawEllipsePoints(cx, cy, x, y, color);

        x++;
        if (d1 < 0) {
            d1 += ry2 * (2 * x + 3);
        } else {
            y--;
            d1 += ry2 * (2 * x + 3) - rx2 * (2 * y + 2);
        }
    }

    // Region 2: iterasi berdasarkan y
    long d2 = ry2 * (x * x + x) + rx2 * (y * y - 2 * y + 1) - rx2 * ry2;

    while (y >= 0) {
        DrawEllipsePoints(cx, cy, x, y, color);

        y--;
        if (d2 > 0) {
            d2 += rx2 * (3 - 2 * y);
        } else {
            x++;
            d2 += ry2 * (2 * x + 2) + rx2 * (3 - 2 * y);
        }
    }
}
