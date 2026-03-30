#ifndef MIDELLIPSE_H
#define MIDELLIPSE_H

#include "raylib.h"

/*
 * Midpoint Ellipse Algorithm
 *
 * Algoritma untuk menggambar elips menggunakan decision parameter.
 * Elips digambar dalam 2 region:
 *   Region 1: |slope| < 1 (gradien landai) - iterasi berdasarkan x
 *   Region 2: |slope| >= 1 (gradien curam) - iterasi berdasarkan y
 *
 * Decision parameter menentukan piksel mana yang dipilih untuk
 * mendekati kurva elips ideal.
 *
 * Rumus elips: (x/a)² + (y/b)² = 1
 *   a = radius horizontal (rx)
 *   b = radius vertikal (ry)
 */

void MidpointEllipse(int cx, int cy, int rx, int ry, Color color);

#endif
