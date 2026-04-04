#ifndef MIDCIRCLE_H
#define MIDCIRCLE_H

#include "raylib.h"

/**
 * @brief Menggambar tepi lingkaran menggunakan Algoritma Midpoint Circle.
 *
 * Algoritma ini memanfaatkan simetri 8 arah lingkaran sehingga hanya perlu
 * menghitung seperdelapan bagian, lalu mencerminkannya ke 7 posisi lainnya.
 * Semua perhitungan menggunakan bilangan bulat tanpa pembagian pecahan.
 *
 * @param centerX Koordinat X pusat lingkaran.
 * @param centerY Koordinat Y pusat lingkaran.
 * @param radius  Jari-jari lingkaran dalam piksel.
 * @param color   Warna tepi lingkaran yang akan digambar.
 */
void Midcircle(int centerX, int centerY, int radius, Color color);

/**
 * @brief Menggambar elips menggunakan Algoritma Midpoint Ellipse.
 *
 * Algoritma ini membagi proses penggambaran elips menjadi dua wilayah
 * berdasarkan kemiringan kurva, lalu mengiterasi masing-masing wilayah
 * menggunakan bilangan bulat.
 *
 * @param centerX Koordinat X pusat elips.
 * @param centerY Koordinat Y pusat elips.
 * @param rx      Jari-jari horizontal (sumbu X).
 * @param ry      Jari-jari vertikal (sumbu Y).
 * @param color   Warna tepi elips yang akan digambar.
 */
void MidpointEllipse(int centerX, int centerY, int rx, int ry, Color color);

#endif