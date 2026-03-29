#ifndef DDA_H
#define DDA_H

#include "raylib.h"

/**
 * @brief Menggambar garis lurus menggunakan algoritma Digital Differential Analyzer (DDA).
 * 
 * @param x1 Koordinat X dari titik awal.
 * @param y1 Koordinat Y dari titik awal.
 * @param x2 Koordinat X dari titik tujuan.
 * @param y2 Koordinat Y dari titik tujuan.
 * @param color Warna garis yang akan digambar.
 */
void DDALine(int x1, int y1, int x2, int y2, Color color);

/**
 * @brief Menggambar garis putus-putus menggunakan algoritma DDA.
 * 
 * @param x1 Koordinat X dari titik awal.
 * @param y1 Koordinat Y dari titik awal.
 * @param x2 Koordinat X dari titik tujuan.
 * @param y2 Koordinat Y dari titik tujuan.
 * @param dashLen Panjang piksel segmen garis yang digambar.
 * @param gapLen Panjang piksel segmen celah (ruang kosong) antar garis.
 * @param color Warna garis putus-putus yang akan digambar.
 */
void DDA_DashedLine(int x1, int y1, int x2, int y2, int dashLen, int gapLen, Color color);

#endif