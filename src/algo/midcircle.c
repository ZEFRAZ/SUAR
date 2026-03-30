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
 * @brief Algoritma Midpoint Ellipse (Integer-Only).
 * 
 * Menggambar kelengkungan elips murni menggunakan angka bulat (integer) sesuai metode dosen.
 * Terbagi menjadi Region 1 (saat slope < 1) dan Region 2 (slope >= 1).
 * Mengalikan rumus asli dengan 4 untuk menghilangkan pecahan desimal.
 * 
 * @param center_x, center_y Titik pusat elips.
 * @param rx Jari-jari mendatar.
 * @param ry Jari-jari tegak.
 * @param color Skema warna pixel di layar.
 */
void MidpointEllipse(int center_x, int center_y, int rx, int ry, Color color) {
    if (rx == 0 || ry == 0) return;

    long long rx2 = (long long)rx * rx;
    long long ry2 = (long long)ry * ry;

    int x = 0;
    int y = ry;

    // Region 1: |slope| < 1, iterasi berdasarkan sumbu X
    // Dikalikan 4 untuk menghindari pecahan 0.25 (p1 = 4*ry2 - 4*rx2*ry + rx2)
    long long p1 = 4 * ry2 - 4 * rx2 * ry + rx2;

    while (ry2 * x < rx2 * y) {
        DrawPixel(center_x + x, center_y + y, color);
        DrawPixel(center_x - x, center_y + y, color);
        DrawPixel(center_x + x, center_y - y, color);
        DrawPixel(center_x - x, center_y - y, color);

        x++;
        if (p1 < 0) {
            p1 += 4 * ry2 * (2 * x + 3);
        } else {
            y--;
            p1 += 4 * ry2 * (2 * x + 3) - 4 * rx2 * (2 * y + 2);
        }
    }

    // Region 2: |slope| >= 1, iterasi berdasarkan sumbu Y
    // Dikalikan 4 untuk menghindari pecahan 0.5 di formula asli
    long long p2 = ry2 * (2 * x + 1) * (2 * x + 1) + 4 * rx2 * (y - 1) * (y - 1) - 4 * rx2 * ry2;

    while (y >= 0) {
        DrawPixel(center_x + x, center_y + y, color);
        DrawPixel(center_x - x, center_y + y, color);
        DrawPixel(center_x + x, center_y - y, color);
        DrawPixel(center_x - x, center_y - y, color);

        y--;
        if (p2 > 0) {
            p2 += 4 * rx2 * (-2 * y + 3);
        } else {
            x++;
            p2 += 4 * ry2 * (2 * x + 2) + 4 * rx2 * (-2 * y + 3);
        }
    }
}