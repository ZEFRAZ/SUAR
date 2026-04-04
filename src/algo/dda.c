#include "dda.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief Menggambar garis lurus menggunakan Algoritma DDA (Digital Differential Analyzer).
 *
 * Cara kerja: algoritma menghitung selisih X dan Y, lalu menentukan sumbu mana yang
 * memiliki jangkauan terpanjang. Sumbu terpanjang itulah yang menjadi jumlah langkah.
 * Di setiap langkah, posisi piksel digeser secara bertahap menggunakan nilai pecahan,
 * lalu dibulatkan ke koordinat piksel terdekat.
 *
 * @param x1 Koordinat X titik awal.
 * @param y1 Koordinat Y titik awal.
 * @param x2 Koordinat X titik akhir.
 * @param y2 Koordinat Y titik akhir.
 * @param color Warna garis yang akan digambar.
 */
void DDALine(int x1, int y1, int x2, int y2, Color color) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    // Jumlah langkah diambil dari jarak terpanjang antar sumbu
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    // Jika titik awal dan akhir sama, cukup gambar satu piksel
    if (steps == 0) {
        DrawPixel(x1, y1, color);
        return;
    }

    // Hitung penambahan posisi di setiap langkah
    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    float x    = (float)x1;
    float y    = (float)y1;

    // Gambar piksel di setiap langkah sambil menggeser posisi secara bertahap
    for (int i = 0; i <= steps; i++) {
        DrawPixel((int)roundf(x), (int)roundf(y), color);
        x += xInc;
        y += yInc;
    }
}

/**
 * @brief Menggambar garis putus-putus menggunakan Algoritma DDA.
 *
 * Cara kerja sama dengan DDA biasa, namun setiap beberapa langkah, proses
 * menggambar dihentikan sementara (membuat celah). Hasilnya adalah pola
 * garis-celah-garis-celah yang bisa diatur panjangnya.
 * Digunakan untuk menggambar garis rasi bintang di malam hari.
 *
 * @param x1      Koordinat X titik awal.
 * @param y1      Koordinat Y titik awal.
 * @param x2      Koordinat X titik akhir.
 * @param y2      Koordinat Y titik akhir.
 * @param dashLen Panjang segmen garis yang diwarnai.
 * @param gapLen  Panjang celah kosong di antara segmen.
 * @param color   Warna garis yang akan digambar.
 */
void DDA_DashedLine(int x1, int y1, int x2, int y2, int dashLen, int gapLen, Color color) {
    int dx    = x2 - x1;
    int dy    = y2 - y1;
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);

    if (steps == 0) {
        DrawPixel(x1, y1, color);
        return;
    }

    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    float x    = (float)x1;
    float y    = (float)y1;

    int counter       = 0;
    int sedangGambar  = 1; // 1 = sedang menggambar, 0 = sedang melewati celah
    int batas_sekarang = dashLen;

    for (int i = 0; i <= steps; i++) {
        if (sedangGambar) {
            DrawPixel((int)roundf(x), (int)roundf(y), color);
        }
        x += xInc;
        y += yInc;
        counter++;

        // Ganti status (gambar/celah) jika sudah mencapai batas segmen saat ini
        if (counter >= batas_sekarang) {
            counter        = 0;
            sedangGambar   = !sedangGambar;
            batas_sekarang = sedangGambar ? dashLen : gapLen;
        }
    }
}