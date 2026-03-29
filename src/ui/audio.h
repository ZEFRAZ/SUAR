#ifndef AUDIO_BGM_H
#define AUDIO_BGM_H

/**
 * @file audio.h
 * @brief Deklarasi antarmuka manajemen perangkat audio dan streaming musik latar.
 * 
 * Melingkupi inisialiasi perangkat *hardware* tingkat rendah, alokasi *buffer* saluran audio,
 * serta manajemen siklus dekoding file `.mp3` ke memori aplikasi.
 */

/** @brief Membuka saluran perangkat audio dan menginisiasi buffer putar balik berulang (Looping Background Music). */
void InitAudio_BGM(void);

/** @brief Mensuplai buffer instruksi *stream* pemutaran secara kontinu. Diwajibkan di dalam iterasi *game loop* utama. */
void UpdateAudio_BGM(void);

/** @brief Memproyeksikan komponen antarmuka *Mute/Unmute* pada matriks koordinat layar absolut. */
void DrawMuteButton(void);

/** @brief Menedemisi (Deallocation) tumpukan memori aliran suara dan menutup perangkat audio keras dari *Thread* utama. */
void UnloadAudio_BGM(void);

#endif // AUDIO_BGM_H