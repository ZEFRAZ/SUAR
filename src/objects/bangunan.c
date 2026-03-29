// =============================================================================
//  bangunan.c — Objek Bangunan: Mercusuar
//  Proyek   : Mercusuar (Komputer Grafis – Semester 4)
//  Deskripsi: Berisi fungsi penggambaran Mercusuar beserta efek cahaya dan
//             balkon. Mendukung mode Solid dan Wireframe (isWire).
//  Catatan  : previewTime diambil dari pola2d.c via 'extern' untuk animasi
//             berkas cahaya (beam) yang bergerak.
// =============================================================================

#include "bangunan.h"
#include "../ui/draw_helpers.h"
#include <math.h>

// Spesifikasi Variabel Asosiasi Terluar (External Variable Linkage)
extern float previewTime; // Determinan interpolasi sudut pancaran berkas (Beam Sweep Time Parameter)

/**
 * @brief Rutin utama dalam memproyeksikan satu unit terpadu struktur menara suar (Lighthouse).
 * 
 * Struktur dipecah ke dalam berbagai poligon tertutup menggunakan primitif pembungkus `DrawCustomPoly()`.
 * Penggambaran diurutkan dengan prinsip pelukis (Painter's Algorithm) di mana bagian fondasi padat
 * digambar lebih dahulu dan cahaya transparan ditambahkan paling akhir.
 * 
 * @param x Posisi absolut X pada koordinat dunia.
 * @param y Posisi absolut Y pada koordinat dunia.
 * @param scale Modifikasi proporsi skala seragam (1.0 melambangkan rasio basis 1:1).
 * @param variant Pemilihan tipe perenderan (0 = Aktif memancarkan cahaya, 1 = Nonaktif).
 * @param isWire Indikator instruksi penganalisaan tepi verteks (Wireframe Mode).
 */
void DrawMercusuar_Pola(float x, float y, float scale, int variant, bool isWire) {
    // Preservasi Titik Jangkar Y Absolut (Absolute Y-Anchor Preservation)
    float origY = y;

    // Transposisi Origin Pusat Massa (Center-of-Mass Offset)
    y += 70.0f * scale;

    // Alokasi Palet Pewarnaan Konstanta (Constant Color Palette Matrix)
    Color cBase    = (Color){130, 130, 130, 255}; 
    Color cTower   = (Color){200, 200, 200, 255}; 
    Color cStripe  = (Color){200,  40,  40, 255}; 
    Color cDoor    = (Color){100,  60,  30, 255}; 
    Color cWin     = (Color){100, 150, 255, 180}; 
    Color cBalcony = (Color){ 50,  50,  50, 255}; 

    bool isLightOn = (variant == 0); // Boolean Penentu Aktivasi Sistem Iluminasi

    Color cBulb  = isLightOn ? (Color){255, 240, 50, 255} : (Color){80, 80, 85, 255};
    Color cGlow1 = (Color){255, 215,   0,  80}; // Konstanta Alpha Rendah (Outer Mask)
    Color cGlow2 = (Color){255, 215,   0, 150}; // Konstanta Alpha Tinggi (Inner Core)

    Color outLine    = isWire ? GREEN : BLANK;
    Color outRailing = isWire ? GREEN : cBalcony;

    // =========================================================================
    // LAYER 1 — TOPOLOGI POLIGON SILUET BANGUNAN (Architectural Silhouette Topology)
    // =========================================================================

    // 1. Matriks Travesium Basis Pondasi (Foundation Trapeze Base)
    Vector2 base[] = {{-40, 0}, {40, 0}, {35, -20}, {-35, -20}};
    DrawCustomPoly(base, 4, x, y, scale, 0, cBase, outLine, isWire);

    // 2. Formasi Utama Silinder Prisma (Primary Tapered Cylinder)
    Vector2 tower[] = {{-35, -20}, {35, -20}, {20, -180}, {-20, -180}};
    DrawCustomPoly(tower, 4, x, y, scale, 0, cTower, outLine, isWire);

    // 3. Menggambar garis strip hiasan pada dinding
    Vector2 stripe1[] = {{-33.5f, -40}, {33.5f, -40}, {30.8f, -70}, {-30.8f, -70}};
    DrawCustomPoly(stripe1, 4, x, y, scale, 0, cStripe, outLine, isWire);

    Vector2 stripe2[] = {{-28.0f, -100}, {28.0f, -100}, {25.2f, -130}, {-25.2f, -130}};
    DrawCustomPoly(stripe2, 4, x, y, scale, 0, cStripe, outLine, isWire);

    Vector2 stripe3[] = {{-22.4f, -160}, {22.4f, -160}, {20.5f, -180}, {-20.5f, -180}};
    DrawCustomPoly(stripe3, 4, x, y, scale, 0, cStripe, outLine, isWire);

    // 4. Konstruksi Geometri Lengkung Portik (Semi-Circle Portico Extrusion)
    Vector2 doorBase[] = {{-10, 0}, {10, 0}, {9, -30}, {-9, -30}};
    DrawCustomPoly(doorBase, 4, x, y, scale, 0, cDoor, outLine, isWire);

    Vector2 doorTop[15]; int dtIdx = 0;
    for (int i = 0; i <= 10; i++) {
        float a = PI + i * (PI / 10.0f);
        doorTop[dtIdx++] = (Vector2){0 + cosf(a)*9.0f, -30 + sinf(a)*9.0f};
    }
    DrawCustomPoly(doorTop, dtIdx, x, y, scale, 0, cDoor, outLine, isWire);

    // 5. Penetapan Rongga Jendela Ventilator (Ventilation Shaft Allocation)
    Color outWinLines = isWire ? GREEN : cBalcony;
    DrawCustomRect(-6, -93, 12, 16, x, y, scale, 0, cWin, outWinLines, isWire);
    DrawCustomRect(-5, -152, 10, 14, x, y, scale, 0, cWin, outWinLines, isWire);

    // 6. Integrasi Prisma Rumah Tungku (Lamp Furnace Compartment)
    DrawCustomRect(-18, -210, 36, 25, x, y, scale, 0, cWin, isWire ? GREEN : BLANK, isWire);

    // 7. Menggambar susunan pagar luar
    DrawCustomRect(-28, -185, 56, 5, x, y, scale, 0, cBalcony, outLine, isWire);

    // Perulangan Antialiasing Spasial Superposisi (Spatial Superposition Loop)
    for (float off = -0.5f; off <= 0.5f; off += 1.0f) {
        DrawCustomLine((Vector2){-28+off, -185}, (Vector2){-28+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){ 28+off, -185}, (Vector2){ 28+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){-14+off, -185}, (Vector2){-14+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){ 14+off, -185}, (Vector2){ 14+off, -195}, x, y, scale, 0, outRailing);
        DrawCustomLine((Vector2){-28, -195+off}, (Vector2){ 28, -195+off}, x, y, scale, 0, outRailing);
        if (isWire) break; // Bypass Limitasi Kinerja Wireframe
    }

    // 8. Menggambar kubah atap mercusuar
    Vector2 roof[] = {{-22, -210}, {22, -210}, {0, -230}};
    DrawCustomPoly(roof, 3, x, y, scale, 0, cStripe, outLine, isWire);
    DrawCustomCircle((Vector2){0, -232}, 4.0f, x, y, scale, 0, cStripe, outLine, isWire);

    // =========================================================================
    // LAPISAN 2 — POST-PROCESSING CAHAYA KONUS (Conical Illumination Masking)
    // =========================================================================

    // 9. Kalkulasi Sapuan Radius Emisi (Rotational Sweep Emission Matrix)
    if (isLightOn && !isWire) {
        float minBeam = 100.0f * DEG2RAD;
        float maxBeam = 180.0f * DEG2RAD;

        // Interpolasi Sudut Sapuan Bidang Sinusoida (Sine-Based Sweep Sweep)
        float sweepAngle = minBeam + (sinf(previewTime * 2.0f) * 0.5f + 0.5f) * (maxBeam - minBeam);
        float beamLength = 3000.0f * scale;
        float spread     = 0.35f; // Radian Lebar Sebaran (Cone Spread Angle)

        Vector2 lCenter = TransformPola(0, -197, x, y, scale, 0);
        Vector2 p1 = { lCenter.x + cosf(sweepAngle - spread) * beamLength,
                       lCenter.y + sinf(sweepAngle - spread) * beamLength };
        Vector2 p2 = { lCenter.x + cosf(sweepAngle + spread) * beamLength,
                       lCenter.y + sinf(sweepAngle + spread) * beamLength };

        // Konstanta Kliping Area Viewport Global (Absolute Bounding Box)
        float minX =  x - 3000.0f;
        float maxX =  x + 3000.0f;
        float minY = origY - 2000.0f; // Limit Asimtotis Minimum Y
        float maxY = origY + 2000.0f; // Limit Asimtotis Maksimum Y

        // Eksekusi Pelerai Filter Masker Pertama (Outer Blend Mask)
        DrawClippedBeamTriangle(lCenter, p1, p2, minX, minY, maxX, maxY, (Color){255, 255, 150, 40});

        // Eksekusi Pelerai Filter Masker Kedua (Inner Blend Mask)
        Vector2 p1_in = { lCenter.x + cosf(sweepAngle - spread*0.3f) * beamLength,
                          lCenter.y + sinf(sweepAngle - spread*0.3f) * beamLength };
        Vector2 p2_in = { lCenter.x + cosf(sweepAngle + spread*0.3f) * beamLength,
                          lCenter.y + sinf(sweepAngle + spread*0.3f) * beamLength };
        DrawClippedBeamTriangle(lCenter, p1_in, p2_in, minX, minY, maxX, maxY, (Color){255, 255, 200, 80});
    }

    // 10. Representasi Inti Pendar Filament (Primary Filament Z-Index Masking)
    if (isLightOn && !isWire) {
        DrawCustomCircle((Vector2){0, -197}, 16.0f, x, y, scale, 0, cGlow1, BLANK, false); 
        DrawCustomCircle((Vector2){0, -197}, 11.0f, x, y, scale, 0, cGlow2, BLANK, false); 
    }
    // Eksekusi Komponen Bohlam Kondisional Tunggal (Singular Bulb Solid Override)
    DrawCustomCircle((Vector2){0, -197}, 6.0f, x, y, scale, 0, cBulb, outLine, isWire); 
}