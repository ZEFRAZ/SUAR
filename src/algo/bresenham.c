#include "bresenham.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief Algoritma Pembentuk Garis Bresenham.
 * 
 * Menggambar sebuah garis dari titik (x1, y1) ke (x2, y2) tanpa menggunakan pembagian pecahan (float/double).
 * Hanya menggunakan penjumlahan dan pengurangan angka bulat (integer) sehingga sangat cepat diproses komputer.
 * 
 * @param x1, y1 Koordinat awal X dan Y.
 * @param x2, y2 Koordinat tujuan X dan Y.
 * @param color Warna cetak titik di layar.
 */
void BresenhamLine(int x1, int y1, int x2, int y2, Color color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    
    // Menentukan arah langkahan penggambaran piksel (-1 atau 1)
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    
    // Menghitung inisialisasi parameter galat (error)
    int err = dx - dy;
    
    while (1) {
        DrawPixel(x1, y1, color);
        
        // Memeriksa jika piksel saat ini telah mencapai tujuan akhir
        if (x1 == x2 && y1 == y2) {
            break;
        }
        
        // Memprediksi galat pada iterasi berikutnya untuk iterasi sub-sumbu
        int e2 = 2 * err;
        if (e2 > -dy) { 
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) { 
            err += dx;
            y1 += sy;
        }
    }
}
