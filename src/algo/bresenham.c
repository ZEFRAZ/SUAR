#include "bresenham.h"
#include <math.h>
#include <stdlib.h>

/**
 * Implementasi fungsi Bresenham untuk merender garis antar titik.
 * Pendekatan ini menggunakan variabel error (err) untuk mendeduksi
 * waktu yang tepat dalam menaikkan koordinat piksel secara bertahap
 * baik di sumbu X maupun Y.
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
