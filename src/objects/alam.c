// =============================================================================
//  alam.c — Objek Alam & Cuaca
//  Proyek   : Mercusuar (Komputer Grafis – Semester 4)
//  Deskripsi: Kumpulan fungsi penggambaran elemen lingkungan:
//             Awan, Burung, Gunung, Tebing, Matahari/Bulan, Bintang,
//             Kabut, Ombak, Hujan/Badai, Rasi Bintang.
//  Catatan  : Semua fungsi mendukung mode Solid (isWire=false) dan
//             mode Wireframe (isWire=true, warna selalu GREEN).
// =============================================================================

#include "alam.h"
#include "../ui/draw_helpers.h"
#include "../algo/dda.h"
#include "../algo/midcircle.h"
#include <math.h>
#include "raymath.h"

// --- Variabel Global (Diambil dari pola2d.c via extern) ----------------------
extern Font  polaFont;    // Font untuk teks debug (mode Wireframe)
extern float previewTime; // Waktu animasi global (detik)

/**
 * @brief Merender formasi awan prosedural menggunakan interpolasi polynomial derajat tiga (Cubic Bezier Curve).
 * 
 * Melakukan kalkulasi parametrik atas 4 titik kontrol (P0 sebagai jangkar awal, P1/P2 pelengkung, P3 jangkar akhir).
 * Algoritma Bezier diaplikasikan secara berurutan untuk membentuk struktur kumulus berlapis.
 * Fungsi ini dibuat untuk meniru cara kerja fungsi bezierCurveTo.
 * 
 * @param type Identifikasi tipe model geometri awan (0 hingga 3).
 * @param x Posisi absolut X titik awal.
 * @param y Posisi absolut Y titik awal.
 * @param scale Faktor penskalaan matriks spasial.
 * @param c Nilai warna solid untuk mode perenderan raster padat (Non-Wireframe).
 * @param isWire Status pengaktifan kerangka geometri hijau.
 */
void DrawAwan_Pola(int type, float x, float y, float scale, Color c, bool isWire) {
    Color outLine = isWire ? GREEN : BLANK;

    // Makro Evaluasi Polinomial Bezier Kubik (Cubic Bezier Evaluation Macro)
    #define ADD_CUBIC(arr, idx, px0, py0, px1, py1, px2, py2, px3, py3) \
        for(int _i=1; _i<=12; _i++) { \
            float _t = _i/12.0f; float _inv = 1.0f - _t; \
            float _x = _inv*_inv*_inv*(px0) + 3.0f*_inv*_inv*_t*(px1) + 3.0f*_inv*_t*_t*(px2) + _t*_t*_t*(px3); \
            float _y = _inv*_inv*_inv*(py0) + 3.0f*_inv*_inv*_t*(py1) + 3.0f*_inv*_t*_t*(py2) + _t*_t*_t*(py3); \
            arr[idx++] = (Vector2){_x, _y}; \
        }

    // Penyesuaian faktor skala matriks batas ruang pandang (Viewport Context)
    float s = scale * 0.4f; 

    // Pengelompokan matriks awan parsial untuk formasi kumulus pusat
    float offsetX[4] = {-200.0f, 180.0f, -150.0f, 150.0f};
    float offsetY[4] = {-120.0f, -150.0f, 100.0f, 80.0f};

    for (int cloudType = 0; cloudType < 4; cloudType++) {
        Vector2 pts[50]; int pIdx = 0;
        
        // Titik jangkar asal (Origin Anchor Point)
        pts[pIdx++] = (Vector2){0, 0}; 

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

        // Draw menggunakan helper
        DrawCustomPoly(pts, pIdx, x + (offsetX[cloudType] * s), y + (offsetY[cloudType] * s), s, 0, c, outLine, isWire);
    }
    
    #undef ADD_CUBIC
}

/**
 * @brief Menganimasikan bentuk burung terbang menggunakan model kinematika mengepak parsial (Flapping Wing Kinematics).
 * 
 * Sayap direpresentasikan oleh dua sub-kurva Bezier simetris yang parameter ujung sayapnya
 * didorong secara dinamis menggunakan nilai osilasi dari sebuah fungsi trigonometri eksternal (flapPhase).
 * 
 * @param flapPhase Faktor siklus gelombang pergerakan sayap, dengan interpolasi sinusoida (0.0 hingga 1.0).
 */
void DrawBurung_Pola(float x, float y, float scale, float flapPhase, Color c, bool isWire) {
    Color outLine = isWire ? GREEN : c;
    
    // Penegasan radius batas solid (Solid Stroke Radius Margin)
    float th = isWire ? 0.0f : 1.5f; 
    
    for(float off = -th; off <= th; off += 1.0f) {
        // Sayap Kiri
        DrawCustomBezier((Vector2){-15.0f, -12.0f * flapPhase + off}, (Vector2){-7.0f, 0.0f + off}, (Vector2){0.0f, 0.0f + off}, x, y, scale, 0.0f, BLANK, outLine, isWire);
        // Sayap Kanan
        DrawCustomBezier((Vector2){0.0f, 0.0f + off}, (Vector2){7.0f, 0.0f + off}, (Vector2){15.0f, -12.0f * flapPhase + off}, x, y, scale, 0.0f, BLANK, outLine, isWire);
        
        if (isWire) break; // Wireframe cukup 1 lapis garis (hijau)
    }
}

/**
 * @brief Merender lansekap pegunungan poligon rendah (Low-Poly) dengan stratifikasi kedalaman 3 lapisan.
 * 
 * Untuk mensimulasikan efek kedalaman atmosferik (Aerial Perspective), teknik peneduhan faset (Facet Shading)
 * diaplikasikan pada setiap lapisan (Jauh, Tengah, Dekat). Saturasi warna meredup sebanding dengan jarak z-index.
 * Pembentukan bentuk geometri dieksekusi dengan matriks koordinat proporsional (MX dan MY) relatif terhadap bounding box.
 */
void DrawGunung_Pola(float x, float y, float w, float h, Color baseColor, bool isWire) {
    int bR = 224, bG = 246, bB = 255; 
    Color outLine = isWire ? GREEN : BLANK;

    float leftX = x - w / 2.0f;
    float topY  = y - h / 2.0f;
    float botY  = y + h / 2.0f;

    // Macro Skala Presisi
    #define MX(perc) (leftX + (w * (perc)))
    #define MY(perc) (topY + (h * (perc)))

    // =========================================================
    // 1. FAR LAYER (Gunung Belakang)
    // =========================================================
    Color fCol = { (unsigned char)(bR*0.8f),  (unsigned char)(bG*0.8f),  (unsigned char)(bB*0.9f), 255 };
    Color fDrk = { (unsigned char)(bR*0.7f),  (unsigned char)(bG*0.7f),  (unsigned char)(bB*0.8f), 255 };
    Color fLgt = { (unsigned char)(bR*0.85f), (unsigned char)(bG*0.85f), (unsigned char)(bB*0.95f), 255 };

    Vector2 fBase[] = {
        {MX(0.50f), botY}, 
        {MX(1.00f), botY}, {MX(1.00f), MY(0.60f)}, {MX(0.80f), MY(0.42f)}, 
        {MX(0.50f), MY(0.55f)}, {MX(0.25f), MY(0.45f)}, {MX(0.00f), MY(0.60f)}, {MX(0.00f), botY} 
    };
    DrawCustomPoly(fBase, 8, 0, 0, 1.0f, 0.0f, fCol, outLine, isWire);

    // Poligon Facet Puncak Sisi Kiri (Terpapar Cahaya)
    Vector2 fP1_L[] = { {MX(0.00f), botY}, {MX(0.00f), MY(0.60f)}, {MX(0.25f), MY(0.45f)}, {MX(0.22f), MY(0.52f)}, {MX(0.24f), MY(0.58f)}, {MX(0.0f), botY} };
    DrawCustomPoly(fP1_L, 6, 0, 0, 1.0f, 0.0f, fLgt, outLine, isWire);

    Vector2 fP1_D[] = { {MX(0.50f), botY}, {MX(0.50f), MY(0.55f)}, {MX(0.25f), MY(0.45f)}, {MX(0.28f), MY(0.52f)}, {MX(0.26f), MY(0.58f)}, {MX(0.60f), botY} };
    DrawCustomPoly(fP1_D, 6, 0, 0, 1.0f, 0.0f, fDrk, outLine, isWire);

    Vector2 fP2_L[] = { {MX(0.50f), botY}, {MX(0.79f), MY(0.56f)}, {MX(0.77f), MY(0.50f)}, {MX(0.80f), MY(0.42f)}, {MX(0.50f), MY(0.55f)} };
    DrawCustomPoly(fP2_L, 5, 0, 0, 1.0f, 0.0f, fLgt, outLine, isWire);

    Vector2 fP2_D[] = { {MX(1.00f), botY}, {MX(0.81f), MY(0.56f)}, {MX(0.83f), MY(0.50f)}, {MX(0.80f), MY(0.42f)}, {MX(1.00f), MY(0.60f)} };
    DrawCustomPoly(fP2_D, 5, 0, 0, 1.0f, 0.0f, fDrk, outLine, isWire);

    // =========================================================
    // 2. MID LAYER (Gunung Tengah)
    // =========================================================
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

    // =========================================================
    // 3. NEAR LAYER (Gunung Depan Clean Low-Poly)
    // =========================================================
    Color nCol = { (unsigned char)(bR*0.4f),  (unsigned char)(bG*0.4f),  (unsigned char)(bB*0.5f), 255 };
    Color nDrk = { (unsigned char)(bR*0.3f),  (unsigned char)(bG*0.3f),  (unsigned char)(bB*0.4f), 255 };
    Color nLgt = { (unsigned char)(bR*0.45f), (unsigned char)(bG*0.45f), (unsigned char)(bB*0.55f), 255 };

    Vector2 nBase[] = {
        {MX(0.20f), botY},
        {MX(0.75f), botY}, {MX(0.75f), MY(0.65f)}, {MX(0.45f), MY(0.52f)}, 
        {MX(0.20f), MY(0.65f)}, {MX(0.05f), MY(0.55f)}, {MX(0.00f), MY(0.60f)}, {MX(0.00f), botY}
    };
    DrawCustomPoly(nBase, 8, 0, 0, 1.0f, 0.0f, nCol, outLine, isWire);

    // --- PUNCAK 1 (KIRI) ---
    Vector2 nP1_L[] = { {MX(0.00f), botY}, {MX(0.05f), botY}, {MX(0.05f), MY(0.55f)}, {MX(0.00f), MY(0.60f)} };
    DrawCustomPoly(nP1_L, 4, 0, 0, 1.0f, 0.0f, nLgt, outLine, isWire);
    
    Vector2 nP1_D[] = { {MX(0.05f), botY}, {MX(0.20f), botY}, {MX(0.20f), MY(0.65f)}, {MX(0.05f), MY(0.55f)} };
    DrawCustomPoly(nP1_D, 4, 0, 0, 1.0f, 0.0f, nDrk, outLine, isWire);

    // --- PUNCAK 2 (TENGAH) ---
    Vector2 nP2_L[] = { {MX(0.20f), botY}, {MX(0.30f), botY}, {MX(0.45f), MY(0.52f)}, {MX(0.20f), MY(0.65f)} };
    DrawCustomPoly(nP2_L, 4, 0, 0, 1.0f, 0.0f, nLgt, outLine, isWire);
    
    Vector2 nP2_D[] = { {MX(0.30f), botY}, {MX(0.75f), botY}, {MX(0.75f), MY(0.65f)}, {MX(0.45f), MY(0.52f)} };
    DrawCustomPoly(nP2_D, 4, 0, 0, 1.0f, 0.0f, nDrk, outLine, isWire);
    
    #undef MX
    #undef MY
}

/**
 * @brief Menggambar pemandangan tebing karang beserta rumah penjaga dan rumput.
 * 
 * Melibatkan pemecahan bidang-bidang trapesium tak beraturan untuk struktur bebatuan asimetris.
 * Permukaan atas tebing dilapisi vegetasi rumput yang dilengkungkan mengikuti interpolasi titik-titik
 * kontrol kurva kuadratik berseri (Quadratic Bezier Curve) untuk memberikan topologi tebing yang melandai.
 * 
 * @param w Skalar rasio perbesaran untuk ukuran tebing keseluruhan (bukan mewakili lebar linier).
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
    
    // Poligon Facet Patahan Sudut Curam
    Vector2 p8[] = {{70, 150}, {110, 15}, {140, 90}, {rX, 40}, {rX, botY}, {150, botY}}; DrawCustomPoly(p8, 6, x, y, s, 0, cR4, o4, isWire);
    Vector2 p9[] = {{180, 15}, {rX, 15}, {rX, 40}}; DrawCustomPoly(p9, 3, x, y, s, 0, cR5, o5, isWire);

    // Makro Evaluasi Polinomial Bezier Kuadratik (Quadratic Bezier Evaluation Macro)
    #define ADD_Q(arr, idx, px0, py0, px1, py1, px2, py2) \
        for(int _i=1; _i<=10; _i++) { float _t = _i/10.0f; float _in = 1.0f - _t; arr[idx++] = (Vector2){_in*_in*(px0) + 2.0f*_in*_t*(px1) + _t*_t*(px2), _in*_in*(py0) + 2.0f*_in*_t*(py1) + _t*_t*(py2)}; }

    // 2. Hamparan Rumput Melengkung (Dark)
    Color cG1 = (Color){45, 70, 35, 255};
    Vector2 g1[100]; int g1Idx = 0;
    g1[g1Idx++] = (Vector2){-145, 0}; g1[g1Idx++] = (Vector2){-145, 25};
    ADD_Q(g1, g1Idx, -145, 25, -130, 40, -110, 25); ADD_Q(g1, g1Idx, -110, 25, -90, 45, -60, 28);
    ADD_Q(g1, g1Idx, -60, 28, -30, 45, 0, 30);      ADD_Q(g1, g1Idx, 0, 30, 40, 40, 80, 25);
    ADD_Q(g1, g1Idx, 80, 25, 110, 35, 140, 22);     ADD_Q(g1, g1Idx, 140, 22, 170, 32, 200, 20);
    // Batas akhir area rumput tebing
    ADD_Q(g1, g1Idx, 200, 20, 205, 24, rX, 18);
    g1[g1Idx++] = (Vector2){rX, 18}; g1[g1Idx++] = (Vector2){rX, 0};
    
    // 3. Hamparan Rumput Highlight (Light)
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
        for(int i=0; i<g1Idx-1; i++) { Vector2 p1 = TransformPola(g1[i].x, g1[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g1[i+1].x, g1[i+1].y, x, y, s, 0); DrawTriangle(gCenter, p1, p2, cG1); DrawTriangle(gCenter, p2, p1, cG1); }
        for(int i=0; i<g2Idx-1; i++) { Vector2 p1 = TransformPola(g2[i].x, g2[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g2[i+1].x, g2[i+1].y, x, y, s, 0); DrawTriangle(gCenter, p1, p2, cG2); DrawTriangle(gCenter, p2, p1, cG2); }
    } else {
        for(int i=0; i<g1Idx-1; i++) { Vector2 p1 = TransformPola(g1[i].x, g1[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g1[i+1].x, g1[i+1].y, x, y, s, 0); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
        for(int i=0; i<g2Idx-1; i++) { Vector2 p1 = TransformPola(g2[i].x, g2[i].y, x, y, s, 0); Vector2 p2 = TransformPola(g2[i+1].x, g2[i+1].y, x, y, s, 0); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
        
        // Penutupan lintasan atas poligon batas siluet padang rumput (Top Surface Wireframe Closure)
        Vector2 g1First = TransformPola(g1[0].x, g1[0].y, x, y, s, 0);
        Vector2 g1Last = TransformPola(g1[g1Idx-1].x, g1[g1Idx-1].y, x, y, s, 0);
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
 * @brief Memproyeksikan cakram Matahari bersinar secara interaktif dipengaruhi oleh parameter siang/malam.
 * 
 * Untuk memancarkan ilusi cahaya yang berpendar (glow effect), deretan lingkaran semi-transparan (alpha-blended) 
 * berwarna gradasi digambar berlapis di belakang matahari utama.
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
 * @brief Memproyeksikan fase Bulan purnama disertai kawah diskrit secara interaktif berdasarkan parameter malam.
 * 
 * Seperti matahari, Bulan memuat algoritma cincin pendar (*glow ring*) progesif. 
 * Topologi kawah dibentuk oleh susunan tiga elips primitif bernuansa relatif gelap yang di-[render] secara asimetris.
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
 * @brief Antarmuka pembungkus (Wrapper Interface) penempatan Matahari dan Bulan dalam mode presentasi statis (Pola 2D).
 * 
 * Fungsi ini bertugas menegasikan komponen trigonometri sistem orbital pusat ke titik jangkar X arbitrer.
 */
void DrawBulanMatahari_Pola(float x, float y, float scale, bool isWire) {
    // Geser titik tengah dunia ke kiri (-120) untuk Matahari dan kanan (+120) untuk Bulan
    float sunWorldX = x - (120.0f * scale);
    float moonWorldX = x + (120.0f * scale);

    DrawMatahari_Animasi(sunWorldX, y, scale, 1.0f, isWire);
    DrawBulan_Animasi(moonWorldX, y, scale, 1.0f, isWire);
}

/**
 * @brief Menghasilkan hamparan Bintang berkelip menggunakan model Penentuan Acak Sintetis (Pseudo-Random Generation).
 * 
 * Ketimbang melakukan penyediaan memori larik array (memory array allocation), posisi tiap bintang dihitung *on the fly* 
 * mempergunakan fungsi hash trigonometri deterministik (sin-hash) agar posisinya konstan namun terlihat acak.
 * Efek kerlap-kerlip terhubung lurus terhadap pengali `previewTime` yang dimodulasi kecepatan individual partikel.
 */
void DrawBintang_Pola(float x, float y, float scale, Color c, bool isWire) {
    int numStars = 200; 
    float areaW = 1300.0f; 
    float areaH = 650.0f;  

    // Batas layar mutlak (Tidak ada cahaya yang boleh lewat dari kotak ini)
    float startX = x - areaW / 2.0f;
    float startY = y - areaH / 2.0f - 40.0f;

    // Helper Kliping Titik
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
            DrawCircleLinesV((Vector2){starX, starY}, size, GREEN);
        } else {
            DrawCircleV((Vector2){starX, starY}, size, starCol);
        }
    }
    #undef IN_BOX
}

/**
 * @brief Menerapkan filter kabut atmosferik mengandalkan gradasi linier vertikal berformat RGBA.
 * 
 * Menyediakan transisi kehalusan (blend) antara batas horizon laut dengan elemen pegunungan, 
 * secara visual membuat batas tepi terburamkan menyesuaikan nilai Alpha dari atas 0% ke bawah 100%.
 */
void DrawKabut_Pola(float x, float y, float scale, bool isWire) {
    // Konstruksi gradient linier vertikal efek atmosfer
    float fogW = 1400.0f;
    float fogH = 400.0f; // Elevasi ambang asimtotis atmosfer
    float fogX = x - fogW / 2.0f;
    
    // Jarak Offset Y absolut
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
 * @brief Algoritma intisari pengeksekusi pembayangan Dinamika Fluida Ombak Laut (Ocean Wave Fluid Dynamics).
 * 
 * Ombak dihasilkan dengan menderivasi interpolasi Sinus Ganda (Double-Sine Wave Interpolation),
 * di mana gelombang primer beramplitudo tinggi dipadukan (blend) bersama gelombang sekunder berfrekuensi dua kali lipat,
 * manghasilkan puncak gelombang laut yang terinterupsi secara organik tanpa terkesan repetitif.
 * 
 * Algoritma perenderan Solid secara brilian hanya menggambar puncak segi tiga untuk mengisi lereng bukit gelombang,
 * sebelum secara vertikal diguyur dengan pewarnaan gradien (Gradient Fill) ke tingkat titik palung samudera.
 * 
 * Buih laut (foam) terbentuk menggunakan deteksi kemiringan sinus: bila nilai f(x)<0.2, maka partikel elips buih dibaca dan digambar.
 */
void DrawOmbak_Lapisan(float startX, float endX, float bottomY, float seaBaseY, float amplitude, float frequency, float baseSpeed, Color colTop, Color colBot, float offset, bool hasFoam, float pWave, float daylight, bool isWire) {
    
    int step = 20;
    Vector2 pts[200];
    int pCount = 0;
    
    // 1. Pemotongan Matriks Sinusoida Horizontal (Precise Horizontal Clipping)
    for (float px = startX; px <= endX + step; px += step) {
        float actualX = (px > endX) ? endX : px; // Limitasi Clamp
        float wave1 = amplitude * sinf(actualX * frequency + pWave * baseSpeed);
        float wave2 = (amplitude * 0.3f) * sinf(actualX * frequency * 2.1f + pWave * baseSpeed * 1.4f);
        float py = seaBaseY + offset + wave1 + wave2;
        
        if (pCount < 200) pts[pCount++] = (Vector2){actualX, py};
        if (actualX == endX) break; // Keluar dari proses pengulangan
    }
    
    // 2. Deteksi Titik Palung Maksimal (Trough Depth Detection)
    float maxDepth = seaBaseY + offset + (amplitude * 1.5f); 
    
    if (!isWire) {
        // Menggambar bentuk gelombang ombak
        for (int i = 0; i < pCount - 1; i++) {
            Vector2 tl = pts[i]; 
            Vector2 tr = pts[i+1];
            Vector2 bl = {tl.x, maxDepth};  
            Vector2 br = {tr.x, maxDepth};
            
            DrawTriangle(tl, bl, br, colTop);
            DrawTriangle(tl, br, tr, colTop);
        }
        // Pengisian gradasi badan elevasi kedalaman laut (Sea Box Depth Interpolation)
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

    // 3. Proyeksi Partikel Buih Organik (Organic Foam Particles Projection)
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
                
                // Pengecekan agar bagian yang tidak terlihat tidak perlu digambar
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
 * @brief Pengendali susunan antarmuka tumpukan tiga strata tingkat kedalaman Ombak Laut (Parallax Scrolling Array).
 */
void DrawOmbak_Pola(float x, float y, float w, float h, bool isWire) {
    float startX = x - w/2.0f;
    float endX = x + w/2.0f;
    float bottomY = y + h/2.0f;
    float pWave = previewTime;
    float daylight = 1.0f; // Konstanta pencahayaan absolut

    // Lapisan 3 (Belakang), Lapisan 2 (Tengah), Lapisan 1 (Depan)
    DrawOmbak_Lapisan(startX, endX, bottomY, y, 15.0f, 0.005f, 1.5f, (Color){30, 100, 180, 255}, (Color){15, 50, 90, 255}, -40.0f, true, pWave, daylight, isWire);
    DrawOmbak_Lapisan(startX, endX, bottomY, y, 20.0f, 0.008f, 2.0f, (Color){45, 130, 220, 255}, (Color){22, 65, 110, 255}, -10.0f, true, pWave, daylight, isWire);
    DrawOmbak_Lapisan(startX, endX, bottomY, y, 25.0f, 0.010f, 2.5f, (Color){60, 160, 255, 255}, (Color){30, 80, 127, 255}, 30.0f, true, pWave, daylight, isWire);
}

/**
 * @brief Menstimulasikan mesin perender cuaca buruk kondisional dengan memancarkan curah presipitasi berkecepatan tinggi.
 * 
 * Sistem akan beralih menuju fase BADAI apabila parameter `isStorm` bernilai benar (True). 
 * Pada fase BADAI, matriks garis petir (Lightning Strike) akan dieksekusi menggunakan kurva zig-zag
 * dan algoritma pencerabangan acak hirarkis sederhana (Branching Algorithm), diimbangi filter pendar ledakan kilat layar.
 */
void DrawCuaca_HujanBadai(float x, float y, float w, float h, bool isStorm, bool isWire) {
    // Batas Matriks Jendela Kliping Partikel Presipitasi (Precipitation Bounding Box)
    float startX = x - w/2.0f;
    float startY = y - h/2.0f;
    float endX = x + w/2.0f;
    float endY = y + h/2.0f;
    
    // 1. Inisiasi Kilat Atmosfer Lanjut (Advanced Lightning Strike Routine)
    if (isStorm) {
        float cycle = fmodf(previewTime, 3.0f);
        float life = 1.0f - (cycle * 2.5f); 

        if (life > 0.0f) {
            // A. Pendar Filter Layar Utama (Screen Glare Mask)
            if (!isWire) {
                DrawRectangle((int)startX, (int)startY, (int)w, (int)h, (Color){255, 255, 255, (unsigned char)(life * 150.0f)});
            }

            // B. Iterasi Percabangan Linier (Linear Spline Traversal)
            int seed = (int)(previewTime / 3.0f);
            float lx = startX + fabsf(sinf(seed * 12.34f)) * w;
            float cx = lx, cy = startY;
            
            Vector2 pts[20]; int pIdx = 0;
            pts[pIdx++] = (Vector2){cx, cy};

            while (cy < startY + h * 0.7f && pIdx < 18) {
                float nx = cx + (sinf(seed * pIdx * 45.6f) * 150.0f - 75.0f);
                float ny = cy + 30.0f + fabsf(sinf(seed * pIdx * 12.3f) * 50.0f);
                
                // Inspeksi Batas Orientasi Petir (Lightning Trajectory Check)
                if (nx < startX) nx = startX;
                if (nx > endX) nx = endX;
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

            // C. Pertumbuhan Akar Cabang Sekunder (Secondary Root Branching)
            if (pIdx > 4) {
                int splitIdx = pIdx / 2;
                cx = pts[splitIdx].x; cy = pts[splitIdx].y;
                Vector2 bPts[5]; int bIdx = 0;
                bPts[bIdx++] = (Vector2){cx, cy};
                for(int i=0; i<3; i++) {
                    float nx = cx + (sinf(seed * i * 89.1f) * 100.0f - 50.0f);
                    float ny = cy + 40.0f;
                    
                    // Inspeksi Batas Orientasi Cabang (Branch Trajectory Check)
                    if (nx < startX) nx = startX;
                    if (nx > endX) nx = endX;
                    
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

    // 2. Proses menghitung posisi dan menggambar rintik hujan
    int dropCount = isStorm ? 800 : 260;
    float windSpeed = isStorm ? 5.0f : 1.0f; 
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

        // Menggambar arah jatuhnya rintik hujan
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
 * @brief Memproyeksikan jejaring titik dan garis konstelasi astronomi.
 * 
 * Menggambar garis maya antar rasi bintang secara putus-putus manual.
 * sebagai substitusi teknis `setLineDash()` dari ranah Canvas API. Dalam pemutaran ranah simulasi (Wireframe mode),
 * sistem akan menggunakan fungsi primitif DDA_DashedLine buatan sendiri.
 * 
 * @param stars Larik vektor yang memuat himpunan koordinat verteks rasi bintang absolut (tanpa penskalaan offset).
 * @param starCount Batas kuantitas pemrosesan iterasi vertex rasi di dalam larik.
 * @param nightlight Determinan fase visibilitas perenderan, linear menurun menjadi pudar ketika siklus waktu siang hari.
 * @param isWire Pengondisian mode analisis bedah geometri visual.
 */
void DrawRasiBintang_Animasi(Vector2 *stars, int starCount, float nightlight, bool isWire) {
    if (starCount <= 0 || nightlight <= 0.0f) return;

    Color cLine = isWire ? GREEN : (Color){200, 230, 255, (unsigned char)(204.0f * nightlight)}; 
    Color cLineGlow = isWire ? BLANK : (Color){200, 230, 255, (unsigned char)(60.0f * nightlight)}; 
    
    Color cStar = isWire ? GREEN : (Color){255, 255, 255, (unsigned char)(255.0f * nightlight)};
    Color cStarGlow = isWire ? BLANK : (Color){255, 255, 255, (unsigned char)(80.0f * nightlight)};

    // 1. Proyeksi Edges Graf Topologi Bintang (Star Topology Edge Projection)
    if (starCount > 1) {
        for (int i = 0; i < starCount - 1; i++) {
            Vector2 p1 = stars[i];
            Vector2 p2 = stars[i+1];
            
            if (!isWire) {
                // Pelapisan Masker Iluminasi (Illumination Mask)
                DrawLineEx(p1, p2, 6.0f, cLineGlow);
                
                // Menggambar urutan garis putus-putusnya
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
                        DrawLineEx(start, end, 1.5f, cLine); // Proyeksi segmen matriks
                    }
                    currentDist += step;
                    drawDash = !drawDash;
                }
            } else {
                DDA_DashedLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, 5, 5, GREEN);
            }
        }
    }

    // 2. Proyeksi Node Vertex Bintang (Star Vertex Core Projection)
    for (int i = 0; i < starCount; i++) {
        if (!isWire) {
            // Masker Iluminasi Inti (Core Illumination)
            DrawCircleV(stars[i], 8.0f, cStarGlow);
            // Inti Entitas Bintang Pusat (Central Core Entity)
            DrawCircleV(stars[i], 2.0f, cStar);
        } else {
            DrawCircleLinesV(stars[i], 2.0f, GREEN);
        }
    }
}