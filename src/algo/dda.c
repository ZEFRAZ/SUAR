#include "dda.h"
#include <math.h>
#include <stdlib.h>

/**
 * @brief Algoritma Pembentuk Garis DDA (Digital Differential Analyzer).
 * 
 * Menggambar garis lurus di layar dari titik (x1, y1) ke (x2, y2).
 * Cara kerjanya: mencari sumbu pencapaian terjauh (X atau Y) dan menjadikannya patokan jumlah langkah.
 * Pada tiap langkah, titik koordinatnya akan ditambah perlahan (pakai pecahan float) lalu 
 * dibulatkan (round) ke koordinat piksel terdekat di layar.
 * 
 * @param x1, y1 Koordinat awal.
 * @param x2, y2 Koordinat akhir sasaran.
 * @param color Skema warna pixel di layar.
 */
void DDALine(int x1, int y1, int x2, int y2, Color color) {
    int dx = x2 - x1, dy = y2 - y1;
    
    // Menghitung langkah maksimum yang diperlukan
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
    
    // Penanganan kasus di mana titik awal dan akhir berada pada koordinat yang sama
    if (steps == 0) { 
        DrawPixel(x1, y1, color); 
        return; 
    }
    
    // Menghitung rasio penambahan untuk masing-masing sumbu
    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    float x = (float)x1, y = (float)y1;
    
    // Melakukan iterasi sebanyak langkah yang telah dikalkulasi
    for (int i = 0; i <= steps; i++) {
        // Pembulatan koordinat real ke integer terdekat untuk dirender ke dalam piksel
        DrawPixel((int)roundf(x), (int)roundf(y), color);
        x += xInc; 
        y += yInc;
    }
}

/**
 * @brief Fungsi penggambar Garis Putus-Putus (Dashed Line) berbasis DDA.
 * 
 * Mirip kegunaannya dengan DDA biasa, namun ia bisa bolak-balik antara status 
 * "menggambar" (sepanjang dashLen) dan status "bolong/hilang" (sepanjang gapLen)
 * sehingga tercetaklah garis putus-putus. Dipakai saat menggambar rasi bintang (di alam.c).
 * 
 * @param x1, y1 Titik permulaan.
 * @param x2, y2 Titik akhir target.
 * @param dashLen Jatah panjang yang akan diwarnai penuh.
 * @param gapLen Jatah panjang lompatan di mana garis tak digambar.
 * @param color Warna cetak piksel.
 */
void DDA_DashedLine(int x1, int y1, int x2, int y2, int dashLen, int gapLen, Color color) {
    int dx = x2 - x1, dy = y2 - y1;
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
    
    if (steps == 0) { 
        DrawPixel(x1, y1, color); 
        return; 
    }
    
    float xInc = (float)dx / steps;
    float yInc = (float)dy / steps;
    float x = (float)x1, y = (float)y1;
    
    int counter = 0;
    int drawing = 1; // Flag penanda state saat ini: 1 untuk menggambar, 0 untuk melompati
    int current_limit = dashLen;

    // Iterasi sepanjang garis trajektori DDA
    for (int i = 0; i <= steps; i++) {
        if (drawing) {
            DrawPixel((int)roundf(x), (int)roundf(y), color);
        }
        x += xInc; 
        y += yInc;
        
        counter++;
        
        // Memeriksa pencapaian batas panjang segmen saat ini
        if (counter >= current_limit) {
            counter = 0;
            drawing = !drawing; // Pembalikan logika state
            current_limit = drawing ? dashLen : gapLen;
        }
    }
}