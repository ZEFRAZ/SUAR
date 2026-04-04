#ifndef ALAM_H
#define ALAM_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file alam.h
 * @brief Deklarasi fungsi penggambaran elemen alam dan cuaca.
 *
 * Modul ini menyediakan fungsi untuk menggambar semua objek lingkungan:
 * awan, burung, gunung, tebing, matahari, bulan, bintang, kabut,
 * ombak berlapis, hujan/badai, dan rasi bintang kustom.
 *
 * Semua fungsi mendukung mode solid (isWire=false) dan wireframe (isWire=true).
 */

/** @brief Menggambar awan bertipe tertentu di posisi dan skala yang diberikan. */
void DrawAwan_Pola(int type, float x, float y, float scale, Color c, bool isWire);

/** @brief Menggambar satu burung camar dengan animasi kepakan sayap. */
void DrawBurung_Pola(float x, float y, float scale, float flapPhase, Color c, bool isWire);

/** @brief Menggambar pemandangan gunung tiga lapis (jauh, tengah, dekat) dengan efek kedalaman. */
void DrawGunung_Pola(float x, float y, float w, float h, Color baseColor, bool isWire);

/** @brief Menggambar tebing pantai beserta rumah penjaga dan pagar pengaman. */
void DrawTebing_Pola(float x, float y, float w, float h, Color rockCol, Color grassCol, bool isWire);

/** @brief Menggambar matahari dan bulan berdampingan untuk layar showcase Pola 2D. */
void DrawBulanMatahari_Pola(float x, float y, float scale, bool isWire);

/** @brief Menggambar matahari dengan efek cahaya berpendar, dipakai di layar Animasi 2D. */
void DrawMatahari_Animasi(float x, float y, float scale, float daylight, bool isWire);

/** @brief Menggambar bulan purnama dengan kawah dan efek cahaya, dipakai di layar Animasi 2D. */
void DrawBulan_Animasi(float x, float y, float scale, float nightlight, bool isWire);

/** @brief Menggambar hamparan bintang berkelip di langit malam. */
void DrawBintang_Pola(float x, float y, float scale, Color c, bool isWire);

/** @brief Menggambar efek kabut tipis di horizon dengan gradasi transparan. */
void DrawKabut_Pola(float x, float y, float scale, bool isWire);

/**
 * @brief Menggambar ombak laut tiga lapis untuk layar showcase Pola 2D.
 *
 * Setiap lapis memiliki amplitudo, frekuensi, dan kecepatan berbeda
 * untuk menciptakan efek kedalaman dan gerak ombak yang alami.
 */
void DrawOmbak_Pola(float x, float y, float w, float h, bool isWire);

/**
 * @brief Menggambar satu lapisan ombak laut dengan buih di puncaknya.
 *
 * Bentuk ombak dihitung dari gabungan dua fungsi sinus dengan frekuensi berbeda
 * agar pola gelombang tidak terlihat berulang (repetitif).
 */
void DrawOmbak_Lapisan(float startX, float endX, float bottomY, float seaBaseY,
                        float amplitude, float frequency, float baseSpeed,
                        Color colTop, Color colBot, float offset, bool hasFoam,
                        float pWave, float daylight, bool isWire);

/** @brief Menggambar cuaca hujan deras atau badai petir dengan kilatan dan butir hujan. */
void DrawCuaca_HujanBadai(float x, float y, float w, float h, bool isStorm, bool isWire);

/** @brief Menggambar rasi bintang kustom dengan garis putus-putus penghubung antar bintang. */
void DrawRasiBintang_Animasi(Vector2 *stars, int starCount, float nightlight, bool isWire);

#endif // ALAM_H