#ifndef MENU_H
#define MENU_H

#include "raylib.h"

/**
 * @file menu.h
 * @brief Deklarasi tipe data dan fungsi untuk layar Menu Utama.
 *
 * File ini mendefinisikan daftar layar yang bisa dibuka (ScreenState),
 * struktur tombol navigasi (CustomButton), dan fungsi-fungsi pengelola menu.
 */

// Daftar semua layar yang ada dalam aplikasi
typedef enum {
    SCREEN_MENU,       // Layar menu utama
    SCREEN_POLA_2D,    // Layar showcase objek (Pola 2D)
    SCREEN_ANIMASI_2D, // Layar simulasi animasi laut
    SCREEN_ABOUT,      // Layar informasi pembuat
    SCREEN_GUIDE,      // Layar panduan penggunaan
    SCREEN_EXIT        // Perintah keluar dari aplikasi
} ScreenState;

// Struktur data untuk setiap tombol navigasi di menu utama
typedef struct {
    Rectangle rect;         // Area posisi dan ukuran tombol di layar
    const char *text;       // Teks label yang ditampilkan di dalam tombol
    float hoverProgress;    // Nilai animasi hover (0.0 = normal, 1.0 = hover penuh)
} CustomButton;

/**
 * @brief Memuat semua aset menu (gambar latar, font) dan mengatur nilai awal tombol.
 *
 * Wajib dipanggil sekali sebelum UpdateDrawMenu() pertama kali dijalankan.
 */
void InitMenu(void);

/**
 * @brief Menjalankan logika dan penggambaran Menu Utama setiap frame.
 *
 * @return ScreenState Layar berikutnya yang akan dibuka sesuai tombol yang ditekan.
 */
ScreenState UpdateDrawMenu(void);

/**
 * @brief Melepaskan semua aset menu dari memori saat program ditutup.
 */
void UnloadMenu(void);

#endif // MENU_H