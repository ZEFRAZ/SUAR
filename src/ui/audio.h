#ifndef AUDIO_BGM_H
#define AUDIO_BGM_H

/**
 * @file audio.h
 * @brief Deklarasi fungsi pengelolaan musik latar dan tombol mute.
 *
 * Modul ini menyediakan fungsi untuk memuat, memutar, memperbarui,
 * dan menghentikan musik latar, serta menggambar tombol mute di layar.
 */

/**
 * @brief Membuka perangkat audio dan mulai memutar musik latar.
 *
 * Wajib dipanggil sekali saat program dimulai, sebelum game loop berjalan.
 */
void InitAudio_BGM(void);

/**
 * @brief Memperbarui buffer pemutaran musik setiap frame.
 *
 * Wajib dipanggil di setiap iterasi game loop agar musik tidak terputus.
 */
void UpdateAudio_BGM(void);

/**
 * @brief Menggambar tombol mute/unmute di pojok kanan atas layar.
 *
 * Klik tombol ini akan menyalakan atau mematikan musik latar.
 */
void DrawMuteButton(void);

/**
 * @brief Menghentikan musik dan melepaskan semua sumber daya audio dari memori.
 *
 * Wajib dipanggil sekali saat program akan ditutup.
 */
void UnloadAudio_BGM(void);

#endif // AUDIO_BGM_H
