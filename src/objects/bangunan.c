/**
 * @file bangunan.c
 * @brief Penggambaran objek bangunan: Mercusuar.
 *
 * File ini berisi fungsi untuk menggambar mercusuar secara lengkap,
 * mulai dari fondasi, badan menara, pagar balkon, hingga efek cahaya
 * sorot yang berputar. Mendukung mode solid dan wireframe.
 *
 * Catatan: variabel previewTime diambil dari pola2d.c melalui 'extern'
 * untuk menggerakkan arah sorot lampu secara animasi.
 */

#include "bangunan.h"
#include "../ui/draw_helpers.h"
#include <math.h>

// Waktu animasi yang didapat dari pola2d.c — dipakai untuk menggerakkan sorot lampu
extern float previewTime;

/**
 * @brief Menggambar bangunan Mercusuar secara utuh beserta efek lampunya.
 *
 * Objek digambar lapis demi lapis dari bawah ke atas mengikuti prinsip pelukis:
 * fondasi → badan menara → strip merah → pintu → jendela → balkon → atap → lampu → sorot cahaya.
 *
 * @param x       Koordinat X pusat bawah mercusuar di layar.
 * @param y       Koordinat Y pusat bawah mercusuar di layar.
 * @param scale   Faktor skala objek (1.0 = ukuran normal).
 * @param variant Mode lampu: 0 = lampu menyala (malam), 1 = lampu mati (siang).
 * @param isWire  Jika true, hanya garis tepi yang digambar (mode wireframe).
 */
void DrawMercusuar_Pola(float x, float y, float scale, int variant, bool isWire) {
    float origY = y;   // Simpan posisi Y asli untuk perhitungan batas sorot cahaya
    y += 70.0f * scale; // Geser ke bawah agar mercusuar tampak berdiri di atas permukaan

    // =============================================================================
    // Palet warna komponen mercusuar
    // =============================================================================
    Color cBase    = (Color){130, 130, 130, 255}; // Abu-abu gelap untuk fondasi
    Color cTower   = (Color){200, 200, 200, 255}; // Abu-abu terang untuk badan menara
    Color cStripe  = (Color){200,  40,  40, 255}; // Merah untuk strip hiasan dan atap
    Color cDoor    = (Color){100,  60,  30, 255}; // Cokelat tua untuk pintu
    Color cWin     = (Color){100, 150, 255, 180}; // Biru transparan untuk jendela/kaca
    Color cBalcony = (Color){ 50,  50,  50, 255}; // Hitam keabu-abuan untuk pagar balkon

    bool  isLightOn = (variant == 0); // true jika lampu harus menyala

    Color cBulb  = isLightOn ? (Color){255, 240, 50, 255} : (Color){80, 80, 85, 255}; // Warna bohlam
    Color cGlow1 = (Color){255, 215,   0,  80};  // Cahaya luar (transparan)
    Color cGlow2 = (Color){255, 215,   0, 150};  // Cahaya dalam (lebih terang)

    Color outLine    = isWire ? GREEN : BLANK;     // Garis tepi: hijau di wireframe, tidak ada di solid
    Color outRailing = isWire ? GREEN : cBalcony;  // Warna pagar (sama di keduanya, beda di wireframe)

    // =============================================================================
    // BAGIAN 1: BADAN BANGUNAN (dari bawah ke atas)
    // =============================================================================

    // Fondasi trapesium di bagian paling bawah
    Vector2 base[] = {{-40, 0}, {40, 0}, {35, -20}, {-35, -20}};
    DrawCustomPoly(base, 4, x, y, scale, 0, cBase, outLine, isWire);

    // Badan menara utama (menyempit ke atas)
    Vector2 tower[] = {{-35, -20}, {35, -20}, {20, -180}, {-20, -180}};
    DrawCustomPoly(tower, 4, x, y, scale, 0, cTower, outLine, isWire);

    // Strip merah pertama (bagian bawah menara)
    Vector2 stripe1[] = {{-33.5f, -40}, {33.5f, -40}, {30.8f, -70}, {-30.8f, -70}};
    DrawCustomPoly(stripe1, 4, x, y, scale, 0, cStripe, outLine, isWire);

    // Strip merah kedua (bagian tengah menara)
    Vector2 stripe2[] = {{-28.0f, -100}, {28.0f, -100}, {25.2f, -130}, {-25.2f, -130}};
    DrawCustomPoly(stripe2, 4, x, y, scale, 0, cStripe, outLine, isWire);

    // Strip merah ketiga (bagian atas menara, di bawah balkon)
    Vector2 stripe3[] = {{-22.4f, -160}, {22.4f, -160}, {20.5f, -180}, {-20.5f, -180}};
    DrawCustomPoly(stripe3, 4, x, y, scale, 0, cStripe, outLine, isWire);

    // Bagian bawah pintu (persegi panjang)
    Vector2 doorBase[] = {{-10, 0}, {10, 0}, {9, -30}, {-9, -30}};
    DrawCustomPoly(doorBase, 4, x, y, scale, 0, cDoor, outLine, isWire);

    // Bagian atas pintu (lengkung setengah lingkaran sebagai 11 titik poligon)
    Vector2 doorTop[15]; int dtIdx = 0;
    for (int i = 0; i <= 10; i++) {
        float a = PI + i * (PI / 10.0f);
        doorTop[dtIdx++] = (Vector2){0 + cosf(a)*9.0f, -30 + sinf(a)*9.0f};
    }
    DrawCustomPoly(doorTop, dtIdx, x, y, scale, 0, cDoor, outLine, isWire);

    // Dua jendela kecil di sisi kiri-kanan menara
    Color outWinLines = isWire ? GREEN : cBalcony;
    DrawCustomRect(-6, -93, 12, 16, x, y, scale, 0, cWin, outWinLines, isWire);
    DrawCustomRect(-5, -152, 10, 14, x, y, scale, 0, cWin, outWinLines, isWire);

    // Kotak kaca ruang lampu di puncak menara
    DrawCustomRect(-18, -210, 36, 25, x, y, scale, 0, cWin, isWire ? GREEN : BLANK, isWire);

    // Lantai pagar balkon (bidang datar)
    DrawCustomRect(-28, -185, 56, 5, x, y, scale, 0, cBalcony, outLine, isWire);

    // Jeruji pagar balkon (4 tiang vertikal + 1 palang horisontal)
    // Digambar dua kali dengan offset ±0.5 agar terlihat lebih tebal di mode solid
    for (float off = -0.5f; off <= 0.5f; off += 1.0f) {
        DrawCustomLine((Vector2){-28+off, -185}, (Vector2){-28+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){ 28+off, -185}, (Vector2){ 28+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){-14+off, -185}, (Vector2){-14+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){ 14+off, -185}, (Vector2){ 14+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){-28, -195+off}, (Vector2){ 28, -195+off}, x, y, scale, 0, outRailing);
        if (isWire) break; // Di wireframe cukup digambar sekali
    }

    // Atap segitiga merah di puncak menara
    Vector2 roof[] = {{-22, -210}, {22, -210}, {0, -230}};
    DrawCustomPoly(roof, 3, x, y, scale, 0, cStripe, outLine, isWire);

    // Penangkal petir kecil di ujung atap
    DrawCustomCircle((Vector2){0, -232}, 4.0f, x, y, scale, 0, cStripe, outLine, isWire);

    // =============================================================================
    // BAGIAN 2: EFEK CAHAYA SOROT (hanya di mode solid saat lampu menyala)
    // =============================================================================
    if (isLightOn && !isWire) {
        // Hitung sudut sorot menggunakan fungsi sinus agar bergerak bolak-balik
        float minBeam    = 100.0f * DEG2RAD;
        float maxBeam    = 180.0f * DEG2RAD;
        float sweepAngle = minBeam + (sinf(previewTime * 2.0f) * 0.5f + 0.5f) * (maxBeam - minBeam);
        float beamLength = 3000.0f * scale;
        float spread     = 0.35f; // Lebar sebaran cahaya dalam radian

        Vector2 lCenter = TransformPola(0, -197, x, y, scale, 0); // Posisi pusat lampu di layar

        // Titik-titik tepi segitiga sorot (luar)
        Vector2 p1 = { lCenter.x + cosf(sweepAngle - spread) * beamLength,
                       lCenter.y + sinf(sweepAngle - spread) * beamLength };
        Vector2 p2 = { lCenter.x + cosf(sweepAngle + spread) * beamLength,
                       lCenter.y + sinf(sweepAngle + spread) * beamLength };

        // Batas pemangkasan agar sorot tidak keluar terlalu jauh
        float minX =  x - 3000.0f;
        float maxX =  x + 3000.0f;
        float minY = origY - 2000.0f;
        float maxY = origY + 2000.0f;

        // Sorot luar: lebih lebar, lebih transparan
        DrawClippedBeamTriangle(lCenter, p1, p2, minX, minY, maxX, maxY, (Color){255, 255, 150, 40});

        // Sorot dalam: lebih sempit, lebih terang (inti cahaya)
        Vector2 p1_in = { lCenter.x + cosf(sweepAngle - spread*0.3f) * beamLength,
                          lCenter.y + sinf(sweepAngle - spread*0.3f) * beamLength };
        Vector2 p2_in = { lCenter.x + cosf(sweepAngle + spread*0.3f) * beamLength,
                          lCenter.y + sinf(sweepAngle + spread*0.3f) * beamLength };
        DrawClippedBeamTriangle(lCenter, p1_in, p2_in, minX, minY, maxX, maxY, (Color){255, 255, 200, 80});
    }

    // =============================================================================
    // BAGIAN 3: BOHLAM LAMPU DI PUNCAK (selalu gambar, beda warna saat mati/nyala)
    // =============================================================================
    if (isLightOn && !isWire) {
        DrawCustomCircle((Vector2){0, -197}, 16.0f, x, y, scale, 0, cGlow1, BLANK, false); // Pancaran luar
        DrawCustomCircle((Vector2){0, -197}, 11.0f, x, y, scale, 0, cGlow2, BLANK, false); // Pancaran dalam
    }
    DrawCustomCircle((Vector2){0, -197}, 6.0f, x, y, scale, 0, cBulb, outLine, isWire); // Bohlam inti
}