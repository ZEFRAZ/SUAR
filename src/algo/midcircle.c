#include "midcircle.h"

/**
 * @brief Fungsi utusan (helper unit) untuk mereplikasi satu titik koordinat ke dalam
 *        delapan posisi simetris secara simultan.
 * 
 * Karena lingkaran memiliki tingkat simetri yang sempurna di 8 kuadran oktan (sumbu X, 
 * sumbu Y, dan sumbu diagonal), algoritma Midpoint bertugas mengkalkulasi satu oktan saja 
 * (seperdelapan putaran), lalu memproyeksikannya sekaligus.
 */
static void PlotSymmetryPoints(int cx, int cy, int x, int y, Color color) {
    DrawPixel(cx + x, cy + y, color); // Oktan 1
    DrawPixel(cx + y, cy + x, color); // Oktan 2
    DrawPixel(cx + y, cy - x, color); // Oktan 3
    DrawPixel(cx + x, cy - y, color); // Oktan 4
    DrawPixel(cx - x, cy - y, color); // Oktan 5
    DrawPixel(cx - y, cy - x, color); // Oktan 6
    DrawPixel(cx - y, cy + x, color); // Oktan 7
    DrawPixel(cx - x, cy + y, color); // Oktan 8
}

/**
 * @brief Pemrosesan utama dari Algoritma Midpoint Lingkaran (Midpoint Circle Algorithm).
 * 
 * Beroperasi di atas parameter keputusan matematika (decision parameter 'd') 
 * untuk mengevaluasi posisi piksel grafis diskrit terdekat terhadap kurva lingkaran faktual
 * dari persamaan kontinu (x^2 + y^2 = r^2). Semua komputasi murni menggunakan basis integer.
 */
void Midcircle(int centerX, int centerY, int radius, Color color) {
    if (radius <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }
    
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;  // Decision parameter awal
    
    while (y >= x) {
        PlotSymmetryPoints(centerX, centerY, x, y, color);
        
        if (d < 0) {
            // Evaluasi jalur Timur (E): Tidak ada perpindahan tingkat Y
            d = d + 4 * x + 6;
        } else {
            // Evaluasi jalur Tenggara (SE): Penurunan tingkat piksel Y
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/**
 * @brief Merender entitas lingkaran tersolidasi dengan efisiensi raster berbasis komposit segmen horizontal.
 * 
 * Berbeda dengan pengisian berbasis piksel tunggal yang menghabiskan instruksi,
 * fungsi ini menembakkan intruksi pembuatan bidang segi empat berskala mikro setebal 1 piksel 
 * (`DrawRectangle`), yang secara esensi mengemulasi algoritma pengisian Boundary Fill Scanline.
 */
void MidcircleFilled(int centerX, int centerY, int radius, Color color) {
    if (radius <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }
    
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius;
    
    while (y >= x) {
        // Penarikan segmen pita horizontal dari tepi ke tepi guna menutup keseluruhan rongga bangun matematis
        DrawRectangle(centerX - x, centerY + y, (x * 2) + 1, 1, color);
        DrawRectangle(centerX - x, centerY - y, (x * 2) + 1, 1, color);
        
        DrawRectangle(centerX - y, centerY + x, (y * 2) + 1, 1, color);
        DrawRectangle(centerX - y, centerY - x, (y * 2) + 1, 1, color);
        
        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

/**
 * @brief Pengimplementasian formal persamaan elips matematis dalam ruang metrik piksel
 *        melalui instrumen ganda Parameter Keputusan (Dual Decision Parameters).
 */
void MidpointEllipse(int center_x, int center_y, int rx, int ry, Color color) {
    long long rxSq = (long long)rx * rx;
    long long rySq = (long long)ry * ry;
    long long x = 0, y = ry, px = 0, py = 2 * rxSq * y;

    // Wilayah 1 (Region 1): Kelengkungan landai, prioritas peningkatan pada absis X
    long long p1 = rySq - (rxSq * ry) + (0.25f * rxSq);
    while (px < py) {
        DrawPixel(center_x + x, center_y + y, color);
        DrawPixel(center_x - x, center_y + y, color);
        DrawPixel(center_x + x, center_y - y, color);
        DrawPixel(center_x - x, center_y - y, color);
        x++;
        px = px + 2 * rySq;
        if (p1 < 0) {
            p1 = p1 + rySq + px;
        } else {
            y--;
            py = py - 2 * rxSq;
            p1 = p1 + rySq + px - py;
        }
    }

    // Wilayah 2 (Region 2): Kelengkungan curam, perpindahan absis lebih cepat dari absis X hingga menyentuh titik singgung bawah
    long long p2 = (rySq) * ((x + 0.5f) * (x + 0.5f)) +
                   (rxSq) * ((y - 1) * (y - 1)) - (rxSq * rySq);
    while (y >= 0) {
        DrawPixel(center_x + x, center_y + y, color);
        DrawPixel(center_x - x, center_y + y, color);
        DrawPixel(center_x + x, center_y - y, color);
        DrawPixel(center_x - x, center_y - y, color);
        y--;
        py = py - 2 * rxSq;
        if (p2 > 0) {
            p2 = p2 + rxSq - py;
        } else {
            x++;
            px = px + 2 * rySq;
            p2 = p2 + rxSq - py + px;
        }
    }
}