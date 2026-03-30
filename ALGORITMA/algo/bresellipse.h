#ifndef BRESELLIPSE_H
#define BRESELLIPSE_H

#include "raylib.h"

/*
 * Bresenham Ellipse Algorithm
 *
 * Variasi algoritma Bresenham untuk menggambar elips.
 * Mirip dengan Midpoint Ellipse tetapi dengan formulasi decision
 * parameter yang sedikit berbeda.
 *
 * Algoritma ini menggunakan pendekatan integer dan membagi
 * elips menjadi 2 region berdasarkan gradien kurva.
 *
 * Rumus elips: (x/a)² + (y/b)² = 1
 *   a = radius horizontal (rx)
 *   b = radius vertikal (ry)
 */

void BresenhamEllipse(int cx, int cy, int rx, int ry, Color color);

#endif
