#ifndef DRAW_HELPERS_H
#define DRAW_HELPERS_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file draw_helpers.h
 * @brief Deklarasi fungsi-fungsi bantu penggambaran objek 2D.
 *
 * File ini mendeklarasikan dua kelompok fungsi:
 * 1. Pengisi bidang solid (segitiga dan lingkaran) berbasis algoritma Scanline.
 * 2. Fungsi penggambaran objek yang mendukung rotasi, skala, dan translasi.
 */

// =============================================================================
// BAGIAN 1: PENGISIAN BIDANG SOLID (SCANLINE RASTERIZER)
// =============================================================================

/**
 * @brief Mengisi bidang segitiga dengan warna menggunakan metode Scanline.
 *
 * @param p1 Titik pertama segitiga.
 * @param p2 Titik kedua segitiga.
 * @param p3 Titik ketiga segitiga.
 * @param c  Warna isi segitiga.
 */
void FillTri_Fast(Vector2 p1, Vector2 p2, Vector2 p3, Color c);

/**
 * @brief Mengisi lingkaran penuh dengan warna menggunakan metode Scanline berbasis Midpoint.
 *
 * @param centerX Koordinat X pusat lingkaran.
 * @param centerY Koordinat Y pusat lingkaran.
 * @param radius  Jari-jari lingkaran dalam piksel.
 * @param color   Warna isi lingkaran.
 */
void FillCircle_Fast(int centerX, int centerY, int radius, Color color);

// =============================================================================
// BAGIAN 2: TRANSFORMASI DAN PENGGAMBARAN OBJEK
// =============================================================================

/**
 * @brief Mengatur algoritma garis yang aktif secara global (Bresenham atau DDA).
 *
 * @param useBresenham true = Bresenham, false = DDA.
 */
void SetDrawHelperAlgorithm(bool useBresenham);

/**
 * @brief Menggambar garis menggunakan algoritma yang sedang aktif.
 *
 * @param x1 Koordinat X titik awal.
 * @param y1 Koordinat Y titik awal.
 * @param x2 Koordinat X titik akhir.
 * @param y2 Koordinat Y titik akhir.
 * @param c  Warna garis.
 */
void AlgoLine(int x1, int y1, int x2, int y2, Color c);

/**
 * @brief Mengubah posisi titik lokal ke koordinat layar berdasarkan translasi, rotasi, dan skala.
 *
 * @param px    Koordinat X titik lokal.
 * @param py    Koordinat Y titik lokal.
 * @param tx    Posisi X objek di layar.
 * @param ty    Posisi Y objek di layar.
 * @param scale Faktor skala (1.0 = normal).
 * @param angle Sudut rotasi dalam radian.
 * @return      Koordinat titik setelah transformasi.
 */
Vector2 TransformPola(float px, float py, float tx, float ty, float scale, float angle);

/**
 * @brief Menggambar poligon dengan warna isi dan garis tepi.
 *
 * @param localPts Array titik-titik lokal.
 * @param count    Jumlah titik.
 * @param tx       Posisi X di layar.
 * @param ty       Posisi Y di layar.
 * @param scale    Faktor skala.
 * @param angle    Sudut rotasi dalam radian.
 * @param fill     Warna isi.
 * @param outline  Warna garis tepi.
 * @param isWire   Jika true, hanya garis tepi (wireframe).
 */
void DrawCustomPoly(Vector2* localPts, int count, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/**
 * @brief Menggambar kurva Bezier kuadratik dari tiga titik.
 *
 * @param p0      Titik awal kurva.
 * @param p1      Titik kontrol (arah lengkungan).
 * @param p2      Titik akhir kurva.
 * @param tx      Posisi X di layar.
 * @param ty      Posisi Y di layar.
 * @param scale   Faktor skala.
 * @param angle   Sudut rotasi dalam radian.
 * @param fill    Warna isi area kurva.
 * @param outline Warna garis tepi.
 * @param isWire  Jika true, hanya garis tepi (wireframe).
 */
void DrawCustomBezier(Vector2 p0, Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/**
 * @brief Menggambar satu segmen garis lurus dengan transformasi otomatis.
 *
 * @param p1    Titik awal (koordinat lokal).
 * @param p2    Titik akhir (koordinat lokal).
 * @param tx    Posisi X di layar.
 * @param ty    Posisi Y di layar.
 * @param scale Faktor skala.
 * @param angle Sudut rotasi dalam radian.
 * @param col   Warna garis.
 */
void DrawCustomLine(Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color col);

/**
 * @brief Menggambar lingkaran dengan warna isi dan tepi, disesuaikan transformasi objek.
 *
 * @param center  Posisi pusat (koordinat lokal).
 * @param radius  Jari-jari lingkaran.
 * @param tx      Posisi X di layar.
 * @param ty      Posisi Y di layar.
 * @param scale   Faktor skala.
 * @param angle   Sudut rotasi dalam radian.
 * @param fill    Warna isi.
 * @param outline Warna tepi.
 * @param isWire  Jika true, hanya garis tepi (wireframe).
 */
void DrawCustomCircle(Vector2 center, float radius, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/**
 * @brief Menggambar cincin (donat) dengan transformasi objek.
 *
 * @param center    Posisi pusat (koordinat lokal).
 * @param radius    Jari-jari tengah cincin.
 * @param thickness Ketebalan cincin.
 * @param tx        Posisi X di layar.
 * @param ty        Posisi Y di layar.
 * @param scale     Faktor skala.
 * @param angle     Sudut rotasi dalam radian.
 * @param color     Warna cincin.
 * @param isWire    Jika true, hanya garis tepi (wireframe).
 */
void DrawCustomRing(Vector2 center, float radius, float thickness, float tx, float ty, float scale, float angle, Color color, bool isWire);

/**
 * @brief Menggambar persegi panjang dengan rotasi dan transformasi objek.
 *
 * @param rx      Koordinat X lokal pojok kiri atas.
 * @param ry      Koordinat Y lokal pojok kiri atas.
 * @param rw      Lebar persegi panjang.
 * @param rh      Tinggi persegi panjang.
 * @param tx      Posisi X di layar.
 * @param ty      Posisi Y di layar.
 * @param scale   Faktor skala.
 * @param angle   Sudut rotasi dalam radian.
 * @param fill    Warna isi.
 * @param outline Warna garis tepi.
 * @param isWire  Jika true, hanya garis tepi (wireframe).
 */
void DrawCustomRect(float rx, float ry, float rw, float rh, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/**
 * @brief Menggambar segitiga sorot cahaya dengan pemangkasan pada batas layar tertentu.
 *
 * Digunakan khusus untuk efek sinar lampu mercusuar agar cahaya tidak keluar
 * dari batas area yang diizinkan.
 *
 * @param p0   Titik pusat sumber cahaya.
 * @param p1   Titik tepi kiri sorotan.
 * @param p2   Titik tepi kanan sorotan.
 * @param minX Batas kiri area tampil.
 * @param minY Batas atas area tampil.
 * @param maxX Batas kanan area tampil.
 * @param maxY Batas bawah area tampil.
 * @param col  Warna sorotan.
 */
void DrawClippedBeamTriangle(Vector2 p0, Vector2 p1, Vector2 p2, float minX, float minY, float maxX, float maxY, Color col);

#endif // DRAW_HELPERS_H