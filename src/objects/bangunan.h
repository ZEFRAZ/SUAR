#ifndef BANGUNAN_H
#define BANGUNAN_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file bangunan.h
 * @brief Deklarasi fungsi penggambaran objek bangunan: Mercusuar.
 *
 * Modul ini menyediakan fungsi untuk menggambar mercusuar secara lengkap,
 * termasuk fondasi, badan menara, balkon, atap, dan efek cahaya sorot.
 */

/**
 * @brief Menggambar bangunan Mercusuar secara utuh beserta efek lampunya.
 *
 * Objek digambar lapis demi lapis dari bawah ke atas. Saat lampu menyala
 * (variant = 0), efek sorot cahaya yang berputar akan ditampilkan.
 *
 * @param x       Koordinat X pusat bawah mercusuar di layar.
 * @param y       Koordinat Y pusat bawah mercusuar di layar.
 * @param scale   Faktor skala objek (1.0 = ukuran normal).
 * @param variant Mode lampu: 0 = lampu menyala, 1 = lampu mati.
 * @param isWire  Jika true, hanya garis tepi yang digambar (mode wireframe).
 */
void DrawMercusuar_Pola(float x, float y, float scale, int variant, bool isWire);

#endif // BANGUNAN_H