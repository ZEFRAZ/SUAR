#include "src/algo/midellipse.h"

/*
 * Midpoint Ellipse Algorithm
 *
 * Algoritma menggambar elips dengan membagi menjadi 2 region:
 *
 * Region 1 (|slope| < 1):
 *   - Iterasi berdasarkan x (x++)
 *   - Decision parameter: p1 = ry² - rx²*ry + rx²/4
 *   - Scale by 4: 4*p1 = 4*ry² - 4*rx²*ry + rx²
 *   - Jika p1 < 0: p1 += ry²*(2x + 3)
 *   - Jika p1 >= 0: p1 += ry²*(2x + 3) - 2*rx²*(y+1), y--
 *
 * Region 2 (|slope| >= 1):
 *   - Iterasi berdasarkan y (y--)
 *   - Decision parameter: p2 = ry²*(x+0.5)² + rx²*(y-1)² - rx²*ry²
 *   - Scale by 4: 4*p2 = ry²*(2x+1)² + 4*rx²*(y-1)² - 4*rx²*ry²
 *   - Jika p2 > 0: p2 += rx²*(-2y + 3), x tetap
 *   - Jika p2 <= 0: p2 += ry²*(2x + 2) + rx²*(-2y + 3), x++
 *
 * Simetri 4-kuadran: setiap piksel (x,y) digambar di 4 titik:
 *   (cx+x, cy+y), (cx-x, cy+y), (cx+x, cy-y), (cx-x, cy-y)
 */

static void DrawEllipsePoints(int cx, int cy, int x, int y, Color color) {
    DrawPixel(cx + x, cy + y, color);  // Kuadran I
    DrawPixel(cx - x, cy + y, color);  // Kuadran II
    DrawPixel(cx + x, cy - y, color);  // Kuadran IV
    DrawPixel(cx - x, cy - y, color);  // Kuadran III
}

void MidpointEllipse(int cx, int cy, int rx, int ry, Color color) {
    if (rx == 0 || ry == 0) return;

    long rx2 = (long)rx * rx;
    long ry2 = (long)ry * ry;

    int x = 0;
    int y = ry;

    // Region 1: |slope| < 1, iterasi berdasarkan x
    // p1 = ry² - rx²*ry + rx²/4
    // Scale by 4 untuk menghindari pembagian integer: 4*p1 = 4*ry² - 4*rx²*ry + rx²
    long p1 = 4 * ry2 - 4 * rx2 * ry + rx2;

    while (ry2 * x < rx2 * y) {
        DrawEllipsePoints(cx, cy, x, y, color);

        x++;
        if (p1 < 0) {
            p1 += 4 * ry2 * (2 * x + 3);
        } else {
            y--;
            p1 += 4 * ry2 * (2 * x + 3) - 4 * rx2 * (2 * y + 2);
        }
    }

    // Region 2: |slope| >= 1, iterasi berdasarkan y
    // p2 = ry²*(x+0.5)² + rx²*(y-1)² - rx²*ry²
    // Scale by 4: 4*p2 = ry²*(2x+1)² + 4*rx²*(y-1)² - 4*rx²*ry²
    long p2 = ry2 * (2 * x + 1) * (2 * x + 1) + 4 * rx2 * (y - 1) * (y - 1) - 4 * rx2 * ry2;

    while (y >= 0) {
        DrawEllipsePoints(cx, cy, x, y, color);

        y--;
        if (p2 > 0) {
            p2 += 4 * rx2 * (-2 * y + 3);
        } else {
            x++;
            p2 += 4 * ry2 * (2 * x + 2) + 4 * rx2 * (-2 * y + 3);
        }
    }
}
