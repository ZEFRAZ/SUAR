#ifndef ABOUT_H
#define ABOUT_H

#include "raylib.h"
#include "menu.h"

/**
 * @file about.h
 * @brief Deklarasi fungsi untuk layar About (Tentang).
 *
 * Layar ini menampilkan informasi pembuat proyek seperti nama, NIM, dan mata kuliah.
 */

/**
 * @brief Menjalankan logika dan penggambaran layar About setiap frame.
 *
 * @return ScreenState Layar berikutnya (SCREEN_MENU jika tombol Kembali ditekan,
 *                     SCREEN_ABOUT jika belum ditekan).
 */
ScreenState UpdateDrawAbout(void);

/**
 * @brief Melepaskan aset layar About dari memori.
 */
void UnloadAbout(void);

#endif // ABOUT_H