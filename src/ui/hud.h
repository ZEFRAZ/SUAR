#ifndef HUD_H
#define HUD_H

#include "raylib.h"
#include "../screens/animasi2d.h" // Ketergantungan struktur state Animasi2D

/**
 * @file hud.h
 * @brief Deklarasi antarmuka pengguna proyeksi depan (Heads-Up Display).
 * 
 * Mengekspos sistem overlay menu berpemisah tab untuk mengontrol 
 * properti lingkungan dinamik (cuaca, waktu) serta peluncuran entitas armada pada simulasi berjalan.
 */

/** @brief Memproyeksikan tumpukan antarmuka menu mengambang pada lapisan terdepan kanvas 2D. */
void DrawCustomHUD(Animasi2DScreen *s);

#endif // HUD_H