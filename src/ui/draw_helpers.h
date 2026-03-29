#ifndef DRAW_HELPERS_H
#define DRAW_HELPERS_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file draw_helpers.h
 * @brief Deklarasi antarmuka layanan perenderan utilitas dan manipulasi matriks geometri.
 * 
 * Modul ini mengekspos dua kategori utama operasi perenderan: fungsionalitas pengisi poligon dasar (Basic Polygon Fill)
 * serta fungsionalitas perenderan vektor tingkat tinggi yang telah dilengkapi transformasi matriks affine
 * (Rotasi, Penskalaan, Translasi) untuk mensimulasikan fungsionalitas `ctx.transform()` pada ranah grafika web.
 */

// =========================================================================================
// 1. PENGISI POLIGON DASAR (BASIC POLYGON FIIL ALGORITHMS)
// Implementasi pengisian bentuk primitif murni berlandaskan manipulasi Raster Piksel.
// =========================================================================================

/** @brief Algoritma rasterisasi raster untuk entitas trapesium tak beraturan (Irregular Trapezoid). */
void FillTrap(int xl_t, int xr_t, int yt, int xl_b, int xr_b, int yb, Color c);

/** @brief Pengisi kotak ortogonal menggunakan iterasi garis horizontal paralel (Scanline Fill). */
void FillRect(int x, int y, int w, int h, Color c);

/** @brief Pemecahan segitiga arbitrer menjadi dua trapesium proyektif untuk rasterisasi berbasis Scanline. */
void FillTri(int x1, int y1, int x2, int y2, int x3, int y3, Color c);

// =========================================================================================
// 2. UTILITAS GEOMETRI TRANSFORMATIF (TRANSFORMATIVE GEOMETRY UTILITIES)
// =========================================================================================

/** @brief Mengatur status boolean yang mendikte pemakaian algoritma Bresenham(True) vs DDA(False). */
void SetDrawHelperAlgorithm(bool useBresenham);

/** @brief Antarmuka pembungkus generik pensuplai perenderan garis lurus primitif. */
void AlgoLine(int x1, int y1, int x2, int y2, Color c);

/** @brief Mengeksekusi produk perkalian matriks linear 2D (Transformasi Vektor Ruang Lokal ke Ruang Dunia). */
Vector2 TransformPola(float px, float py, float tx, float ty, float scale, float angle);

/** @brief Prosedur generasi poligon tertutup yang dilengkapi pemangku Triangulasi Otomatis (Auto-Triangulation). */
void DrawCustomPoly(Vector2* localPts, int count, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/** @brief Perenderan kurva parametrik derajat tiga bersudut presisi dinamis berskala Bezier. */
void DrawCustomBezier(Vector2 p0, Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/** @brief Wrapper fungsi perenderan segmen batas linear bermuatan param transformasi. */
void DrawCustomLine(Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color col);

/** @brief Menggambarkan representasi primitif disk bundar utuh (Solid Circle Fill + Stroke). */
void DrawCustomCircle(Vector2 center, float radius, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/** @brief Algoritma pembangunan topologi cincin toroid memangkas wilayah internal silinder. */
void DrawCustomRing(Vector2 center, float radius, float thickness, float tx, float ty, float scale, float angle, Color color, bool isWire);

/** @brief Alternatif spesialis perender kotak berotasi bebas (Oriented Bounding Box Rendering). */
void DrawCustomRect(float rx, float ry, float rw, float rh, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire);

/** @brief Mengaplikasikan implementasi kliping poligon Sutherland-Hodgman untuk iluminasi konis (Conical Beam). */
void DrawClippedBeamTriangle(Vector2 p0, Vector2 p1, Vector2 p2, float minX, float minY, float maxX, float maxY, Color col);

#endif // DRAW_HELPERS_H