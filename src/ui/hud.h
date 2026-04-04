#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include "../screens/animasi2d.h" // Butuh struktur data Animasi2DScreen

/**
 * @file hud.h
 * @brief Deklarasi fungsi panel kontrol (HUD) untuk layar Animasi 2D.
 *
 * Panel ini ditampilkan di atas layar animasi dan berisi tab untuk mengatur
 * cuaca, waktu, ombak, angin, serta menambahkan kapal ke dalam simulasi.
 */

/**
 * @brief Menggambar panel kontrol HUD di atas layar animasi.
 *
 * @param s Pointer ke data status layar Animasi2D yang akan dikendalikan.
 */
void DrawCustomHUD(Animasi2DScreen *s);

#endif // HUD_H
