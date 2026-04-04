#ifndef DDA_H
#define DDA_H

#include "raylib.h"

/**
 * @brief Menggambar garis lurus menggunakan Algoritma DDA (Digital Differential Analyzer).
 *
 * Algoritma ini menghitung penambahan posisi secara bertahap menggunakan nilai
 * pecahan, lalu membulatkannya ke piksel terdekat di setiap langkah.
 *
 * @param x1    Koordinat X titik awal.
 * @param y1    Koordinat Y titik awal.
 * @param x2    Koordinat X titik akhir.
 * @param y2    Koordinat Y titik akhir.
 * @param color Warna garis yang akan digambar.
 */
void DDALine(int x1, int y1, int x2, int y2, Color color);

/**
 * @brief Menggambar garis putus-putus menggunakan Algoritma DDA.
 *
 * Sama dengan DDALine, namun setiap beberapa langkah proses menggambar
 * dihentikan sementara sehingga menghasilkan pola garis dan celah bergantian.
 *
 * @param x1      Koordinat X titik awal.
 * @param y1      Koordinat Y titik awal.
 * @param x2      Koordinat X titik akhir.
 * @param y2      Koordinat Y titik akhir.
 * @param dashLen Panjang segmen garis yang diwarnai.
 * @param gapLen  Panjang celah kosong di antara segmen.
 * @param color   Warna garis yang akan digambar.
 */
void DDA_DashedLine(int x1, int y1, int x2, int y2, int dashLen, int gapLen, Color color);

#endif