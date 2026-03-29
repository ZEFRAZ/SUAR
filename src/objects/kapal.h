#ifndef KAPAL_H
#define KAPAL_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file kapal.h
 * @brief Deklarasi matriks perenderan armada maritim (Maritime Fleet Rendering Matrix).
 * 
 * Mengekspos pustaka rutin pendefinisian entitas maritim tingkat tinggi.
 * Setiap model kapal direpresentasikan oleh kumpulan poligon vektor kompleks,
 * kurva Bezier, dan simulasi partikel (asap cerobong), yang diproses secara dinamis
 * berdasarkan perhitungan posisi dan ukuran (Pergeseran, Skala, Putaran).
 */

/** @brief Merender entitas Kapal Kargo komersial pengangkut peti kemas. */
void DrawShip_Cargo(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWireframe);

/** @brief Merender entitas Kapal Pesiar wisata berskala besar dengan multi-geladak. */
void DrawShip_Cruise(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWireframe);

/** @brief Merender entitas Kapal Perang militer kelas perusak bersenjatakan meriam balistik. */
void DrawShip_Military(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWireframe);

/** @brief Merender entitas Kapal Nelayan pukat tarik tradisional berskala kecil. */
void DrawShip_Fishing(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWireframe);

/** @brief Merender entitas Kapal Riset observasi oseanografi tingkat lanjut. */
void DrawShip_Research(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWireframe);

/** @brief Merender entitas Kapal Ekspedisi Pesiar mewah (Luxury Yacht) bersiluet minimalis modern. */
void DrawShip_Yacht(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWireframe);

/** @brief Pembungkus basis (Base Wrapper) untuk rendering formasi Kapal Bajak Laut layar ganda. */
void DrawShip_Pirate(float x, float y, float scale, float angle, float sailFlap, Color hullCol, Color sailCol, bool isWireframe);

/** @brief Mengeksekusi perenderan kapal bertiang ganda (Galleon) berlambung masif tipe Armada Spanyol. */
void DrawShip_Galleon(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire);

/** @brief Pembungkus basis (Base Wrapper) untuk rendering formasi Kapal Layar tunggal (Sloop). */
void DrawShip_Sloop(float x, float y, float scale, float angle, float sailFlap, Color hullCol, Color sailCol, bool isWireframe);

/** @brief Pemilih variasi turunan dari model dasar Kapal Bajak Laut berlambung besar. */
void DrawShip_Pirate_Variant(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire);

/** @brief Pemilih variasi turunan dari model dasar Kapal Layar Sloop bermanuver lincah. */
void DrawShip_Sloop_Variant(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire);

#endif // KAPAL_H