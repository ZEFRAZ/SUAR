#ifndef BANGUNAN_H
#define BANGUNAN_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file bangunan.h
 * @brief Deklarasi entitas arsitektur buatan (Man-made Architecture).
 * 
 * Modul ini mengekspos fungsi-fungsi yang bertanggung jawab merender objek bangunan solid
 * dengan geometri kompleks yang melibatkan operasi tumpukan poligon (Polygon Overlay)
 * dan komputasi proyeksi pencahayaan relasional (Relational Lighting Projection).
 */

/**
 * @brief Menggambar bangunan Mercusuar secara utuh beserta lampunya.
 * 
 * Melakukan perenderan tumpukan lapisan (layering) dimulai dari pondasi bawah bersudut tumpul
 * hingga ruang kaca lampu di elevasi tertinggi.
 * 
 * @param x Koordinat dasar absis (Tengah).
 * @param y Koordinat dasar ordinat (Tengah).
 * @param scale Skalar pengali seragam (Uniform Scaling Factor).
 * @param variant Pengondisian sakelar visibilitas lampu (0 = Menyala, 1 = Padam).
 * @param isWireframe Penentu mode inspeksi kerangka (True = Hijau).
 */
void DrawMercusuar_Pola(float x, float y, float scale, int variant, bool isWireframe);

#endif // BANGUNAN_H