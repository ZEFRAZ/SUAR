#ifndef GUIDE_H
#define GUIDE_H

#include "raylib.h"
#include "menu.h"

/**
 * @file guide.h
 * @brief Deklarasi fungsi untuk layar Panduan (Guide).
 *
 * Layar ini menampilkan daftar tombol keyboard dan cara berinteraksi
 * dengan simulasi di layar Animasi 2D.
 */

/**
 * @brief Menjalankan logika dan penggambaran layar Panduan setiap frame.
 *
 * @return ScreenState Layar berikutnya (SCREEN_MENU jika tombol Kembali ditekan,
 *                     SCREEN_GUIDE jika belum ditekan).
 */
ScreenState UpdateDrawGuide(void);

/**
 * @brief Melepaskan aset layar Guide dari memori.
 */
void UnloadGuide(void);

#endif // GUIDE_H