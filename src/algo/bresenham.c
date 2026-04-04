#include "bresenham.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief Menggambar garis lurus menggunakan Algoritma Bresenham.
 *
 * Cara kerja: algoritma ini hanya menggunakan penjumlahan dan pengurangan
 * bilangan bulat untuk menentukan piksel mana yang akan diwarnai di setiap langkah.
 * Tidak ada perhitungan pecahan (float) sama sekali, sehingga sangat efisien.
 *
 * @param x1 Koordinat X titik awal.
 * @param y1 Koordinat Y titik awal.
 * @param x2 Koordinat X titik akhir.
 * @param y2 Koordinat Y titik akhir.
 * @param color Warna garis yang akan digambar.
 */
void BresenhamLine(int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    // Menentukan arah gerakan: ke kanan (+1) atau ke kiri (-1)
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;

    // Parameter galat awal untuk menentukan pemilihan piksel berikutnya
    int err = dx - dy;

    while (1) {
        DrawPixel(x1, y1, color);

        // Berhenti jika sudah mencapai titik akhir
        if (x1 == x2 && y1 == y2) break;

        int e2 = 2 * err;

        // Geser sumbu X jika galat cukup besar
        if (e2 > -dy) {
            err -= dy;
            x1  += sx;
        }

        // Geser sumbu Y jika galat cukup kecil
        if (e2 < dx) {
            err += dx;
            y1  += sy;
        }
    }
}
