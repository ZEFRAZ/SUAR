#ifndef BACK_BUTTON_H
#define BACK_BUTTON_H

/**
 * @file back_button.h
 * @brief Deklarasi antarmuka komponen navigasi layar tunggal.
 * 
 * Melingkupi pengawasan *input handler* kembali ke layar beranda (Return to Menu)
 * dan proyeksi visual *button* interaktif dengan animasi suspensi dinamika fluida (Hover Effect).
 */

/** @brief Memindai pemicu interaksi pointer (*Click*) maupun papan tik (ESC/Backspace). */
int  BackButtonPressed(void);

/** @brief Menghasilkan rendering tumpukan poligon matriks navigasi tombol "Kembali". */
void DrawBackButton(void);

#endif