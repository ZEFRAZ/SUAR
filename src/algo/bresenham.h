#ifndef BRESENHAM_H
#define BRESENHAM_H

#include "raylib.h"

/**
 * @brief Menggambar garis lurus menggunakan Algoritma Bresenham.
 *
 * Algoritma ini bekerja murni dengan bilangan bulat sehingga tidak memerlukan
 * perhitungan pecahan. Hasilnya akurat dan sangat cepat diproses.
 *
 * @param x1 Koordinat X titik awal.
 * @param y1 Koordinat Y titik awal.
 * @param x2 Koordinat X titik akhir.
 * @param y2 Koordinat Y titik akhir.
 * @param color Warna garis yang akan digambar.
 */
void BresenhamLine(int x1, int y1, int x2, int y2, Color color);

#endif