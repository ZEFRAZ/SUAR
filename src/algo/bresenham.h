#ifndef BRESENHAM_H
#define BRESENHAM_H

#include "raylib.h"

/**
 * @brief Menggambar garis lurus menggunakan algoritma Bresenham.
 * 
 * Algoritma ini berjalan murni dengan aritmatika bilangan bulat (integer),
 * menjadikannya sangat efisien dan cepat tanpa operasi pembulatan *floating-point*.
 * 
 * @param x1 Koordinat X dari titik awal.
 * @param y1 Koordinat Y dari titik awal.
 * @param x2 Koordinat X dari titik tujuan.
 * @param y2 Koordinat Y dari titik tujuan.
 * @param color Warna piksel yang akan dirender.
 */
void BresenhamLine(int x1, int y1, int x2, int y2, Color color);

#endif