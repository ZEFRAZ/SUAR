#ifndef POLA2D_H
#define POLA2D_H

#include "raylib.h"
#include "menu.h"

/**
 * @file pola2d.h
 * @brief Deklarasi fungsi untuk layar Showcase Objek (Pola 2D).
 *
 * Layar ini menampilkan seluruh objek proyek satu per satu di tengah layar
 * agar bisa diperiksa detail bentuknya. Mendukung mode solid dan wireframe,
 * serta pemilihan algoritma garis (DDA atau Bresenham).
 */

/**
 * @brief Menjalankan logika dan penggambaran layar Pola 2D setiap frame.
 *
 * @return ScreenState Layar berikutnya (SCREEN_MENU jika tombol Kembali ditekan).
 */
ScreenState UpdateDrawPola2D(void);

/**
 * @brief Mengatur algoritma garis yang dipakai di seluruh layar Pola 2D.
 *
 * @param useBresenhamAlgo true = Bresenham, false = DDA.
 */
void SetPolaAlgorithm(bool useBresenhamAlgo);

/**
 * @brief Melepaskan aset layar Pola 2D dari memori.
 */
void UnloadPola2D(void);

#endif // POLA2D_H