/**
 * @file alam.c
 * @brief Penggambaran semua elemen alam dan cuaca.
 *
 * File ini berisi fungsi untuk menggambar seluruh objek lingkungan:
 * awan, burung, gunung, tebing, matahari, bulan, bintang, kabut,
 * ombak berlapis, hujan/badai petir, dan rasi bintang kustom.
 *
 * Semua fungsi mendukung mode solid (isWire=false) dan wireframe (isWire=true).
 */

#include "alam.h"
#include "../ui/draw_helpers.h"
#include "../algo/dda.h"
#include "../algo/midcircle.h"
#include <math.h>
#include "raymath.h"

// Variabel dari pola2d.c — dibutuhkan untuk animasi dan teks debug
extern Font  polaFont;    // Font untuk teks label di mode wireframe
extern float previewTime; // Waktu animasi global dalam detik

/**
 * @brief Menggambar formasi awan dengan kurva Bezier kubik.
 *
 * Setiap awan dibentuk dari beberapa kurva Bezier kubik yang disambung
 * sehingga menghasilkan bentuk awan yang membulat dan alami.
 *
 * @param type  Jenis bentuk awan (0 hingga 3 — masing-masing bentuk berbeda).
 * @param x     Koordinat X pusat awan di layar.
 * @param y     Koordinat Y pusat awan di layar.
 * @param scale Faktor skala awan.
 * @param c     Warna awan di mode solid.
 * @param isWire Jika true, hanya garis tepi yang digambar.
 */
void DrawAwan_Pola(int type, float x, float y, float scale, Color c, bool isWire) {
    Color outLine = isWire ? GREEN : BLANK;

    // Makro bantu untuk menambah titik kurva Bezier kubik ke dalam array
    #define ADD_CUBIC(arr, idx, px0, py0, px1, py1, px2, py2, px3, py3) \
        for(int _i=1; _i<=12; _i++) { \
            float _t = _i/12.0f; float _inv = 1.0f - _t; \
            float _x = _inv*_inv*_inv*(px0) + 3.0f*_inv*_inv*_t*(px1) + 3.0f*_inv*_t*_t*(px2) + _t*_t*_t*(px3); \
            float _y = _inv*_inv*_inv*(py0) + 3.0f*_inv*_inv*_t*(py1) + 3.0f*_inv*_t*_t*(py2) + _t*_t*_t*(py3); \
            arr[idx++] = (Vector2){_x, _y}; \
        }

    float s = scale * 0.4f; // Skala diperkecil agar awan tidak terlalu besar

    // Setiap awan digambar dengan posisi yang sedikit berbeda agar terlihat tersebar
    float offsetX[4] = {-200.0f, 180.0f, -150.0f, 150.0f};
    float offsetY[4] = {-120.0f, -150.0f, 100.0f, 80.0f};

    for (int cloudType = 0; cloudType < 4; cloudType++) {
        Vector2 pts[50]; int pIdx = 0;
        
        pts[pIdx++] = (Vector2){0, 0}; // Titik awal awan

        if (cloudType == 0) {
            ADD_CUBIC(pts, pIdx, 0, 0, -30, 0, -40, -30, -10, -40);
            ADD_CUBIC(pts, pIdx, -10, -40, 0, -70, 50, -70, 60, -40);
            ADD_CUBIC(pts, pIdx, 60, -40, 90, -40, 90, 0, 60, 0);
        } else if (cloudType == 1) {
            ADD_CUBIC(pts, pIdx, 0, 0, -20, 0, -20, -20, 10, -20);
            ADD_CUBIC(pts, pIdx, 10, -20, 30, -40, 70, -30, 80, -15);
            ADD_CUBIC(pts, pIdx, 80, -15, 110, -15, 110, 0, 80, 0);
        } else if (cloudType == 2) {
            ADD_CUBIC(pts, pIdx, 0, 0, -40, 10, -50, -40, -10, -40);
            ADD_CUBIC(pts, pIdx, -10, -40, -10, -80, 60, -80, 60, -40);
            ADD_CUBIC(pts, pIdx, 60, -40, 100, -40, 90, 10, 50, 0);
        } else if (cloudType == 3) {
            ADD_CUBIC(pts, pIdx, 0, 0, -20, 0, -30, -15, -10, -20);
            ADD_CUBIC(pts, pIdx, -10, -20, 0, -40, 30, -40, 40, -20);
            ADD_CUBIC(pts, pIdx, 40, -20, 60, -20, 60, 0, 40, 0);
        }

        // Gambar poligon awan ke posisi yang sudah digeser
        DrawCustomPoly(pts, pIdx, x + (offsetX[cloudType] * s), y + (offsetY[cloudType] * s), s, 0, c, outLine, isWire);
    }
    
    #undef ADD_CUBIC
}

/**
 * @brief Menggambar satu burung camar dengan animasi kepakan sayap.
 *
 * Sayap kiri dan kanan masing-masing dibentuk dari satu kurva Bezier.
 * Parameter flapPhase menentukan seberapa tinggi ujung sayap terangkat
 * (0.0 = sayap datar, 1.0 = sayap terangkat penuh).
 *
 * @param flapPhase Fase kepakan sayap (0.0 hingga 1.0).
 */
void DrawBurung_Pola(float x, float y, float scale, float flapPhase, Color c, bool isWire) {
    Color outLine = isWire ? GREEN : c;

    // Di mode solid, garis sayap digambar dua kali dengan offset kecil agar terlihat lebih tebal
    float th = isWire ? 0.0f : 1.5f;

    for (float off = -th; off <= th; off += 1.0f) {
        // Sayap kiri — melengkung ke atas mengikuti flapPhase
        DrawCustomBezier((Vector2){-15.0f, -12.0f * flapPhase + off}, (Vector2){-7.0f, 0.0f + off}, (Vector2){0.0f, 0.0f + off}, x, y, scale, 0.0f, BLANK, outLine, isWire);
        // Sayap kanan — simetris dengan sayap kiri
        DrawCustomBezier((Vector2){0.0f, 0.0f + off}, (Vector2){7.0f, 0.0f + off}, (Vector2){15.0f, -12.0f * flapPhase + off}, x, y, scale, 0.0f, BLANK, outLine, isWire);

        if (isWire) break; // Wireframe cukup digambar satu kali
    }
}

/**
 * @brief Menggambar pemandangan gunung tiga lapis (jauh, tengah, dekat).
 *
 * Setiap lapisan terdiri dari beberapa poligon dengan warna yang makin gelap
 * saat semakin dekat ke depan, menciptakan efek kedalaman yang alami.
 * Setiap gunung juga memiliki sisi terang dan sisi gelap untuk kesan dimensi.
 *
 * @param w Lebar area gunung dalam piksel.
 * @param h Tinggi area gunung dalam piksel.
 */
void DrawGunung_Pola(float x, float y, float w, float h, Color baseColor, bool isWire) {
    int bR = 224, bG = 246, bB = 255; 
    Color outLine = isWire ? GREEN : BLANK;

    float leftX = x - w / 2.0f;
    float topY  = y - h / 2.0f;
    float botY  = y + h / 2.0f;

    // Makro bantu untuk mengubah persentase menjadi koordinat piksel
    #define MX(perc) (leftX + (w * (perc)))
    #define MY(perc) (topY + (h * (perc)))

    // =========================================================================
    // LAPISAN 1: GUNUNG JAUH (paling belakang, warna paling terang)
    // =========================================================================
    Color fCol = { (unsigned char)(bR*0.8f),  (unsigned char)(bG*0.8f),  (unsigned char)(bB*0.9f), 255 };
    Color fDrk = { (unsigned char)(bR*0.7f),  (unsigned char)(bG*0.7f),  (unsigned char)(bB*0.8f), 255 };
    Color fLgt = { (unsigned char)(bR*0.85f), (unsigned char)(bG*0.85f), (unsigned char)(bB*0.95f), 255 };

    Vector2 fBase[] = {
        {MX(0.50f), botY}, 
        {MX(1.00f), botY}, {MX(1.00f), MY(0.60f)}, {MX(0.80f), MY(0.42f)}, 
        {MX(0.50f), MY(0.55f)}, {MX(0.25f), MY(0.45f)}, {MX(0.00f), MY(0.60f)}, {MX(0.00f), botY} 
    };
    DrawCustomPoly(fBase, 8, 0, 0, 1.0f, 0.0f, fCol, outLine, isWire);

    // Sisi terang (terkena cahaya) dan sisi gelap puncak kiri
    Vector2 fP1_L[] = { {MX(0.00f), botY}, {MX(0.00f), MY(0.60f)}, {MX(0.25f), MY(0.45f)}, {MX(0.22f), MY(0.52f)}, {MX(0.24f), MY(0.58f)}, {MX(0.0f), botY} };
    DrawCustomPoly(fP1_L, 6, 0, 0, 1.0f, 0.0f, fLgt, outLine, isWire);

    Vector2 fP1_D[] = { {MX(0.50f), botY}, {MX(0.50f), MY(0.55f)}, {MX(0.25f), MY(0.45f)}, {MX(0.28f), MY(0.52f)}, {MX(0.26f), MY(0.58f)}, {MX(0.60f), botY} };
    DrawCustomPoly(fP1_D, 6, 0, 0, 1.0f, 0.0f, fDrk, outLine, isWire);

    // Sisi terang dan sisi gelap puncak kanan
    Vector2 fP2_L[] = { {MX(0.50f), botY}, {MX(0.79f), MY(0.56f)}, {MX(0.77f), MY(0.50f)}, {MX(0.80f), MY(0.42f)}, {MX(0.50f), MY(0.55f)} };
    DrawCustomPoly(fP2_L, 5, 0, 0, 1.0f, 0.0f, fLgt, outLine, isWire);

    Vector2 fP2_D[] = { {MX(1.00f), botY}, {MX(0.81f), MY(0.56f)}, {MX(0.83f), MY(0.50f)}, {MX(0.80f), MY(0.42f)}, {MX(1.00f), MY(0.60f)} };
    DrawCustomPoly(fP2_D, 5, 0, 0, 1.0f, 0.0f, fDrk, outLine, isWire);

    // =========================================================================
    // LAPISAN 2: GUNUNG TENGAH (lebih dekat, warna lebih gelap)
    // =========================================================================
    Color mCol = { (unsigned char)(bR*0.6f),  (unsigned char)(bG*0.6f),  (unsigned char)(bB*0.7f), 255 };
    Color mDrk = { (unsigned char)(bR*0.5f),  (unsigned char)(bG*0.5f),  (unsigned char)(bB*0.6f), 255 };
    Color mLgt = { (unsigned char)(bR*0.65f), (unsigned char)(bG*0.65f), (unsigned char)(bB*0.75f), 255 };

    Vector2 mBase[] = {
        {MX(0.35f), botY}, 
        {MX(0.90f), botY}, {MX(0.90f), MY(0.65f)}, {MX(0.65f), MY(0.48f)}, 
        {MX(0.35f), MY(0.60f)}, {MX(0.15f), MY(0.50f)}, {MX(0.00f), MY(0.60f)}, {MX(0.00f), botY}
    };
    DrawCustomPoly(mBase, 8, 0, 0, 1.0f, 0.0f, mCol, outLine, isWire);

    Vector2 mP1_L[] = { {MX(0.00f), botY}, {MX(0.00f), MY(0.60f)}, {MX(0.15f), MY(0.50f)}, {MX(0.12f), MY(0.58f)}, {MX(0.14f), MY(0.65f)} };
    DrawCustomPoly(mP1_L, 5, 0, 0, 1.0f, 0.0f, mLgt, outLine, isWire);

    Vector2 mP1_D[] = { {MX(0.35f), botY}, {MX(0.35f), MY(0.60f)}, {MX(0.15f), MY(0.50f)}, {MX(0.18f), MY(0.58f)}, {MX(0.16f), MY(0.65f)}, {MX(0.40f), botY} };
    DrawCustomPoly(mP1_D, 6, 0, 0, 1.0f, 0.0f, mDrk, outLine, isWire);

    Vector2 mP2_L[] = { {MX(0.35f), botY}, {MX(0.64f), MY(0.63f)}, {MX(0.62f), MY(0.55f)}, {MX(0.65f), MY(0.48f)}, {MX(0.35f), MY(0.60f)} };
    DrawCustomPoly(mP2_L, 5, 0, 0, 1.0f, 0.0f, mLgt, outLine, isWire);

    Vector2 mP2_D[] = { {MX(0.90f), botY}, {MX(0.66f), MY(0.63f)}, {MX(0.68f), MY(0.55f)}, {MX(0.65f), MY(0.48f)}, {MX(0.90f), MY(0.65f)} };
    DrawCustomPoly(mP2_D, 5, 0, 0, 1.0f, 0.0f, mDrk, outLine, isWire);

    // =========================================================================
    // LAPISAN 3: GUNUNG DEPAN (paling dekat, warna paling gelap)
    // =========================================================================
    Color nCol = { (unsigned char)(bR*0.4f),  (unsigned char)(bG*0.4f),  (unsigned char)(bB*0.5f), 255 };
    Color nDrk = { (unsigned char)(bR*0.3f),  (unsigned char)(bG*0.3f),  (unsigned char)(bB*0.4f), 255 };
    Color nLgt = { (unsigned char)(bR*0.45f), (unsigned char)(bG*0.45f), (unsigned char)(bB*0.55f), 255 };

    Vector2 nBase[] = {
        {MX(0.20f), botY},
        {MX(0.75f), botY}, {MX(0.75f), MY(0.65f)}, {MX(0.45f), MY(0.52f)}, 
        {MX(0.20f), MY(0.65f)}, {MX(0.05f), MY(0.55f)}, {MX(0.00f), MY(0.60f)}, {MX(0.00f), botY}
    };
    DrawCustomPoly(nBase, 8, 0, 0, 1.0f, 0.0f, nCol, outLine, isWire);

    // Puncak kiri — sisi terang dan sisi gelap
    Vector2 nP1_L[] = { {MX(0.00f), botY}, {MX(0.05f), botY}, {MX(0.05f), MY(0.55f)}, {MX(0.00f), MY(0.60f)} };
    DrawCustomPoly(nP1_L, 4, 0, 0, 1.0f, 0.0f, nLgt, outLine, isWire);

    Vector2 nP1_D[] = { {MX(0.05f), botY}, {MX(0.20f), botY}, {MX(0.20f), MY(0.65f)}, {MX(0.05f), MY(0.55f)} };
    DrawCustomPoly(nP1_D, 4, 0, 0, 1.0f, 0.0f, nDrk, outLine, isWire);

    // Puncak tengah — sisi terang dan sisi gelap
    Vector2 nP2_L[] = { {MX(0.20f), botY}, {MX(0.30f), botY}, {MX(0.45f), MY(0.52f)}, {MX(0.20f), MY(0.65f)} };
    DrawCustomPoly(nP2_L, 4, 0, 0, 1.0f, 0.0f, nLgt, outLine, isWire);
    
    Vector2 nP2_D[] = { {MX(0.30f), botY}, {MX(0.75f), botY}, {MX(0.75f), MY(0.65f)}, {MX(0.45f), MY(0.52f)} };
    DrawCustomPoly(nP2_D, 4, 0, 0, 1.0f, 0.0f, nDrk, outLine, isWire);
    
    #undef MX
    #undef MY
}

/**
 * @brief Menggambar tebing pantai beserta rumah penjaga dan pagar pengaman.
 *
 * Tebing dibentuk dari beberapa poligon batu karang dengan warna berbeda
 * untuk memberi kesan dimensi. Permukaan atasnya dilapisi rumput melengkung
 * yang dibentuk dari kurva Bezier kuadratik.
 *
 * @param w Pengali skala keseluruhan tebing.
 */
void DrawTebing_Pola(float x, float y, float w, float h, Color rockCol, Color grassCol, bool isWire) {
    // 'w' berfungsi sebagai pengali skala agar tebing bisa diperbesar bebas
    float s = w; 
    
    Color cR1 = (Color){58, 53, 50, 255};   Color cR2 = (Color){75, 68, 64, 255};
    Color cR3 = (Color){94, 85, 80, 255};   Color cR4 = (Color){115, 105, 98, 255};
    Color cR5 = (Color){138, 127, 119, 255};
    
    Color o1 = isWire ? GREEN : cR1; Color o2 = isWire ? GREEN : cR2;
    Color o3 = isWire ? GREEN : cR3; Color o4 = isWire ? GREEN : cR4;
    Color o5 = isWire ? GREEN : cR5;

    float botY = 200.0f; // Batas bawah laut
    float rX = 210.0f;   

    // 1. Facet (Patahan Geometris) Batu Karang
    Vector2 p1[] = {{-250, botY}, {-140, 15}, {rX, 15}, {rX, botY}}; DrawCustomPoly(p1, 4, x, y, s, 0, cR1, o1, isWire);
    Vector2 p2[] = {{-250, botY}, {-140, 15}, {-60, 15}, {-100, 120}}; DrawCustomPoly(p2, 4, x, y, s, 0, cR2, o2, isWire);
    Vector2 p3[] = {{-140, 15}, {-60, 15}, {-80, 60}}; DrawCustomPoly(p3, 3, x, y, s, 0, cR3, o3, isWire);
    Vector2 p4[] = {{-100, 120}, {-60, 15}, {30, 80}, {-10, botY}}; DrawCustomPoly(p4, 4, x, y, s, 0, cR4, o4, isWire);
    Vector2 p5[] = {{-60, 15}, {30, 15}, {30, 80}}; DrawCustomPoly(p5, 3, x, y, s, 0, cR5, o5, isWire);
    Vector2 p6[] = {{30, 15}, {110, 15}, {70, 150}, {30, 80}}; DrawCustomPoly(p6, 4, x, y, s, 0, cR2, o2, isWire);
    Vector2 p7[] = {{110, 15}, {180, 15}, {140, 90}}; DrawCustomPoly(p7, 3, x, y, s, 0, cR3, o3, isWire);
    
    // Patahan batu sisi kanan tebing
    Vector2 p8[] = {{70, 150}, {110, 15}, {140, 90}, {rX, 40}, {rX, botY}, {150, botY}}; DrawCustomPoly(p8, 6, x, y, s, 0, cR4, o4, isWire);
    Vector2 p9[] = {{180, 15}, {rX, 15}, {rX, 40}}; DrawCustomPoly(p9, 3, x, y, s, 0, cR5, o5, isWire);

    // Makro bantu untuk menambah titik kurva Bezier kuadratik ke dalam array
    #define ADD_Q(arr, idx, px0, py0, px1, py1, px2, py2) \
        for(int _i=1; _i<=10; _i++) { float _t = _i/10.0f; float _in = 1.0f - _t; arr[idx++] = (Vector2){_in*_in*(px0) + 2.0f*_in*_t*(px1) + _t*_t*(px2), _in*_in*(py0) + 2.0f*_in*_t*(py1) + _t*_t*(py2)}; }

    // Lapisan rumput bawah (warna gelap)
    Color cG1 = (Color){45, 70, 35, 255};
    Vector2 g1[100]; int g1Idx = 0;
    g1[g1Idx++] = (Vector2){-145, 0}; g1[g1Idx++] = (Vector2){-145, 25};
    ADD_Q(g1, g1Idx, -145, 25, -130, 40, -110, 25); ADD_Q(g1, g1Idx, -110, 25, -90, 45, -60, 28);
    ADD_Q(g1, g1Idx, -60, 28, -30, 45, 0, 30);      ADD_Q(g1, g1Idx, 0, 30, 40, 40, 80, 25);
    ADD_Q(g1, g1Idx, 80, 25, 110, 35, 140, 22);     ADD_Q(g1, g1Idx, 140, 22, 170, 32, 200, 20);
    // Batas akhir area rumput tebing
    ADD_Q(g1, g1Idx, 200, 20, 205, 24, rX, 18);
    g1[g1Idx++] = (Vector2){rX, 18}; g1[g1Idx++] = (Vector2){rX, 0};
    
    // Lapisan rumput atas (warna lebih terang sebagai highlight)
    Color cG2 = (Color){60, 90, 45, 255};
    Vector2 g2[100]; int g2Idx = 0;
    g2[g2Idx++] = (Vector2){-142, 0}; g2[g2Idx++] = (Vector2){-142, 20};
    ADD_Q(g2, g2Idx, -142, 20, -130, 32, -110, 22); ADD_Q(g2, g2Idx, -110, 22, -90, 38, -60, 25);
    ADD_Q(g2, g2Idx, -60, 25, -30, 40, 0, 28);      ADD_Q(g2, g2Idx, 0, 28, 40, 35, 80, 22);
    ADD_Q(g2, g2Idx, 80, 22, 110, 32, 140, 20);     ADD_Q(g2, g2Idx, 140, 20, 170, 30, 200, 18);
    ADD_Q(g2, g2Idx, 200, 18, 205, 21, rX, 15);
    g2[g2Idx++] = (Vector2){rX, 15}; g2[g2Idx++] = (Vector2){rX, 0};

    if (!isWire) {
        Vector2 gCenter = TransformPola(-145, 0, x, y, s, 0);
        for(int i=0; i<g1Idx-1; i++) { Vector2 p1 = TransformPola(g1[i].x, g1[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g1[i+1].x, g1[i+1].y, x, y, s, 0); FillTri_Fast(gCenter, p1, p2, cG1); FillTri_Fast(gCenter, p2, p1, cG1); }
        for(int i=0; i<g2Idx-1; i++) { Vector2 p1 = TransformPola(g2[i].x, g2[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g2[i+1].x, g2[i+1].y, x, y, s, 0); FillTri_Fast(gCenter, p1, p2, cG2); FillTri_Fast(gCenter, p2, p1, cG2); }
    } else {
        for(int i=0; i<g1Idx-1; i++) { Vector2 p1 = TransformPola(g1[i].x, g1[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g1[i+1].x, g1[i+1].y, x, y, s, 0); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
        for(int i=0; i<g2Idx-1; i++) { Vector2 p1 = TransformPola(g2[i].x, g2[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g2[i+1].x, g2[i+1].y, x, y, s, 0); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
        
        // Tutup kontur bawah rumput agar poligon tampak tertutup
        Vector2 g1First = TransformPola(g1[0].x, g1[0].y, x, y, s, 0);
        Vector2 g1Last  = TransformPola(g1[g1Idx-1].x, g1[g1Idx-1].y, x, y, s, 0);
        AlgoLine((int)g1Last.x, (int)g1Last.y, (int)g1First.x, (int)g1First.y, GREEN);

        Vector2 g2First = TransformPola(g2[0].x, g2[0].y, x, y, s, 0);
        Vector2 g2Last = TransformPola(g2[g2Idx-1].x, g2[g2Idx-1].y, x, y, s, 0);
        AlgoLine((int)g2Last.x, (int)g2Last.y, (int)g2First.x, (int)g2First.y, GREEN);
    }

    // 4. Pagar Pengaman
    Color cRailing = (Color){130, 130, 130, 255};
    for(int i=0; i<=4; i++) { DrawCustomLine((Vector2){-135 + i*20, 0}, (Vector2){-135 + i*20, -12}, x, y, s, 0, isWire?GREEN:cRailing); }
    DrawCustomLine((Vector2){-135, -12}, (Vector2){-55, -12}, x, y, s, 0, isWire?GREEN:cRailing);
    for(int i=0; i<=4; i++) { DrawCustomLine((Vector2){130 + i*20, 0}, (Vector2){130 + i*20, -12}, x, y, s, 0, isWire?GREEN:cRailing); }
    DrawCustomLine((Vector2){130, -12}, (Vector2){210, -12}, x, y, s, 0, isWire?GREEN:cRailing);

    // 5. Rumah Penjaga
    float hX = 95.0f; float hS = 1.2f;
    Color cWall = (Color){220, 215, 210, 255}; Color cRoof = (Color){200, 40, 40, 255}; Color cDoor = (Color){80, 45, 30, 255}; Color cWin = (Color){30, 40, 50, 255};
    DrawCustomRect(hX - 20*hS, -30*hS, 40*hS, 30*hS, x, y, s, 0, cWall, isWire?GREEN:BLANK, isWire);
    Vector2 roof[] = {{hX - 25*hS, -30*hS}, {hX, -55*hS}, {hX + 25*hS, -30*hS}}; DrawCustomPoly(roof, 3, x, y, s, 0, cRoof, isWire?GREEN:BLANK, isWire);
    DrawCustomRect(hX - 6*hS, -14*hS, 12*hS, 14*hS, x, y, s, 0, cDoor, isWire?GREEN:BLANK, isWire);
    DrawCustomRect(hX - 16*hS, -20*hS, 6*hS, 8*hS, x, y, s, 0, cWin, isWire?GREEN:BLANK, isWire);
    DrawCustomRect(hX + 10*hS, -20*hS, 6*hS, 8*hS, x, y, s, 0, cWin, isWire?GREEN:BLANK, isWire);
    
    #undef ADD_Q
}

/**
 * @brief Menggambar matahari dengan efek cahaya berpendar.
 *
 * Efek berpendar dibuat dari beberapa lingkaran transparan yang ditumpuk
 * di belakang lingkaran utama. Tingkat kecerahan diatur oleh parameter daylight.
 *
 * @param daylight Tingkat kecerahan siang hari (0.0 = malam, 1.0 = siang penuh).
 */
void DrawMatahari_Animasi(float x, float y, float scale, float daylight, bool isWire) {
    Color outLine = isWire ? GREEN : BLANK;
    Color cSun = (Color){255, 215, 0, (unsigned char)(255.0f * daylight)};

    if (!isWire && daylight > 0.0f) {
        float maxGlowRadius = 40.0f + 50.0f; 
        for (float r = maxGlowRadius; r > 40.0f; r -= 4.0f) {
            float intensity = 1.0f - ((r - 40.0f) / 50.0f); 
            Color glowColor = (Color){255, 215, 0, (unsigned char)(intensity * 25.0f * daylight)}; 
            // Pusat lokal {0,0} karena koordinat orbit x,y dunia sudah dikirim dari animasi2d
            DrawCustomCircle((Vector2){0, 0}, r, x, y, scale, 0.0f, glowColor, BLANK, false);
        }
    }
    
    if (isWire) cSun = BLANK;
    DrawCustomCircle((Vector2){0, 0}, 40.0f, x, y, scale, 0.0f, cSun, outLine, isWire);
}

/**
 * @brief Menggambar bulan purnama dengan kawah dan efek cahaya berpendar.
 *
 * Kawah digambar sebagai tiga lingkaran kecil berwarna gelap di atas permukaan bulan.
 * Efek pendar dihasilkan dengan cara yang sama seperti matahari.
 *
 * @param nightlight Tingkat kecerahan malam hari (0.0 = siang, 1.0 = malam penuh).
 */
void DrawBulan_Animasi(float x, float y, float scale, float nightlight, bool isWire) {
    Color outLine = isWire ? GREEN : BLANK;
    Color cMoon = (Color){244, 246, 240, (unsigned char)(255.0f * nightlight)};
    Color cCrater = (Color){0, 0, 0, (unsigned char)(25.0f * nightlight)};

    if (!isWire && nightlight > 0.0f) {
        float maxMoonGlow = 30.0f + 30.0f; 
        for (float r = maxMoonGlow; r > 30.0f; r -= 3.0f) {
            float intensity = 1.0f - ((r - 30.0f) / 30.0f); 
            Color glowColor = (Color){255, 255, 255, (unsigned char)(intensity * 30.0f * nightlight)};
            DrawCustomCircle((Vector2){0, 0}, r, x, y, scale, 0.0f, glowColor, BLANK, false);
        }
    }
    
    if (isWire) cMoon = BLANK;
    DrawCustomCircle((Vector2){0, 0}, 30.0f, x, y, scale, 0.0f, cMoon, outLine, isWire);

    Color outCrater = isWire ? GREEN : BLANK;
    Color fillCrater = isWire ? BLANK : cCrater;
    
    DrawCustomCircle((Vector2){-10.0f, -5.0f}, 5.0f, x, y, scale, 0.0f, fillCrater, outCrater, isWire);
    DrawCustomCircle((Vector2){8.0f, 10.0f}, 7.0f, x, y, scale, 0.0f, fillCrater, outCrater, isWire);
    DrawCustomCircle((Vector2){5.0f, -12.0f}, 4.0f, x, y, scale, 0.0f, fillCrater, outCrater, isWire);
}

/**
 * @brief Menggambar matahari dan bulan berdampingan untuk layar showcase Pola 2D.
 *
 * Matahari diletakkan di kiri dan bulan di kanan dari titik tengah yang diberikan.
 */
void DrawBulanMatahari_Pola(float x, float y, float scale, bool isWire) {
    // Geser titik tengah dunia ke kiri (-120) untuk Matahari dan kanan (+120) untuk Bulan
    float sunWorldX = x - (120.0f * scale);
    float moonWorldX = x + (120.0f * scale);

    DrawMatahari_Animasi(sunWorldX, y, scale, 1.0f, isWire);
    DrawBulan_Animasi(moonWorldX, y, scale, 1.0f, isWire);
}

/**
 * @brief Menggambar hamparan bintang berkelip di langit malam.
 *
 * Posisi setiap bintang dihitung menggunakan fungsi hash sinus agar posisinya
 * tetap konsisten setiap frame namun terlihat acak — tanpa perlu menyimpan array posisi.
 * Efek kerlip dibuat dari fungsi sinus yang berjalan dengan kecepatan berbeda per bintang.
 */
void DrawBintang_Pola(float x, float y, float scale, Color c, bool isWire) {
    int numStars = 200; 
    float areaW = 1300.0f; 
    float areaH = 650.0f;  

    float startX = x - areaW / 2.0f;
    float startY = y - areaH / 2.0f - 40.0f;

    #define IN_BOX(px, py) ((px) >= startX && (px) <= endX && (py) >= startY && (py) <= endY)

    // ==========================================
    // 1. GAMBAR BINTANG KELAP-KELIP (TWINKLING)
    // ==========================================
    for (int i = 0; i < numStars; i++) {
        float fracX = fabsf(sinf(i * 12.9898f) * 43758.5453f); fracX -= floorf(fracX);
        float fracY = fabsf(sinf(i * 78.2330f) * 43758.5453f); fracY -= floorf(fracY);
        float fracSize = fabsf(sinf(i * 31.4159f) * 43758.5453f); fracSize -= floorf(fracSize);
        float fracSpeed = fabsf(sinf(i * 93.1230f) * 43758.5453f); fracSpeed -= floorf(fracSpeed);
        float fracOffset = fabsf(sinf(i * 17.5670f) * 43758.5453f); fracOffset -= floorf(fracOffset);

        float starX = startX + fracX * areaW;
        float starY = startY + fracY * areaH; 
        
        float size = (0.5f + fracSize * 1.0f) * scale;
        float speed = 1.0f + fracSpeed * 3.0f;
        float offset = fracOffset * PI * 2.0f;
        
        float twinkle = 0.5f + 0.5f * sinf(previewTime * speed + offset);
        Color starCol = isWire ? GREEN : (Color){255, 255, 255, (unsigned char)(255.0f * twinkle)};
        
        if (isWire) {
            Midcircle((int)starX, (int)starY, (int)size, GREEN);
        } else {
            FillCircle_Fast((int)starX, (int)starY, (int)size, starCol);
        }
    }
    #undef IN_BOX
}

/**
 * @brief Menggambar efek kabut tipis di bagian horizon dengan gradasi transparan.
 *
 * Kabut digambar sebagai persegi panjang dengan gradasi vertikal dari
 * transparan (atas) ke setengah buram (bawah), menciptakan kesan jarak.
 */
void DrawKabut_Pola(float x, float y, float scale, bool isWire) {
    float fogW = 1400.0f;
    float fogH = 400.0f; // Tinggi area kabut
    float fogX = x - fogW / 2.0f;
    float fogY = y - 100.0f;

    if (!isWire) {
        // Pemetaan matriks warna gradien
        Color fogTop = (Color){210, 225, 240, 0};
        Color fogBot = (Color){210, 225, 240, 150}; 
        DrawRectangleGradientV((int)fogX, (int)fogY, (int)fogW, (int)fogH, fogTop, fogBot);
    } else {
        DrawRectangleLines((int)fogX, (int)fogY, (int)fogW, (int)fogH, GREEN);
        DrawTextEx(polaFont, "AREA KABUT ATMOSFER (GRADIENT)", (Vector2){fogX + 20, fogY + 20}, 24.0f, 1.5f, GREEN);
    }
}

/**
 * @brief Menggambar satu lapisan ombak laut dengan buih di puncaknya.
 *
 * Bentuk ombak dihitung dari gabungan dua fungsi sinus dengan frekuensi berbeda
 * agar pola gelombang tidak terlihat berulang dan tampak lebih alami.
 * Buih muncul di titik-titik lembah gelombang dan digambar sebagai elips kecil.
 */
void DrawOmbak_Lapisan(float startX, float endX, float bottomY, float seaBaseY, float amplitude, float frequency, float baseSpeed, Color colTop, Color colBot, float offset, bool hasFoam, float pWave, float daylight, bool isWire) {
    
    int step = 20;
    Vector2 pts[200];
    int pCount = 0;
    
    // Hitung titik-titik permukaan ombak dari kiri ke kanan
    for (float px = startX; px <= endX + step; px += step) {
        float actualX = (px > endX) ? endX : px; // Batasi di tepi kanan
        float wave1 = amplitude * sinf(actualX * frequency + pWave * baseSpeed);
        float wave2 = (amplitude * 0.3f) * sinf(actualX * frequency * 2.1f + pWave * baseSpeed * 1.4f);
        float py = seaBaseY + offset + wave1 + wave2;

        if (pCount < 200) pts[pCount++] = (Vector2){actualX, py};
        if (actualX == endX) break;
    }

    // Batas bawah pengisian — sedikit di bawah lembah ombak terdalam
    float maxDepth = seaBaseY + offset + (amplitude * 1.5f);
    
    if (!isWire) {
        // Isi area di bawah permukaan ombak dengan warna laut
        for (int i = 0; i < pCount - 1; i++) {
            Vector2 tl = pts[i];
            Vector2 tr = pts[i+1];
            Vector2 bl = {tl.x, maxDepth};
            Vector2 br = {tr.x, maxDepth};

            FillTri_Fast(tl, bl, br, colTop);
            FillTri_Fast(tl, br, tr, colTop);
        }
        // Isi bagian bawah layar dengan gradasi dari warna atas ke bawah
        if (bottomY > maxDepth) {
            DrawRectangleGradientV((int)startX, (int)maxDepth, (int)(endX - startX), (int)(bottomY - maxDepth), colTop, colBot);
        }
    } else {
        for (int i = 0; i < pCount - 1; i++) {
            AlgoLine((int)pts[i].x, (int)pts[i].y, (int)pts[i+1].x, (int)pts[i+1].y, GREEN);
        }
        AlgoLine((int)pts[pCount-1].x, (int)pts[pCount-1].y, (int)endX, (int)bottomY, GREEN);
        AlgoLine((int)endX, (int)bottomY, (int)startX, (int)bottomY, GREEN);
        AlgoLine((int)startX, (int)bottomY, (int)pts[0].x, (int)pts[0].y, GREEN);
    }

    // Gambar buih di lembah ombak (berbentuk elips kecil berwarna putih)
    if (hasFoam) {
        Color foamCol = isWire ? GREEN : (Color){255, 255, 255, (unsigned char)((0.15f + daylight * 0.3f) * 255.0f)};
        for (float px = startX; px <= endX; px += 12.0f) {
            float sinVal = sinf(px * frequency + pWave * baseSpeed);
            if (sinVal < -0.2f) {
                float intensity = fabsf(sinVal + 0.2f) / 0.8f;
                float wave1 = amplitude * sinf(px * frequency + pWave * baseSpeed);
                float wave2 = (amplitude * 0.3f) * sinf(px * frequency * 2.1f + pWave * baseSpeed * 1.4f);
                float py = seaBaseY + offset + wave1 + wave2;
                
                float cx1 = px + 5.0f;           float cy1 = py + 2.0f + sinf(px * 0.2f) * 2.0f;
                float rx1 = 6.0f * intensity;    float ry1 = 2.0f * intensity;
                
                // Lewati buih yang berada di luar batas area ombak
                if (cx1 + rx1 > endX || cx1 - rx1 < startX) continue;

                if (!isWire) DrawEllipse((int)cx1, (int)cy1, rx1, ry1, foamCol);
                else {
                    MidpointEllipse((int)cx1, (int)cy1, (int)rx1, (int)ry1, GREEN);
                }
                
                if (intensity > 0.5f) {
                    float cx2 = px - 2.0f;           float cy2 = py + 4.0f + cosf(px * 0.3f) * 2.0f;
                    float rx2 = 4.0f * intensity;    float ry2 = 1.5f * intensity;
                    
                    if (cx2 + rx2 > endX || cx2 - rx2 < startX) continue;

                    if (!isWire) DrawEllipse((int)cx2, (int)cy2, rx2, ry2, foamCol);
                    else {
                        MidpointEllipse((int)cx2, (int)cy2, (int)rx2, (int)ry2, GREEN);
                    }
                }
            }
        }
    }
}

/**
 * @brief Menggambar ombak laut tiga lapis untuk layar showcase Pola 2D.
 *
 * Setiap lapisan memiliki amplitudo, frekuensi, dan kecepatan berbeda
 * untuk menciptakan efek kedalaman dan gerak ombak yang terlihat alami.
 */
void DrawOmbak_Pola(float x, float y, float w, float h, bool isWire) {
    float startX = x - w/2.0f;
    float endX = x + w/2.0f;
    float bottomY = y + h/2.0f;
    float pWave = previewTime;
    float daylight = 1.0f; // Kecerahan penuh untuk showcase

    // Tiga lapis ombak: belakang (jauh), tengah, dan depan (dekat)
    DrawOmbak_Lapisan(startX, endX, bottomY, y, 15.0f, 0.005f, 1.5f, (Color){30, 100, 180, 255}, (Color){15, 50, 90, 255}, -40.0f, true, pWave, daylight, isWire);
    DrawOmbak_Lapisan(startX, endX, bottomY, y, 20.0f, 0.008f, 2.0f, (Color){45, 130, 220, 255}, (Color){22, 65, 110, 255}, -10.0f, true, pWave, daylight, isWire);
    DrawOmbak_Lapisan(startX, endX, bottomY, y, 25.0f, 0.010f, 2.5f, (Color){60, 160, 255, 255}, (Color){30, 80, 127, 255}, 30.0f, true, pWave, daylight, isWire);
}

/**
 * @brief Menggambar cuaca hujan deras atau badai petir.
 *
 * Saat isStorm=true, kilatan petir bercabang dan efek cahaya layar akan ditampilkan
 * bersama butir hujan yang jauh lebih padat dan cepat.
 *
 * @param isStorm Jika true, tampilkan badai petir; jika false, hanya hujan biasa.
 */
void DrawCuaca_HujanBadai(float x, float y, float w, float h, bool isStorm, bool isWire) {
    // Batas area hujan dan petir
    float startX = x - w/2.0f;
    float startY = y - h/2.0f;
    float endX   = x + w/2.0f;
    float endY   = y + h/2.0f;

    // Gambar petir hanya saat mode badai aktif
    if (isStorm) {
        float cycle = fmodf(previewTime, 3.0f);
        float life = 1.0f - (cycle * 2.5f); 

        if (life > 0.0f) {
            // Efek cahaya kilat — layar menjadi terang sesaat
            if (!isWire) {
                DrawRectangle((int)startX, (int)startY, (int)w, (int)h, (Color){255, 255, 255, (unsigned char)(life * 150.0f)});
            }

            // Buat jalur petir zig-zag dengan seed berdasarkan waktu
            int   seed = (int)(previewTime / 3.0f);
            float lx   = startX + fabsf(sinf(seed * 12.34f)) * w;
            float cx   = lx, cy = startY;
            
            Vector2 pts[20]; int pIdx = 0;
            pts[pIdx++] = (Vector2){cx, cy};

            while (cy < startY + h * 0.7f && pIdx < 18) {
                float nx = cx + (sinf(seed * pIdx * 45.6f) * 150.0f - 75.0f);
                float ny = cy + 30.0f + fabsf(sinf(seed * pIdx * 12.3f) * 50.0f);
                
                // Batasi petir agar tidak keluar dari area yang ditentukan
                if (nx < startX) nx = startX;
                if (nx > endX)   nx = endX;
                if (ny > endY) ny = endY;

                pts[pIdx++] = (Vector2){nx, ny};
                cx = nx; cy = ny;
            }

            Color boltCol = (Color){255, 255, 255, (unsigned char)(life * 255.0f)};
            Color glowCol = (Color){255, 255, 255, (unsigned char)(life * 60.0f)};

            for (int i = 0; i < pIdx - 1; i++) {
                if (!isWire) {
                    DrawLineEx(pts[i], pts[i+1], 15.0f * life, glowCol); 
                    DrawLineEx(pts[i], pts[i+1], 4.0f * life, boltCol);  
                } else AlgoLine((int)pts[i].x, (int)pts[i].y, (int)pts[i+1].x, (int)pts[i+1].y, GREEN);
            }

            // Buat cabang petir kecil dari titik tengah jalur utama
            if (pIdx > 4) {
                int splitIdx = pIdx / 2;
                cx = pts[splitIdx].x; cy = pts[splitIdx].y;
                Vector2 bPts[5]; int bIdx = 0;
                bPts[bIdx++] = (Vector2){cx, cy};
                for(int i=0; i<3; i++) {
                    float nx = cx + (sinf(seed * i * 89.1f) * 100.0f - 50.0f);
                    float ny = cy + 40.0f;
                    
                    // Batasi cabang agar tidak keluar dari area
                    if (nx < startX) nx = startX;
                    if (nx > endX)   nx = endX;
                    
                    bPts[bIdx++] = (Vector2){nx, ny};
                    cx = nx; cy = ny;
                }
                for (int i = 0; i < bIdx - 1; i++) {
                    if (!isWire) DrawLineEx(bPts[i], bPts[i+1], 2.0f * life, boltCol);
                    else AlgoLine((int)bPts[i].x, (int)bPts[i].y, (int)bPts[i+1].x, (int)bPts[i+1].y, GREEN);
                }
            }
        }
    }

    // Gambar rintik hujan — saat badai lebih padat dan lebih cepat
    int   dropCount     = isStorm ? 800 : 260;
    float windSpeed     = isStorm ? 5.0f : 1.0f;
    float baseFallSpeed = isStorm ? 1800.0f : 1000.0f;
    
    Color dropCol = isStorm ? (Color){255, 255, 255, 100} : (Color){255, 255, 255, 50};
    if (isWire) dropCol = GREEN;
    float strokeThick = (isStorm && !isWire) ? 2.0f : 1.0f;

    for (int i = 0; i < dropCount; i++) {
        float r1 = fabsf(sinf(i * 12.9898f) * 43758.5453f); r1 -= floorf(r1);
        float r2 = fabsf(sinf(i * 78.233f) * 43758.5453f);  r2 -= floorf(r2);
        float r3 = fabsf(sinf(i * 31.415f) * 43758.5453f);  r3 -= floorf(r3);

        float length = 10.0f + (r3 * 20.0f);
        float speed = baseFallSpeed + (r1 * 800.0f);

        float currentY = startY + fmodf(r2 * h + previewTime * speed, h);
        float currentX = startX + fmodf(r1 * w + previewTime * windSpeed * 300.0f, w);

        Vector2 bot = {currentX, currentY};
        Vector2 top = {currentX - (windSpeed * length * 0.1f), currentY - length};

        // Pangkas rintik hujan agar tidak keluar dari batas atas area
        if (top.y < startY) {
            float diffY = startY - top.y;
            top.y = startY;
            top.x += diffY * (windSpeed * 0.1f);
        }
        if (top.x < startX) {
            float diffX = startX - top.x;
            top.x = startX;
            top.y += diffX / (windSpeed * 0.1f);
        }

        if (!isWire) {
            DrawLineEx(top, bot, strokeThick, dropCol);
        } else {
            AlgoLine((int)top.x, (int)top.y, (int)bot.x, (int)bot.y, GREEN);
        }
    }
}

/**
 * @brief Menggambar rasi bintang kustom dengan garis putus-putus penghubung.
 *
 * Setiap bintang digambar sebagai lingkaran kecil bercahaya, lalu dihubungkan
 * satu per satu dengan garis putus-putus. Di mode wireframe, garis putus-putus
 * digambar menggunakan algoritma DDA kustom (DDA_DashedLine).
 *
 * @param stars      Array koordinat bintang di layar.
 * @param starCount  Jumlah bintang dalam array.
 * @param nightlight Kecerahan berdasarkan waktu malam (0.0 = siang, 1.0 = malam).
 * @param isWire     Jika true, hanya garis tepi yang digambar.
 */
void DrawRasiBintang_Animasi(Vector2 *stars, int starCount, float nightlight, bool isWire) {
    if (starCount <= 0 || nightlight <= 0.0f) return;

    Color cLine = isWire ? GREEN : (Color){200, 230, 255, (unsigned char)(204.0f * nightlight)}; 
    Color cLineGlow = isWire ? BLANK : (Color){200, 230, 255, (unsigned char)(60.0f * nightlight)}; 
    
    Color cStar = isWire ? GREEN : (Color){255, 255, 255, (unsigned char)(255.0f * nightlight)};
    Color cStarGlow = isWire ? BLANK : (Color){255, 255, 255, (unsigned char)(80.0f * nightlight)};

    // Gambar garis putus-putus penghubung antar bintang
    if (starCount > 1) {
        for (int i = 0; i < starCount - 1; i++) {
            Vector2 p1 = stars[i];
            Vector2 p2 = stars[i+1];
            
            if (!isWire) {
                // Gambar cahaya samar di belakang garis
                DrawLineEx(p1, p2, 6.0f, cLineGlow);

                // Gambar garis putus-putus di atasnya
                float dist = Vector2Distance(p1, p2);
                float dashLen = 5.0f;
                float gapLen = 5.0f;
                Vector2 dir = { (p2.x - p1.x) / dist, (p2.y - p1.y) / dist };
                
                float currentDist = 0.0f;
                bool drawDash = true;
                while (currentDist < dist) {
                    float step = drawDash ? dashLen : gapLen;
                    if (currentDist + step > dist) step = dist - currentDist;
                    
                    if (drawDash) {
                        Vector2 start = { p1.x + dir.x * currentDist, p1.y + dir.y * currentDist };
                        Vector2 end = { p1.x + dir.x * (currentDist + step), p1.y + dir.y * (currentDist + step) };
                        DrawLineEx(start, end, 1.5f, cLine);
                    }
                    currentDist += step;
                    drawDash = !drawDash;
                }
            } else {
                DDA_DashedLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, 5, 5, GREEN);
            }
        }
    }

    // Gambar titik cahaya di setiap posisi bintang
    for (int i = 0; i < starCount; i++) {
        if (!isWire) {
            FillCircle_Fast((int)stars[i].x, (int)stars[i].y, 8, cStarGlow);
            FillCircle_Fast((int)stars[i].x, (int)stars[i].y, 2, cStar);
        } else {
            Midcircle((int)stars[i].x, (int)stars[i].y, 2, GREEN);
        }
    }
}
/**
 * @brief Menggambar entitas lumba-lumba yang dapat melompat.
 *
 * Struktur tubuh dirakit dari beberapa himpunan poligon cembung guna mengatasi keterbatasan
 * rendering pada bentuk cekung (concave). Susunan ini memastikan proses culling warna bekerja mulus.
 * 
 * @param x         Koordinat kartesian X posisi badan.
 * @param y         Koordinat kartesian Y posisi dasar sebelum di offset oleh fase.
 * @param scale     Skala resolusi dimensi tubuh.
 * @param jumpPhase Fase putaran (sudut) saat ini berdasar kalkulator sinus.
 * @param c         Warna blok material utama lumba-lumba.
 * @param isWire    Apakah akan dirender dengan bingkai rangka hijau atau isi solid.
 */
void DrawLumbaLumba_Pola(float x, float y, float scale, float jumpPhase, Color c, bool isWire) {
    // Sudut translasi lumba-lumba saat menukik/menanjak
    float angle = cosf(jumpPhase) * -0.6f; 

    // Poligon: Tubuh Utama (Cembung aerodinamis)
    Vector2 bodyPts[] = {
        {30, 0}, {22, -8}, {10, -12}, {-5, -14}, {-20, -12}, {-35, -8}, {-48, -4},
        {-48, 2}, {-35, 6}, {-20, 10}, {-5, 12}, {10, 10}, {20, 6}, {28, 2}
    };
    
    // Poligon: Moncong (Lurus meruncing ke depan)
    Vector2 beakPts[] = {
        {30, -1}, {46, 2}, {28, 3}
    };
    
    // Poligon: Sirip Punggung (Melengkung asimetris ke belakangan)
    Vector2 dorsalPts[] = {
        {-2, -13}, {-18, -26}, {-16, -11}
    };

    // Poligon: Sirip Dada (Memiliki offset pada sisi pinggul)
    Vector2 pectoralPts[] = {
        {8, 8}, {-4, 24}, {0, 10}
    };

    // Poligon: Sirip Ekor Atas
    Vector2 tailUp[] = {
        {-46, -4}, {-62, -12}, {-52, 0}
    };
    
    // Poligon: Sirip Ekor Bawah
    Vector2 tailDn[] = {
        {-46, 2}, {-52, 0}, {-62, 10}
    };

    Color outline = isWire ? GREEN : BLANK;

    // Perenderan subkumpulan organ badan. Bagian Dorsal & Ekor berada pada z-index sejajar punggung belakang.
    DrawCustomPoly(dorsalPts, 3, x, y, scale, angle, c, outline, isWire);
    DrawCustomPoly(tailUp, 3, x, y, scale, angle, c, outline, isWire);
    DrawCustomPoly(tailDn, 3, x, y, scale, angle, c, outline, isWire);
    DrawCustomPoly(beakPts, 3, x, y, scale, angle, c, outline, isWire);
    
    // Menggambar tulang punggung utama tubuh
    DrawCustomPoly(bodyPts, 14, x, y, scale, angle, c, outline, isWire);
    
    // Sirip Dada menjorok pada tampak samping depan
    DrawCustomPoly(pectoralPts, 3, x, y, scale, angle, c, outline, isWire);

    // Penggambaran komponen optikal (Mata) khusus layar solid
    if (!isWire) {
        float eyeOffsetX = 24.0f;
        float eyeOffsetY = -2.0f;
        
        // Mempertahankan rotasi absolut koordinat mata relatif terhadap titik poros tubuh
        float eyeX = x + (eyeOffsetX * cosf(angle) - eyeOffsetY * sinf(angle)) * scale;
        float eyeY = y + (eyeOffsetX * sinf(angle) + eyeOffsetY * cosf(angle)) * scale;
        
        DrawCircle((int)eyeX, (int)eyeY, 2.0f * scale, (Color){20, 20, 30, 255});
    }
}

/** 
 * @brief Menggambar cuaca kabut tebal yang menutupi layar. 
 * 
 * ==========================================================================================
 * DESKRIPSI: Fitur environment cuaca tambahan berupa efek kabut tebal ("tambahkan cuaca berkabut").
 * ==========================================================================================
 */
void DrawCuaca_KabutTebal(float x, float y, float w, float h, bool isWire) {
    if (!isWire) {
        Color topFog = (Color){220, 230, 235, 80}; 
        Color botFog = (Color){190, 200, 210, 140};
        DrawRectangleGradientV((int)(x - w/2.0f), (int)(y - h/2.0f), (int)w, (int)h, topFog, botFog);
    }
}
