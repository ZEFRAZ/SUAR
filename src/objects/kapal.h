#ifndef KAPAL_H
#define KAPAL_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file kapal.h
 * @brief Deklarasi fungsi penggambaran semua jenis kapal.
 *
 * Modul ini menyediakan fungsi untuk menggambar 9 jenis kapal yang berbeda.
 * Setiap kapal mendukung mode solid dan wireframe, serta dapat dikustomisasi
 * warna lambung dan detailnya. Kapal layar juga mendukung animasi sailFlap.
 */

/** @brief Menggambar Kapal Kargo lengkap dengan kontainer bertumpuk dan kabin navigasi. */
void DrawShip_Cargo(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire);

/** @brief Menggambar Kapal Pesiar besar berlapis-lapis dengan cerobong oranye. */
void DrawShip_Cruise(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire);

/** @brief Menggambar Kapal Perang militer bergaya perusak dengan meriam dan radar. */
void DrawShip_Military(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire);

/** @brief Menggambar Kapal Nelayan tradisional dengan derek dan tali temali. */
void DrawShip_Fishing(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire);

/** @brief Menggambar Kapal Riset oseanografi dengan lambung bertingkat asimetris. */
void DrawShip_Research(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire);

/** @brief Menggambar Kapal Yacht mewah bergaya modern dan minimalis. */
void DrawShip_Yacht(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire);

/** @brief Menggambar Kapal Bajak Laut varian 0 (klasik dengan tiga tiang). */
void DrawShip_Pirate(float x, float y, float scale, float angle, float sailFlap, Color hullCol, Color sailCol, bool isWire);

/** @brief Menggambar salah satu dari dua varian Kapal Galiung dagang (Eropa atau Jong Asia). */
void DrawShip_Galleon(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire);

/** @brief Menggambar Kapal Layar kecil (Sloop) varian 0 (putih-merah). */
void DrawShip_Sloop(float x, float y, float scale, float angle, float sailFlap, Color hullCol, Color sailCol, bool isWire);

/** @brief Menggambar salah satu dari tiga varian Kapal Bajak Laut. */
void DrawShip_Pirate_Variant(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire);

/** @brief Menggambar salah satu dari tiga varian Kapal Layar kecil (Sloop). */
void DrawShip_Sloop_Variant(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire);

#endif // KAPAL_H