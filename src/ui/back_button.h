#ifndef BACK_BUTTON_H
#define BACK_BUTTON_H

/**
 * @file back_button.h
 * @brief Deklarasi fungsi tombol navigasi "Kembali".
 *
 * Modul ini menyediakan tombol Kembali yang muncul di pojok kiri atas layar
 * pada setiap layar selain menu utama. Mendukung klik mouse dan tombol ESC/Backspace.
 */

/**
 * @brief Mengecek apakah tombol Kembali ditekan (klik mouse atau ESC/Backspace).
 *
 * @return 1 jika tombol ditekan, 0 jika tidak.
 */
int BackButtonPressed(void);

/**
 * @brief Menggambar tombol Kembali di pojok kiri atas layar dengan efek hover.
 */
void DrawBackButton(void);

#endif
