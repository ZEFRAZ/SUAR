#ifndef ALAM_H
#define ALAM_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file alam.h
 * @brief Deklarasi komponen entitas lingkungan dan cuaca.
 * 
 * Header ini menaungi antarmuka pemrograman untuk pembuatan objek prosedural alam 
 * yang mencakup elemen benda angkasa (matahari, bulan, konstelasi bintang),
 * formasi geografis (gunung, tebing), hidrologi (ombak berlapis), 
 * hingga fenomena meteorologi (awan, kabut, curah hujan presipitasi).
 */

void DrawAwan_Pola(int type, float x, float y, float scale, Color c, bool isWireframe);
void DrawBurung_Pola(float x, float y, float scale, float flapPhase, Color c, bool isWireframe);
void DrawGunung_Pola(float x, float y, float w, float h, Color baseColor, bool isWireframe);
void DrawTebing_Pola(float x, float y, float w, float h, Color rockCol, Color grassCol, bool isWireframe);
void DrawBulanMatahari_Pola(float x, float y, float scale, bool isWireframe);
void DrawMatahari_Animasi(float x, float y, float scale, float daylight, bool isWire);
void DrawBulan_Animasi(float x, float y, float scale, float nightlight, bool isWire);
void DrawBintang_Pola(float x, float y, float scale, Color c, bool isWire);
void DrawKabut_Pola(float x, float y, float scale, bool isWire);

/** @brief Fungsi sistem dinamika fluida berlandaskan Fungsi Sinus Ganda */
void DrawOmbak_Pola(float x, float y, float w, float h, bool isWire);
void DrawOmbak_Lapisan(float startX, float endX, float bottomY, float seaBaseY, float amplitude, float frequency, float baseSpeed, Color colTop, Color colBot, float offset, bool hasFoam, float pWave, float daylight, bool isWire);

void DrawCuaca_HujanBadai(float x, float y, float w, float h, bool isStorm, bool isWire);
void DrawRasiBintang_Animasi(Vector2 *stars, int starCount, float nightlight, bool isWire);

#endif // ALAM_H