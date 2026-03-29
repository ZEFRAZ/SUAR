#include "kapal.h"
#include "../ui/draw_helpers.h"
#include <math.h>

// Mengambil waktu untuk animasi dari file utama
extern float previewTime;

/**
 * @brief Rutin utilitas untuk memancarkan simulasi dinamika fluida gas (Particle Smoke System).
 * 
 * Mengeksekusi generasi sekuensial lingkaran transparan asimetris bermodalkan pseudo-RNG.
 * Posisi awal bergantung pada letak cerobong dan didorong ke atas serta tertiup angin secara periodik.
 */
static void DrawProceduralSmoke(float funnelX, float funnelY, float x, float y, float scale, float angle, bool isWire) {
    int numSmoke = 15;
    for (int i = 0; i < numSmoke; i++) {
        // Membuat pola asimetris kepulan asap
        float r1 = fabsf(sinf(i * 12.9898f + funnelX) * 43758.5453f); r1 -= floorf(r1);
        float r2 = fabsf(sinf(i * 78.2330f + funnelY) * 43758.5453f); r2 -= floorf(r2);
        
        float maxLife = 1.0f + r1 * 1.5f; 
        float cycle = fmodf(previewTime + (r2 * maxLife), maxLife);
        float prog = cycle / maxLife;

        // Arah mengapung asap
        float px = funnelX - (prog * (40.0f + r1 * 30.0f));
        float py = funnelY - (prog * (50.0f + r2 * 30.0f));

        // Mengurangi besaran kepulan asap
        float radius = (2.0f + r1 * 2.0f) + (prog * 12.0f);
        
        float alpha = (1.0f - prog) * 0.45f; 
        if (alpha < 0) alpha = 0;

        Vector2 center = TransformPola(px, py, x, y, scale, angle);
        Color smokeCol = isWire ? GREEN : (Color){100, 100, 105, (unsigned char)(alpha * 255.0f)};

        if (cycle > 0.05f) { // Mencegah asap muncul terlalu besar di awal
            if (!isWire) DrawCircleV(center, radius * scale, smokeCol);
            else DrawCircleLinesV(center, radius * scale, smokeCol);
        }
    }
}

/**
 * @brief Analisator geospasial geometri (Point-in-Polygon Hit Testing).
 * 
 * Menggunakan algoritma *Ray-Casting* yang dimodifikasi untuk menembakkan sinaran lurus imajiner 
 * melintasi batas lambung kapal bajak laut. Berguna untuk menginstruksikan modul kliping khusus (Custom Clipping).
 */
static bool IsPointInPirateHull(Vector2 p) {
    Vector2 poly[40]; int pIdx = 0;
    poly[pIdx++] = (Vector2){-110, -25}; poly[pIdx++] = (Vector2){-80, -25};
    poly[pIdx++] = (Vector2){-80, -15};  poly[pIdx++] = (Vector2){-45, -15};
    poly[pIdx++] = (Vector2){-45, 0};    poly[pIdx++] = (Vector2){45, 0};
    poly[pIdx++] = (Vector2){45, -15};   poly[pIdx++] = (Vector2){95, -15};
    poly[pIdx++] = (Vector2){120, -30};
    Vector2 c1_p0 = (Vector2){120, -30}, c1_p1 = (Vector2){80, 45}, c1_p2 = (Vector2){20, 45};
    for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; poly[pIdx++] = (Vector2){inv*inv*c1_p0.x + 2.0f*inv*t*c1_p1.x + t*t*c1_p2.x, inv*inv*c1_p0.y + 2.0f*inv*t*c1_p1.y + t*t*c1_p2.y}; }
    poly[pIdx++] = (Vector2){-40, 45};
    Vector2 c2_p0 = (Vector2){-40, 45}, c2_p1 = (Vector2){-90, 45}, c2_p2 = (Vector2){-110, -10};
    for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; poly[pIdx++] = (Vector2){inv*inv*c2_p0.x + 2.0f*inv*t*c2_p1.x + t*t*c2_p2.x, inv*inv*c2_p0.y + 2.0f*inv*t*c2_p1.y + t*t*c2_p2.y}; }

    bool inside = false;
    for (int i = 0, j = pIdx - 1; i < pIdx; j = i++) {
        if (((poly[i].y > p.y) != (poly[j].y > p.y)) && (p.x < (poly[j].x - poly[i].x) * (p.y - poly[i].y) / (poly[j].y - poly[i].y) + poly[i].x)) {
            inside = !inside;
        }
    }
    return inside;
}

/**
 * @brief Memproyeksikan hiasan panel kayu melengkung pada lambung yang tunduk secara ketat pada kliping batas jaring.
 * 
 * Mengimitasi fungsionalitas murni `ctx.clip()` dari pustaka Canvas API. Menggunakan evaluasi Point-in-Polygon kontinu
 * setiap beberapa jarak tertentu untuk menggambar garis kurva.
 */
static void DrawClippedStrip(Vector2 p0, Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color col, bool isWire) {
    int segments = 60; // Jumlah potongan garis agar kurva terlihat halus
    Vector2 prev = {0,0};
    bool hasPrev = false;
    for (int i = 0; i <= segments; i++) {
        float t = (float)i / segments; float inv = 1.0f - t;
        float px = inv*inv*p0.x + 2.0f*inv*t*p1.x + t*t*p2.x;
        float py = inv*inv*p0.y + 2.0f*inv*t*p1.y + t*t*p2.y;
        
        // Mengecek apakah titik berada di dalam kapal
        if (IsPointInPirateHull((Vector2){px, py})) {
            Vector2 curr = TransformPola(px, py, tx, ty, scale, angle);
            if (hasPrev) {
                if (isWire) {
                    // Cukup menggambar satu garis untuk mode wireframe
                    AlgoLine((int)prev.x, (int)prev.y, (int)curr.x, (int)curr.y, GREEN);
                } else {
                    // Mengulang menggambar garis agar terlihat tebal
                    AlgoLine((int)prev.x, (int)prev.y-1, (int)curr.x, (int)curr.y-1, col);
                    AlgoLine((int)prev.x, (int)prev.y,   (int)curr.x, (int)curr.y,   col);
                    AlgoLine((int)prev.x, (int)prev.y+1, (int)curr.x, (int)curr.y+1, col);
                    AlgoLine((int)prev.x, (int)prev.y-2, (int)curr.x, (int)curr.y-2, col);
                    AlgoLine((int)prev.x, (int)prev.y+2, (int)curr.x, (int)curr.y+2, col);
                }
            }
            prev = curr;
            hasPrev = true;
        } else {
            hasPrev = false; // Memutus garis agar tidak melewati luar kapal
        }
    }
}

/**
 * @brief Merender formasi kain layar segi empat tertiup tegangan angin kencang (Wind Inflated Sail).
 * 
 * Direkonstruksi secara murni memanfaatkan jaring simpul berseri (vertex arrays) dan persamaan Bezier kuadratik.
 */
static void DrawPirateSail(float sx, float sy, float w, float h, Color cSail, float x, float y, float scale, float angle, float sailFlap, bool isWire) {
    Color cMidBrown = (Color){135, 75, 40, 255};
    Color outYard = isWire ? GREEN : BLANK;
    Color outSail = isWire ? GREEN : BLANK; // Menyesuaikan garis luar

    Vector2 yard[] = {{sx - w/2.0f - 8.0f, sy - 4.0f}, {sx + w/2.0f + 8.0f, sy - 4.0f}, {sx + w/2.0f + 8.0f, sy + 2.0f}, {sx - w/2.0f - 8.0f, sy + 2.0f}};
    DrawCustomPoly(yard, 4, x, y, scale, angle, cMidBrown, outYard, isWire);

    Vector2 pts[30]; int idx = 0;
    pts[idx++] = (Vector2){sx, sy + h/2.0f}; // Pusat tumpuan tiang layar utama
    pts[idx++] = (Vector2){sx - w/2.0f, sy};
    pts[idx++] = (Vector2){sx + w/2.0f, sy};

    Vector2 r0 = {sx + w/2.0f, sy}; Vector2 r1 = {sx + w/2.0f + 6.0f, sy + h/2.0f}; Vector2 r2 = {sx + w/2.0f + (sailFlap * 0.4f), sy + h};
    for(int i=1; i<=6; i++) { float t = i/6.0f; float inv = 1.0f - t; pts[idx++] = (Vector2){inv*inv*r0.x + 2.0f*inv*t*r1.x + t*t*r2.x, inv*inv*r0.y + 2.0f*inv*t*r1.y + t*t*r2.y}; }
    
    Vector2 b0 = r2; Vector2 b1 = {sx, sy + h + 1.0f}; Vector2 b2 = {sx - w/2.0f + (sailFlap * 0.4f), sy + h};
    for(int i=1; i<=6; i++) { float t = i/6.0f; float inv = 1.0f - t; pts[idx++] = (Vector2){inv*inv*b0.x + 2.0f*inv*t*b1.x + t*t*b2.x, inv*inv*b0.y + 2.0f*inv*t*b1.y + t*t*b2.y}; }

    Vector2 l0 = b2; Vector2 l1 = {sx - w/2.0f - 6.0f, sy + h/2.0f}; Vector2 l2 = {sx - w/2.0f, sy};
    for(int i=1; i<=6; i++) { float t = i/6.0f; float inv = 1.0f - t; pts[idx++] = (Vector2){inv*inv*l0.x + 2.0f*inv*t*l1.x + t*t*l2.x, inv*inv*l0.y + 2.0f*inv*t*l1.y + t*t*l2.y}; }

    DrawCustomPoly(pts, idx, x, y, scale, angle, cSail, outSail, isWire);
}

/**
 * @brief Varian render kain layar trapesium terlipat spesifik bagi variasi arsitektur Galleon (Spanish Armada).
 */
static void DrawGalleonSail(float sx, float sy, float w, float h, Color cSail, float x, float y, float scale, float angle, float wind, bool isWire) {
    Color cMast = (Color){30, 15, 10, 255};
    Color outYard = isWire ? GREEN : BLANK;
    Color outSail = isWire ? GREEN : BLANK;

    Vector2 yard[] = {{sx - w/2.0f - 4.0f, sy - 3.0f}, {sx + w/2.0f + 4.0f, sy - 3.0f}, {sx + w/2.0f + 4.0f, sy + 2.0f}, {sx - w/2.0f - 4.0f, sy + 2.0f}};
    DrawCustomPoly(yard, 4, x, y, scale, angle, cMast, outYard, isWire);

    Vector2 pts[30]; int idx = 0;
    pts[idx++] = (Vector2){sx, sy + h/2.0f}; // Pusat tumpuan tiang layar utama
    pts[idx++] = (Vector2){sx - w/2.0f, sy};
    pts[idx++] = (Vector2){sx + w/2.0f, sy};

    Vector2 r0 = {sx + w/2.0f, sy}; Vector2 r1 = {sx + w/2.0f + 3.0f, sy + h/2.0f}; Vector2 r2 = {sx + w/2.0f - wind, sy + h};
    for(int i=1; i<=6; i++) { float t=i/6.0f; float inv=1.0f-t; pts[idx++] = (Vector2){inv*inv*r0.x + 2.0f*inv*t*r1.x + t*t*r2.x, inv*inv*r0.y + 2.0f*inv*t*r1.y + t*t*r2.y}; }
    
    pts[idx++] = (Vector2){sx - w/2.0f - wind, sy + h}; // Batas bawah presisi linear

    Vector2 l0 = {sx - w/2.0f - wind, sy + h}; Vector2 l1 = {sx - w/2.0f - 5.0f, sy + h/2.0f}; Vector2 l2 = {sx - w/2.0f, sy};
    for(int i=1; i<=6; i++) { float t=i/6.0f; float inv=1.0f-t; pts[idx++] = (Vector2){inv*inv*l0.x + 2.0f*inv*t*l1.x + t*t*l2.x, inv*inv*l0.y + 2.0f*inv*t*l1.y + t*t*l2.y}; }

    DrawCustomPoly(pts, idx, x, y, scale, angle, cSail, outSail, isWire);
}

/**
 * @brief Memfasilitasi antarmuka perenderan independen (Standalone Wrapper) bagi efek kepulan asap pada mode Statis 2D.
 */
void DrawAsapKapal_Pola(float x, float y, float scale, bool isWire) {
    // 1. Menggambar dasar cerobong asap
    float funnelX = x - 50.0f; // Menyesuaikan letak cerobong awal asap
    float funnelY = y + 80.0f;
    
    if (!isWire) {
        DrawRectangle((int)funnelX - 15, (int)funnelY, 30, 40, (Color){50, 50, 55, 255});
        DrawRectangle((int)funnelX - 20, (int)funnelY - 8, 40, 8, (Color){200, 40, 40, 255}); // Menggambar garis pita cerobong
    } else {
        DrawRectangleLines((int)funnelX - 15, (int)funnelY, 30, 40, GREEN);
        DrawRectangleLines((int)funnelX - 20, (int)funnelY - 8, 40, 8, GREEN);
    }

    // 2. Partikel Asap
    int numSmoke = 25; // Membatasi jumlah asap yang muncul

    for (int i = 0; i < numSmoke; i++) {
        // Mengacak posisi awal munculnya partikel
        float r1 = fabsf(sinf(i * 12.9898f) * 43758.5453f); r1 -= floorf(r1);
        float r2 = fabsf(sinf(i * 78.2330f) * 43758.5453f); r2 -= floorf(r2);
        float r3 = fabsf(sinf(i * 31.4159f) * 43758.5453f); r3 -= floorf(r3);

        // Membatasi umur kepulan asap
        float maxLife = 2.0f + r1 * 2.0f; 
        
        // Menghitung siklus waktu animasi asap
        float cycle = fmodf(previewTime + r2 * 10.0f, maxLife);
        float prog = cycle / maxLife; // Menghitung status perkembangan asap

        // Menghitung arah terbang asap
        float currentY = funnelY - 10.0f - (prog * 250.0f);
        float currentX = funnelX + (r3 - 0.5f) * 30.0f + (prog * 120.0f); 

        // Memperbesar ukuran bola asap
        float radius = (8.0f + (r2 * 5.0f)) + (prog * 45.0f); 
        
        // Mengatur pudarnya intensitas asap
        float alpha = 1.0f - prog; 
        if (alpha < 0.0f) alpha = 0.0f;

        // Mengatur warna asap
        Color smokeCol = isWire ? GREEN : (Color){45, 45, 50, (unsigned char)(alpha * 160.0f)};

        if (cycle > 0.05f) { // Menunda kemunculan asap agar terlihat alami
            if (!isWire) {
                DrawCircleV((Vector2){currentX, currentY}, radius * scale * 0.5f, smokeCol);
            } else {
                DrawCircleLinesV((Vector2){currentX, currentY}, radius * scale * 0.5f, smokeCol);
            }
        }
    }
}


void DrawShip_Sloop(float x, float y, float scale, float angle, float sailFlap, Color hullCol, Color sailCol, bool isWire) {
    DrawShip_Sloop_Variant(x, y, scale, angle, sailFlap, 0, isWire); 
}
void DrawShip_Pirate(float x, float y, float scale, float angle, float sailFlap, Color hullCol, Color sailCol, bool isWire) {
    DrawShip_Pirate_Variant(x, y, scale, angle, sailFlap, 0, isWire); 
}

/**
 * @brief Merender arsitektur Kapal Kargo komersial berdetail presisi dengan muatan kontainer berlapis lapis.
 * 
 * Dilengkapi dengan struktur komando navigasi menjulang dan modulasi pelapisan trim cat lambung depan.
 */
void DrawShip_Cargo(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire) {
    float s = scale; 

    // Daftar warna kapal
    Color cRedHull   = (Color){239, 62, 74, 255};   // #EF3E4A
    Color cBlueHull  = (Color){38, 78, 102, 255};   // #264E66
    Color cLightBlue = (Color){100, 200, 220, 255}; // #64C8DC
    Color cPort      = (Color){28, 46, 61, 255};    // #1C2E3D
    Color cWhite     = (Color){255, 255, 255, 255}; // #FFFFFF
    Color cLine      = (Color){0, 0, 0, 38};        // rgba(0,0,0,0.15)
    Color outLine    = isWire ? GREEN : BLANK;

    // 1. Lambung Bawah (Merah)
    Vector2 lHull[] = {{-150, 5}, {140, 5}, {120, 20}, {-140, 20}};
    DrawCustomPoly(lHull, 4, x, y, s, angle, cRedHull, outLine, isWire);

    // 2. Menggambar lambung utama kapal tanpa ruang kosong
    if (!isWire) {
        Vector2 pBase[]  = {{-150, 5}, {140, 5}, {115, -5}, {-120, -5}};
        Vector2 pFront[] = {{140, 5}, {180, -20}, {130, -20}, {115, -5}};
        Vector2 pBack[]  = {{-150, 5}, {-120, -5}, {-135, -25}, {-165, -25}};
        DrawCustomPoly(pBase, 4, x, y, s, angle, cBlueHull, BLANK, false);
        DrawCustomPoly(pFront, 4, x, y, s, angle, cBlueHull, BLANK, false);
        DrawCustomPoly(pBack, 4, x, y, s, angle, cBlueHull, BLANK, false);
    } else {
        // Menggambar outline kapal untuk mode wireframe
        Vector2 uHull[] = {{-150, 5}, {140, 5}, {180, -20}, {130, -20}, {115, -5}, {-120, -5}, {-135, -25}, {-165, -25}};
        for (int i = 0; i < 8; i++) {
            Vector2 p1 = TransformPola(uHull[i].x, uHull[i].y, x, y, s, angle);
            Vector2 p2 = TransformPola(uHull[(i+1)%8].x, uHull[(i+1)%8].y, x, y, s, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }

    // 3. Trim Biru Muda di Moncong
    DrawCustomRect(145, -12, 12, 4, x, y, s, angle, cLightBlue, outLine, isWire);

    // 4. Portholes (Menghitung titik-titik posisi jendela)
    int portX[] = {-125, -100, -75, 80, 105};
    for(int i=0; i<5; i++) {
        DrawCustomCircle((Vector2){portX[i], 12}, 2.5f, x, y, s, angle, cRedHull, isWire ? GREEN : cPort, isWire);
    }

    // 5. Kabin Bertingkat (Putih)
    DrawCustomRect(-115, -45, 60, 40, x, y, s, angle, cWhite, outLine, isWire);
    DrawCustomRect(-115, -75, 40, 30, x, y, s, angle, cWhite, outLine, isWire);

    // 6. Menggambar barisan jendela kabin
    for(int i=0; i<5; i++) DrawCustomRect(-108 + i*11, -25, 6, 6, x, y, s, angle, cBlueHull, outLine, isWire);
    DrawCustomRect(-108, -55, 25, 4, x, y, s, angle, cBlueHull, outLine, isWire);
    DrawCustomRect(-108, -67, 25, 4, x, y, s, angle, cBlueHull, outLine, isWire);

    // 7. Menggambar antena dan tiang
    DrawCustomLine((Vector2){-100, -75}, (Vector2){-100, -120}, x, y, s, angle, isWire ? GREEN : cBlueHull);
    if (!isWire) DrawCustomLine((Vector2){-99, -75}, (Vector2){-99, -120}, x, y, s, angle, cBlueHull);

    // 8. Menggambar susunan kontainer kargo
    Color contColors[2][8] = {
        { (Color){42, 66, 83, 255}, (Color){50, 134, 92, 255}, (Color){85, 106, 122, 255}, (Color){239, 62, 74, 255}, (Color){19, 53, 42, 255}, (Color){63, 179, 165, 255}, (Color){140, 122, 107, 255}, (Color){233, 191, 50, 255} },
        { (Color){19, 53, 42, 255}, (Color){239, 62, 74, 255}, (Color){233, 191, 50, 255}, (Color){50, 134, 92, 255}, (Color){63, 179, 165, 255}, (Color){106, 102, 125, 255}, (Color){239, 62, 74, 255}, (Color){169, 147, 85, 255} }
    };
    
    float cw = 18.0f, ch = 28.0f, startX = -40.0f;
    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < 8; c++) {
            float px = startX + c * (cw + 1.0f);
            float py = -5.0f - (r + 1) * ch;
            Color fillC = isWire ? BLANK : contColors[r][c];
            DrawCustomRect(px, py, cw, ch - 1.0f, x, y, s, angle, fillC, outLine, isWire);
            
            Color lCol = isWire ? GREEN : cLine;
            for (int l = 1; l < 4; l++) {
                DrawCustomLine((Vector2){px + (l * cw/4.0f), py}, (Vector2){px + (l * cw/4.0f), py + ch - 1.0f}, x, y, s, angle, lCol);
            }
        }
    }
    DrawProceduralSmoke(-100, -120, x, y, s, angle, isWire);
}

/**
 * @brief Memproyeksikan struktur mega Kapal Pesiar wisata multi-strata (Multi-deck Cruise Liner).
 * 
 * Melibatkan penggambaran 9 geladak terpisah yang diderivasikan oleh konstanta kemiringan buritan dan haluan (Sloped Ratios)
 * serta geometri jendela lambung berbanjar.
 */
void DrawShip_Cruise(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire) {
    float s = scale * 1.2f;

    // Palet Warna
    Color cWhite  = (Color){252, 252, 252, 255};
    Color cPort   = (Color){190, 195, 200, 255};
    Color cOrange = (Color){255, 165, 75, 255};
    Color cGray1  = (Color){145, 148, 152, 255};
    Color cGray2  = (Color){220, 222, 225, 255};

    Color outLine = isWire ? GREEN : BLANK;

    // 1. Lambung Bawah (Putih)
    Vector2 bHull[30]; int bhIdx = 0;
    bHull[bhIdx++] = (Vector2){-135, 15};
    bHull[bhIdx++] = (Vector2){110, 15};
    // Kurva Moncong Depan
    Vector2 cb_p0 = {110, 15}, cb_p1 = {150, 15}, cb_p2 = {165, -8};
    for(int i=1; i<=10; i++) {
        float t = i/10.0f; float inv = 1.0f - t;
        bHull[bhIdx++] = (Vector2){inv*inv*cb_p0.x + 2.0f*inv*t*cb_p1.x + t*t*cb_p2.x, inv*inv*cb_p0.y + 2.0f*inv*t*cb_p1.y + t*t*cb_p2.y};
    }
    bHull[bhIdx++] = (Vector2){-140, -8};
    DrawCustomPoly(bHull, bhIdx, x, y, s, angle, cWhite, outLine, isWire);

    // 2. Menghitung posisi deretan jendela bulat
    int pStarts[] = {-125, -20, 55};
    int pCounts[] = {13, 7, 6};
    for(int p=0; p<3; p++) {
        for(int i=0; i<pCounts[p]; i++) {
            // Meneruskan status wireframe ke fungsi gambar
            DrawCustomCircle((Vector2){pStarts[p] + i * 5.5f, 5}, 2.0f, x, y, s, angle, cPort, BLANK, isWire);
        }
    }

    // Rumus bantu untuk kemiringan lambung kapal
    #define CRUISE_LX(y_val) (-140.0f - (((y_val) + 8.0f) * 0.4f))
    #define CRUISE_RX(y_val) (150.0f + ((y_val) * 0.6f))

    // 3. Garis Dek Oranye
    Vector2 orangeStrip[] = {
        {CRUISE_LX(-8.0f), -8.0f}, {CRUISE_RX(-8.0f), -8.0f},
        {CRUISE_RX(-11.0f), -11.0f}, {CRUISE_LX(-11.0f), -11.0f}
    };
    DrawCustomPoly(orangeStrip, 4, x, y, s, angle, cOrange, outLine, isWire);

    // 4. 9 Lapis Geladak (Strips)
    float stripY[] = {-11, -14.5f, -18, -21.5f, -25, -28.5f, -32, -35.5f, -39, -42.5f};
    Color stripC[] = {cGray1, cGray2, cWhite, cGray2, cWhite, cGray2, cWhite, cGray2, cGray1};
    
    for (int i = 0; i < 9; i++) {
        Vector2 strip[] = {
            {CRUISE_LX(stripY[i]), stripY[i]}, {CRUISE_RX(stripY[i]), stripY[i]},
            {CRUISE_RX(stripY[i+1]), stripY[i+1]}, {CRUISE_LX(stripY[i+1]), stripY[i+1]}
        };
        DrawCustomPoly(strip, 4, x, y, s, angle, stripC[i], outLine, isWire);
    }

    // 5. Menggambar tingkatan dek kabin
    float cabY[] = {-42.5f, -46.5f, -50.5f, -54.5f};
    Color cabC[] = {cWhite, cGray1, cWhite};
    
    for (int i = 0; i < 3; i++) {
        // Blok Kabin Kiri
        Vector2 cLeft[] = {
            {-95.0f + (cabY[i] - -42.5f) * -1.0f, cabY[i]},
            {-20.0f + (cabY[i] - -42.5f) * 1.0f,  cabY[i]},
            {-20.0f + (cabY[i+1] - -42.5f) * 1.0f, cabY[i+1]},
            {-95.0f + (cabY[i+1] - -42.5f) * -1.0f, cabY[i+1]}
        };
        DrawCustomPoly(cLeft, 4, x, y, s, angle, cabC[i], outLine, isWire);

        // Blok Kabin Kanan
        Vector2 cRight[] = {
            {10.0f + (cabY[i] - -42.5f) * -1.0f, cabY[i]},
            {95.0f + (cabY[i] - -42.5f) * 1.0f,  cabY[i]},
            {95.0f + (cabY[i+1] - -42.5f) * 1.0f, cabY[i+1]},
            {10.0f + (cabY[i+1] - -42.5f) * -1.0f, cabY[i+1]}
        };
        DrawCustomPoly(cRight, 4, x, y, s, angle, cabC[i], outLine, isWire);
    }

    // 6. Cerobong Asap (Funnels)
    Vector2 fLeft[] = {{-62, -54.5f}, {-47, -54.5f}, {-52, -64.5f}, {-67, -64.5f}};
    DrawCustomPoly(fLeft, 4, x, y, s, angle, cOrange, outLine, isWire);
    DrawCustomRect(-42, -57, 5, 2.5f, x, y, s, angle, cOrange, outLine, isWire);

    Vector2 fRight[] = {{42, -54.5f}, {57, -54.5f}, {52, -64.5f}, {37, -64.5f}};
    DrawCustomPoly(fRight, 4, x, y, s, angle, cOrange, outLine, isWire);
    DrawCustomRect(60, -57, 5, 2.5f, x, y, s, angle, cOrange, outLine, isWire);

    // 7. Sayap Belakang (Antenna / Fin)
    Vector2 fin[] = {{-117, -42.5f}, {-115, -42.5f}, {-116, -75}};
    DrawCustomPoly(fin, 3, x, y, s, angle, cGray1, outLine, isWire);

    DrawProceduralSmoke(-55, -64.5f, x, y, s, angle, isWire);
    DrawProceduralSmoke(48, -64.5f, x, y, s, angle, isWire);

    // Hapus makro agar tidak mengotori scope file
    #undef CRUISE_LX
    #undef CRUISE_RX
}

/**
 * @brief Menghasilkan proyeksi geometris Kapal Perusak Militer (Military Destroyer) bersiluman parsial.
 * 
 * Siluet dikonstruksi mayoritas dari sudut poligon tajam bernuansa gelap militer, disertai integrasi menara meriam 
 * (Artillery Turret) dan antena instrumen komunikasi jamak di superstruktur atas.
 */
void DrawShip_Military(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire) {
    float sf = scale * 0.70f; // Pengaturan skala pengecilan ukuran

    // Kumpulan warna bahan kapal
    Color cRedBot   = (Color){235, 60, 70, 255};
    Color cRedShade = (Color){190, 45, 55, 255};
    Color cDark     = (Color){45, 50, 65, 255};
    Color cHullGrey = (Color){145, 150, 160, 255};
    Color cLineG    = (Color){115, 120, 135, 255};
    Color cTurret   = (Color){100, 110, 125, 255};
    Color cBridge   = (Color){165, 170, 180, 255};
    Color outLine   = isWire ? GREEN : BLANK;
    Color lineD     = isWire ? GREEN : cDark;

    // 1. Lambung Bawah (Merah Terang)
    Vector2 hb1[] = {{-140, 15}, {125, 15}, {105, 30}, {-140, 30}};
    DrawCustomPoly(hb1, 4, x, y, sf, angle, cRedBot, outLine, isWire);
    
    // 2. Bayangan Lambung Bawah (Merah Gelap)
    Vector2 hb2[] = {{-140, 25}, {112, 25}, {105, 30}, {-140, 30}};
    DrawCustomPoly(hb2, 4, x, y, sf, angle, cRedShade, outLine, isWire);
    
    // 3. Aksen Kotak Gelap Lambung Bawah
    DrawCustomRect(-110, 19, 15, 3, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(40, 19, 15, 3, x, y, sf, angle, cDark, outLine, isWire);

    // 4. Menggambar lambung atas kapal
    Vector2 hFront[] = {{-65, -15}, {155, -15}, {125, 15}, {-65, 15}};
    
    Vector2 hRear[30]; int hrIdx = 0;
    hRear[hrIdx++] = (Vector2){-140, 0};
    hRear[hrIdx++] = (Vector2){-140, 15};
    hRear[hrIdx++] = (Vector2){-65, 15};
    hRear[hrIdx++] = (Vector2){-65, -30};
    hRear[hrIdx++] = (Vector2){-145, -30};
    Vector2 cr_p0 = {-145, -30}, cr_p1 = {-140, -15}, cr_p2 = {-140, 0};
    for(int i=1; i<10; i++) {
        float t = i/10.0f; float inv = 1.0f - t;
        hRear[hrIdx++] = (Vector2){inv*inv*cr_p0.x + 2.0f*inv*t*cr_p1.x + t*t*cr_p2.x, inv*inv*cr_p0.y + 2.0f*inv*t*cr_p1.y + t*t*cr_p2.y};
    }

    if (!isWire) {
        DrawCustomPoly(hFront, 4, x, y, sf, angle, cHullGrey, BLANK, false);
        // Titik Jangkar Triangulasi Fan untuk mencegah overlap
        Vector2 safeCenter = TransformPola(-100, -10, x, y, sf, angle);
        for(int i=0; i<hrIdx; i++) {
            Vector2 p1 = TransformPola(hRear[i].x, hRear[i].y, x, y, sf, angle);
            Vector2 p2 = TransformPola(hRear[(i+1)%hrIdx].x, hRear[(i+1)%hrIdx].y, x, y, sf, angle);
            DrawTriangle(safeCenter, p1, p2, cHullGrey); DrawTriangle(safeCenter, p2, p1, cHullGrey);
        }
    } else {
        // Menggambar outline luar kapal lengkap
        Vector2 hOut[40]; int hoIdx=0;
        hOut[hoIdx++] = (Vector2){-140, 15}; hOut[hoIdx++] = (Vector2){125, 15};
        hOut[hoIdx++] = (Vector2){155, -15}; hOut[hoIdx++] = (Vector2){-65, -15};
        hOut[hoIdx++] = (Vector2){-65, -30}; hOut[hoIdx++] = (Vector2){-145, -30};
        for(int i=1; i<=10; i++) {
            float t = i/10.0f; float inv = 1.0f - t;
            hOut[hoIdx++] = (Vector2){inv*inv*cr_p0.x + 2.0f*inv*t*cr_p1.x + t*t*cr_p2.x, inv*inv*cr_p0.y + 2.0f*inv*t*cr_p1.y + t*t*cr_p2.y};
        }
        for(int i=0; i<hoIdx; i++) {
            Vector2 p1 = TransformPola(hOut[i].x, hOut[i].y, x, y, sf, angle);
            Vector2 p2 = TransformPola(hOut[(i+1)%hoIdx].x, hOut[(i+1)%hoIdx].y, x, y, sf, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }

    // 5. Menggambar garis strip luar kapal
    Color trimCol = isWire ? GREEN : cLineG;
    float thLine = isWire ? 0.0f : 1.5f;
    for(float off = -thLine; off <= thLine; off += 1.0f) {
        DrawCustomLine((Vector2){-25, -4.5f+off}, (Vector2){60, -4.5f+off}, x, y, sf, angle, trimCol);
        DrawCustomLine((Vector2){-25, 2.5f+off}, (Vector2){60, 2.5f+off}, x, y, sf, angle, trimCol);
        DrawCustomLine((Vector2){-25, 9.5f+off}, (Vector2){60, 9.5f+off}, x, y, sf, angle, trimCol);
        if(isWire) break;
    }

    // 6. Jendela Lambung (Rectangles)
    for (int r = 0; r < 2; r++) { 
        for (int c = 0; c < 4; c++) { 
            DrawCustomRect(-135.0f + c * 10.0f, -23.0f + r * 7.0f, 5, 5, x, y, sf, angle, cDark, outLine, isWire);
        } 
    } 
    DrawCustomRect(-85, -23, 5, 5, x, y, sf, angle, cDark, outLine, isWire); 
    DrawCustomRect(-75, -23, 5, 5, x, y, sf, angle, cDark, outLine, isWire); 
    DrawCustomRect(-85, -16, 5, 5, x, y, sf, angle, cDark, outLine, isWire); 
    DrawCustomRect(130, -5, 8, 5, x, y, sf, angle, cDark, outLine, isWire);

    // 7. Meriam (Turret) Belakang
    Vector2 rt[] = {{-105, -30}, {-85, -30}, {-85, -52}, {-100, -52}};
    DrawCustomPoly(rt, 4, x, y, sf, angle, cTurret, outLine, isWire);
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) { 
        DrawCustomLine((Vector2){-95, -45+off}, (Vector2){-135, -49+off}, x, y, sf, angle, lineD); 
        if(isWire) break; 
    }
    DrawCustomRect(-138, -50.5f, 4, 3, x, y, sf, angle, cDark, outLine, isWire);

    // 8. Superstruktur / Kabin Utama
    DrawCustomRect(-25, -35, 75, 20, x, y, sf, angle, cBridge, outLine, isWire);
    for(int i=0; i<5; i++) DrawCustomRect(-15.0f + i*13.0f, -32, 8, 8, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(-10, -45, 45, 10, x, y, sf, angle, cHullGrey, outLine, isWire);
    
    // Menggambar bagian kubah radar
    Vector2 d1[15]; int d1Idx=0;
    for(int i=0; i<=10; i++) { float a = PI + i * (PI / 10.0f); d1[d1Idx++] = (Vector2){-5 + cosf(a)*4, -45 + sinf(a)*4}; }
    DrawCustomPoly(d1, d1Idx, x, y, sf, angle, cTurret, outLine, isWire);
    
    Vector2 d2[15]; int d2Idx=0;
    for(int i=0; i<=10; i++) { float a = PI + i * (PI / 10.0f); d2[d2Idx++] = (Vector2){30 + cosf(a)*4, -45 + sinf(a)*4}; }
    DrawCustomPoly(d2, d2Idx, x, y, sf, angle, cTurret, outLine, isWire);

    Vector2 mTop[] = {{5, -45}, {20, -45}, {15, -65}, {10, -65}};
    DrawCustomPoly(mTop, 4, x, y, sf, angle, cTurret, outLine, isWire);

    // 9. Tiang Komunikasi & Radar
    for(float off = -0.5f; off <= 0.5f; off+=0.5f) {
        DrawCustomLine((Vector2){-20+off, -35}, (Vector2){-20+off, -70}, x, y, sf, angle, lineD);
        DrawCustomLine((Vector2){45+off, -35}, (Vector2){45+off, -70}, x, y, sf, angle, lineD);
        DrawCustomLine((Vector2){12.5f+off, -65}, (Vector2){12.5f+off, -95}, x, y, sf, angle, lineD);
        DrawCustomLine((Vector2){6, -75+off}, (Vector2){19, -75+off}, x, y, sf, angle, lineD);
        if(isWire) break;
    }
    DrawCustomCircle((Vector2){-20, -70}, 2.0f, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomCircle((Vector2){45, -70}, 2.0f, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomCircle((Vector2){12.5f, -95}, 3.5f, x, y, sf, angle, cDark, outLine, isWire);

    // 10. Menggambar meriam depan kapal
    Vector2 ft[] = {{80, -15}, {100, -15}, {100, -35}, {85, -35}};
    DrawCustomPoly(ft, 4, x, y, sf, angle, cTurret, outLine, isWire);
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) { 
        DrawCustomLine((Vector2){95, -28+off}, (Vector2){135, -32+off}, x, y, sf, angle, lineD); 
        if(isWire) break; 
    }
    DrawCustomRect(132, -33.5f, 4, 3, x, y, sf, angle, cDark, outLine, isWire);

    // 11. Pagar Pengaman (Railing)
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) {
        DrawCustomLine((Vector2){-145, -38+off}, (Vector2){-65, -38+off}, x, y, sf, angle, lineD);
        DrawCustomLine((Vector2){-65, -23+off}, (Vector2){75, -23+off}, x, y, sf, angle, lineD);
        for(float rx = -145; rx <= -65; rx += 10) DrawCustomLine((Vector2){rx+off, -30}, (Vector2){rx+off, -38}, x, y, sf, angle, lineD);
        for(float rx = -65; rx <= 75; rx += 10) DrawCustomLine((Vector2){rx+off, -15}, (Vector2){rx+off, -23}, x, y, sf, angle, lineD);
        if(isWire) break;
    }
}

/**
 * @brief Merender formasi Kapal Nelayan bertiang tarik tradisional dengan rigi tali temali menjuntai.
 * 
 * Penggarisan kelengkungan perahu diproses menggunakan poligon trapesium manual guna meminimalkan biaya *overhead* 
 * dari perenderan Bezier di tingkat mesin, disertai detail mekanis semacam katrol pengait jaring.
 */
void DrawShip_Fishing(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire) {

    y += 20.0f * scale; // Menurunkan posisi tenggelam kapal

    float sf = scale * 0.45f; // Menyesuaikan skala pengecilan
    
    Color cHullBot = (Color){80, 150, 120, 255};
    Color cHullTop = (Color){240, 235, 215, 255};
    Color cTrim    = (Color){115, 190, 150, 255};
    Color cWinIn   = (Color){85, 105, 125, 255};
    Color cDark    = (Color){55, 60, 70, 255};
    Color cMetal   = (Color){165, 170, 175, 255};
    Color cRope    = (Color){130, 135, 140, 255};
    Color cOrange  = (Color){255, 135, 35, 255};
    Color cWood    = (Color){205, 160, 100, 255};
    Color cRed     = (Color){180, 70, 50, 255};

    // 1. Menggambar dasar lambung bawah air
    Vector2 hBot[30]; int hbIdx = 0;
    hBot[hbIdx++] = (Vector2){-125, -5};
    hBot[hbIdx++] = (Vector2){-115, 20};
    hBot[hbIdx++] = (Vector2){80, 20};
    Vector2 cb0 = {80, 20}, cb1 = {120, 20}, cb2 = {135, -5};
    for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; hBot[hbIdx++] = (Vector2){inv*inv*cb0.x + 2.0f*inv*t*cb1.x + t*t*cb2.x, inv*inv*cb0.y + 2.0f*inv*t*cb1.y + t*t*cb2.y}; }
    
    if (!isWire) {
        Vector2 center = TransformPola(0, 5, x, y, sf, angle);
        for(int i=0; i<hbIdx; i++) {
            Vector2 p1 = TransformPola(hBot[i].x, hBot[i].y, x, y, sf, angle); Vector2 p2 = TransformPola(hBot[(i+1)%hbIdx].x, hBot[(i+1)%hbIdx].y, x, y, sf, angle);
            DrawTriangle(center, p1, p2, cHullBot); DrawTriangle(center, p2, p1, cHullBot);
        }
    } else {
        for(int i=0; i<hbIdx; i++) {
            Vector2 p1 = TransformPola(hBot[i].x, hBot[i].y, x, y, sf, angle); Vector2 p2 = TransformPola(hBot[(i+1)%hbIdx].x, hBot[(i+1)%hbIdx].y, x, y, sf, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }

    // 2. Menggambar badan atas kapal
    if (!isWire) {
        Vector2 htBox[] = {{-125, -25}, {-20, -25}, {-20, -5}, {-125, -5}};
        DrawCustomPoly(htBox, 4, x, y, sf, angle, cHullTop, BLANK, false);
        
        Vector2 htFront[30]; int hfIdx = 0;
        htFront[hfIdx++] = (Vector2){-20, -35}; htFront[hfIdx++] = (Vector2){150, -35};
        Vector2 cf0 = {150, -35}, cf1 = {145, -20}, cf2 = {135, -5}; 
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; htFront[hfIdx++] = (Vector2){inv*inv*cf0.x + 2.0f*inv*t*cf1.x + t*t*cf2.x, inv*inv*cf0.y + 2.0f*inv*t*cf1.y + t*t*cf2.y}; }
        htFront[hfIdx++] = (Vector2){-20, -5};
        
        Vector2 center2 = TransformPola(50, -15, x, y, sf, angle);
        for(int i=0; i<hfIdx; i++) {
            Vector2 p1 = TransformPola(htFront[i].x, htFront[i].y, x, y, sf, angle); Vector2 p2 = TransformPola(htFront[(i+1)%hfIdx].x, htFront[(i+1)%hfIdx].y, x, y, sf, angle);
            DrawTriangle(center2, p1, p2, cHullTop); DrawTriangle(center2, p2, p1, cHullTop);
        }
    } else {
        Vector2 htFull[40]; int htFIdx = 0;
        htFull[htFIdx++] = (Vector2){-125, -5}; htFull[htFIdx++] = (Vector2){135, -5};
        Vector2 cf0 = {135, -5}, cf1 = {145, -20}, cf2 = {150, -35};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; htFull[htFIdx++] = (Vector2){inv*inv*cf0.x + 2.0f*inv*t*cf1.x + t*t*cf2.x, inv*inv*cf0.y + 2.0f*inv*t*cf1.y + t*t*cf2.y}; }
        htFull[htFIdx++] = (Vector2){-20, -35}; htFull[htFIdx++] = (Vector2){-20, -25}; htFull[htFIdx++] = (Vector2){-125, -25};
        for(int i=0; i<htFIdx; i++) {
            Vector2 p1 = TransformPola(htFull[i].x, htFull[i].y, x, y, sf, angle); Vector2 p2 = TransformPola(htFull[(i+1)%htFIdx].x, htFull[(i+1)%htFIdx].y, x, y, sf, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }

    // 3. Menggambar garis kayu penghias pinggir kapal
    Color outTrim = isWire ? GREEN : cTrim;
    float th = isWire ? 0.0f : 1.5f;
    for(float off = -th; off <= th; off += 1.0f) {
        DrawCustomLine((Vector2){-125, -25+off}, (Vector2){-20, -25+off}, x, y, sf, angle, outTrim);
        DrawCustomLine((Vector2){-20+off, -25}, (Vector2){-20+off, -35}, x, y, sf, angle, outTrim);
        DrawCustomLine((Vector2){-20, -35+off}, (Vector2){150, -35+off}, x, y, sf, angle, outTrim);
        if(isWire) break;
    }

    // 4. Menggambar deretan jendela bundar
    int phX[] = {-100, -65, 10, 45, 80};
    for(int i=0; i<5; i++) {
        DrawCustomCircle((Vector2){phX[i], -15}, 6.0f, x, y, sf, angle, cHullTop, BLANK, isWire);
        DrawCustomCircle((Vector2){phX[i], -15}, 4.0f, x, y, sf, angle, cWinIn, BLANK, isWire);
    }

    // 5. Menggambar ruang kemudi dan kabin atas
    DrawCustomRect(-50, -45, 30, 20, x, y, sf, angle, cHullTop, isWire ? GREEN : BLANK, isWire);
    Vector2 trimTop[] = {{-55, -45}, {-20, -45}, {-20, -40}, {-55, -40}}; DrawCustomPoly(trimTop, 4, x, y, sf, angle, cTrim, isWire ? GREEN : BLANK, isWire);
    Vector2 mCab[] = {{-20, -35}, {60, -35}, {80, -75}, {-20, -75}}; DrawCustomPoly(mCab, 4, x, y, sf, angle, cHullTop, isWire ? GREEN : BLANK, isWire);
    Vector2 rTrim[] = {{-25, -75}, {85, -75}, {80, -82}, {-25, -82}}; DrawCustomPoly(rTrim, 4, x, y, sf, angle, cTrim, isWire ? GREEN : BLANK, isWire);

    // 6. Jendela Kabin
    Vector2 w1[] = {{-10, -65}, {20, -65}, {20, -45}, {-10, -45}}; DrawCustomPoly(w1, 4, x, y, sf, angle, cWinIn, isWire ? GREEN : BLANK, isWire);
    Vector2 w2[] = {{30, -65}, {68, -65}, {58, -45}, {30, -45}}; DrawCustomPoly(w2, 4, x, y, sf, angle, cWinIn, isWire ? GREEN : BLANK, isWire);

    // 7. Menggambar bagian mesin belakang
    Vector2 dome[15]; int dIdx = 0;
    for(int i=0; i<=10; i++) { float a = PI + i * (PI / 10.0f); dome[dIdx++] = (Vector2){-15 + cosf(a)*5, -82 + sinf(a)*5}; }
    DrawCustomPoly(dome, dIdx, x, y, sf, angle, cDark, isWire ? GREEN : BLANK, isWire);

    // 8. Menggambar tiang utama dan tali penahan
    Color outMetal = isWire ? GREEN : cMetal; Color outRope = isWire ? GREEN : cRope;
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) {
        DrawCustomLine((Vector2){15+off, -82}, (Vector2){15+off, -160}, x, y, sf, angle, outMetal);
        DrawCustomLine((Vector2){-20, -45+off}, (Vector2){-80, -80+off}, x, y, sf, angle, outMetal);
        DrawCustomLine((Vector2){-20, -58+off}, (Vector2){-55, -75+off}, x, y, sf, angle, outMetal);
        if (isWire) break;
    }
    DrawCustomLine((Vector2){0, -140}, (Vector2){30, -140}, x, y, sf, angle, outMetal);
    DrawCustomLine((Vector2){5, -125}, (Vector2){25, -125}, x, y, sf, angle, outMetal);
    DrawCustomLine((Vector2){0, -110}, (Vector2){30, -110}, x, y, sf, angle, outMetal);
    DrawCustomLine((Vector2){15, -150}, (Vector2){145, -35}, x, y, sf, angle, outRope);
    DrawCustomLine((Vector2){15, -130}, (Vector2){-80, -80}, x, y, sf, angle, outRope);
    DrawCustomLine((Vector2){15, -115}, (Vector2){-55, -75}, x, y, sf, angle, outRope);
    DrawCustomLine((Vector2){15, -120}, (Vector2){-125, -25}, x, y, sf, angle, outRope);
    
    // 9. Menggambar derek dan sistem derek kapal
    DrawCustomLine((Vector2){-80, -80}, (Vector2){-80, -50}, x, y, sf, angle, outMetal);
    DrawCustomLine((Vector2){-55, -75}, (Vector2){-55, -45}, x, y, sf, angle, outMetal);
    DrawCustomCircle((Vector2){-80, -65}, 5, x, y, sf, angle, cDark, BLANK, isWire);
    DrawCustomCircle((Vector2){-55, -60}, 4, x, y, sf, angle, cDark, BLANK, isWire);
    DrawCustomCircle((Vector2){-80, -50}, 4, x, y, sf, angle, cDark, BLANK, isWire);
    DrawCustomCircle((Vector2){-55, -45}, 3, x, y, sf, angle, cDark, BLANK, isWire);
    
    Color outDark = isWire ? GREEN : cDark;
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) {
        DrawCustomLine((Vector2){-80+off, -50}, (Vector2){-80+off, -36}, x, y, sf, angle, outDark);
        DrawCustomLine((Vector2){-55+off, -45}, (Vector2){-55+off, -32}, x, y, sf, angle, outDark);
        if(isWire) break;
    }
    
    // Menggambar kaitan besi
    Vector2 hk1[15]; Vector2 hk2[15]; int hkIdx=0;
    for(int i=0; i<=10; i++) { float a = 0 + i * (PI / 10.0f); hk1[hkIdx] = (Vector2){-83 + cosf(a)*3, -36 + sinf(a)*3}; hk2[hkIdx] = (Vector2){-58 + cosf(a)*3, -32 + sinf(a)*3}; hkIdx++; }
    for(int i=0; i<hkIdx-1; i++) {
        DrawCustomLine(hk1[i], hk1[i+1], x, y, sf, angle, outDark); DrawCustomLine(hk2[i], hk2[i+1], x, y, sf, angle, outDark);
        if(!isWire) { DrawCustomLine((Vector2){hk1[i].x, hk1[i].y+1}, (Vector2){hk1[i+1].x, hk1[i+1].y+1}, x, y, sf, angle, outDark); DrawCustomLine((Vector2){hk2[i].x, hk2[i].y+1}, (Vector2){hk2[i+1].x, hk2[i+1].y+1}, x, y, sf, angle, outDark); }
    }

    // 10. Tiang Bendera & Bendera Belakang
    for(float off = -0.5f; off <= 0.5f; off+=0.5f) { DrawCustomLine((Vector2){-115+off, -25}, (Vector2){-115+off, -55}, x, y, sf, angle, outMetal); if(isWire) break; }
    Vector2 flag[] = {{-115, -53}, {-135, -45}, {-115, -37}}; DrawCustomPoly(flag, 3, x, y, sf, angle, cOrange, isWire ? GREEN : BLANK, isWire);

    // 11. Menggambar tumpukan boks kayu kargo
    DrawCustomRect(-110, -37, 15, 10, x, y, sf, angle, cWood, outDark, isWire);
    DrawCustomRect(82, -45, 18, 10, x, y, sf, angle, cWood, outDark, isWire);
    DrawCustomRect(85, -55, 12, 10, x, y, sf, angle, cWood, outDark, isWire);

    // 12. Kubah Merah Dek Atas (Top Half Arc)
    Vector2 redDome[15]; int rdIdx = 0;
    for(int i=0; i<=10; i++) { float a = PI + i * (PI / 10.0f); redDome[rdIdx++] = (Vector2){-30 + cosf(a)*10, -25 + sinf(a)*10}; }
    DrawCustomPoly(redDome, rdIdx, x, y, sf, angle, cRed, isWire ? GREEN : BLANK, isWire);

    // 13. PELAMPUNG CINCIN BERLUBANG (Life Rings)
    DrawCustomRing((Vector2){-85, -15}, 6.0f, 3.5f, x, y, sf, angle, cOrange, isWire);
    DrawCustomRing((Vector2){120, -20}, 6.0f, 3.5f, x, y, sf, angle, cOrange, isWire);

    // 14. Jangkar Depan (Anchor)
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) {
        DrawCustomLine((Vector2){95+off, -15}, (Vector2){95+off, 3}, x, y, sf, angle, outDark);
        DrawCustomLine((Vector2){90, -10+off}, (Vector2){100, -10+off}, x, y, sf, angle, outDark);
        if(isWire) break;
    }
    DrawCustomRing((Vector2){95, -17}, 2.5f, 1.5f, x, y, sf, angle, outDark, isWire);
    Vector2 anc[15]; int anIdx=0;
    for(int i=0; i<=10; i++) { float a = 0 + i * (PI / 10.0f); anc[anIdx++] = (Vector2){95 + cosf(a)*7, 0 + sinf(a)*7}; }
    for(int i=0; i<anIdx-1; i++) { DrawCustomLine(anc[i], anc[i+1], x, y, sf, angle, outDark); if(!isWire) DrawCustomLine((Vector2){anc[i].x, anc[i].y+1}, (Vector2){anc[i+1].x, anc[i+1].y+1}, x, y, sf, angle, outDark); }
    Vector2 fl1[] = {{102, 0}, {106, -4}, {100, -4}}; DrawCustomPoly(fl1, 3, x, y, sf, angle, cDark, isWire ? GREEN : BLANK, isWire);
    Vector2 fl2[] = {{88, 0}, {84, -4}, {90, -4}}; DrawCustomPoly(fl2, 3, x, y, sf, angle, cDark, isWire ? GREEN : BLANK, isWire);

    // 15. Pagar Depan (Railing)
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) {
        DrawCustomLine((Vector2){65, -45+off}, (Vector2){135, -45+off}, x, y, sf, angle, outMetal);
        DrawCustomLine((Vector2){135, -45+off}, (Vector2){147, -35+off}, x, y, sf, angle, outMetal);
        for(float rx = 70; rx <= 130; rx += 15) { DrawCustomLine((Vector2){rx+off, -35}, (Vector2){rx+off, -45}, x, y, sf, angle, outMetal); }
        if(isWire) break;
    }
}

/**
 * @brief Menganimasikan bentuk arsitektural Kapal Riset Observatorium Laut (Oceanographic Research Vessel).
 * 
 * Memiliki pelat lambung asimetris kompleks (*L-shaped concave profile*) yang diurai secara manual menjadi sekuens  
 * segitiga jaring (Manual Triangulation) demi mencegah artefak robekan bidang render akibat geometri tak sederhana.
 */
void DrawShip_Research(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire) {
    float sf = scale * 0.75f; // const scaleOffset = 0.75;
    
    // Palet Warna
    Color cHull  = (Color){74, 77, 84, 255};
    Color cTrim  = (Color){160, 165, 170, 255};
    Color cDark  = (Color){28, 32, 36, 255};
    Color cPort  = (Color){40, 45, 50, 255};
    Color cWhite = (Color){215, 220, 225, 255};
    Color outLine = isWire ? GREEN : BLANK;
    Color outMet  = isWire ? GREEN : cWhite;
    
    // 1. Menggambar badan utama bentuk dasar kapal
    if (!isWire) {
        Vector2 tA = TransformPola(-155, -20, x, y, sf, angle);
        Vector2 tB = TransformPola(-85, -20, x, y, sf, angle);
        Vector2 tC = TransformPola(-75, -5, x, y, sf, angle);
        Vector2 tD = TransformPola(65, -5, x, y, sf, angle);
        Vector2 tE = TransformPola(75, -20, x, y, sf, angle);
        Vector2 tF = TransformPola(125, -20, x, y, sf, angle);
        Vector2 tG = TransformPola(95, 15, x, y, sf, angle);
        Vector2 tH = TransformPola(-125, 15, x, y, sf, angle);
        
        // Blok Geladak Kiri
        DrawTriangle(tA, tB, tC, cHull); DrawTriangle(tA, tC, tB, cHull);
        DrawTriangle(tA, tC, tH, cHull); DrawTriangle(tA, tH, tC, cHull);
        // Blok Badan Tengah (Turun)
        DrawTriangle(tC, tD, tG, cHull); DrawTriangle(tC, tG, tD, cHull);
        DrawTriangle(tC, tG, tH, cHull); DrawTriangle(tC, tH, tG, cHull);
        // Blok Geladak Kanan
        DrawTriangle(tD, tE, tF, cHull); DrawTriangle(tD, tF, tE, cHull);
        DrawTriangle(tD, tF, tG, cHull); DrawTriangle(tD, tG, tF, cHull);
    } else {
        Vector2 hOut[] = {{-155, -20}, {-85, -20}, {-75, -5}, {65, -5}, {75, -20}, {125, -20}, {95, 15}, {-125, 15}};
        for (int i=0; i<8; i++) {
            Vector2 p1 = TransformPola(hOut[i].x, hOut[i].y, x, y, sf, angle);
            Vector2 p2 = TransformPola(hOut[(i+1)%8].x, hOut[(i+1)%8].y, x, y, sf, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }
    
    // 2. Garis pinggir kapal panjang
    float tThick = isWire ? 0.0f : 3.0f; // Ketebalan outline (lineWidth 8.0)
    for(float off = -tThick; off <= tThick; off += 1.0f) {
        DrawCustomLine((Vector2){-155, -20+off}, (Vector2){-85, -20+off}, x, y, sf, angle, isWire ? GREEN : cTrim);
        DrawCustomLine((Vector2){-85, -20+off}, (Vector2){-75, -5+off}, x, y, sf, angle, isWire ? GREEN : cTrim);
        DrawCustomLine((Vector2){-75, -5+off}, (Vector2){65, -5+off}, x, y, sf, angle, isWire ? GREEN : cTrim);
        DrawCustomLine((Vector2){65, -5+off}, (Vector2){75, -20+off}, x, y, sf, angle, isWire ? GREEN : cTrim);
        DrawCustomLine((Vector2){75, -20+off}, (Vector2){125, -20+off}, x, y, sf, angle, isWire ? GREEN : cTrim);
        if (isWire) break;
    }
    
    // 3. Garis pembatas cat bawah air kapal
    Vector2 dStrip[] = {{-122, 11}, {92, 11}, {95, 15}, {-125, 15}};
    DrawCustomPoly(dStrip, 4, x, y, sf, angle, cDark, outLine, isWire);
    
    // 4. Menggambar jendela deret bawah kapal
    for (int i = 0; i < 11; i++) {
        DrawCustomCircle((Vector2){-105 + i * 18, 4}, 2.5f, x, y, sf, angle, cPort, BLANK, isWire);
    }
    
    // 5. Menggambar ruangan dalam kapal utama
    Vector2 cCurve[15]; int ccIdx=0;
    Vector2 c_p0 = {-38, -32}, c_p1 = {-45, -32}, c_p2 = {-45, -22};
    for(int i=0; i<=10; i++) {
        float t = i/10.0f; float inv = 1.0f - t;
        cCurve[ccIdx++] = (Vector2){inv*inv*c_p0.x + 2.0f*inv*t*c_p1.x + t*t*c_p2.x, inv*inv*c_p0.y + 2.0f*inv*t*c_p1.y + t*t*c_p2.y};
    }
    if (!isWire) {
        // Balok Kabin Kanan
        Vector2 cR[] = {{-10, -6}, {35, -6}, {20, -50}, {-10, -50}};
        DrawCustomPoly(cR, 4, x, y, sf, angle, cWhite, BLANK, false);
        
        // Balok Kabin Kiri (Memeluk Kurva)
        Vector2 cL[20]; int clIdx=0;
        cL[clIdx++] = (Vector2){-45, -6}; cL[clIdx++] = (Vector2){-10, -6}; cL[clIdx++] = (Vector2){-10, -32};
        for(int i=0; i<ccIdx; i++) cL[clIdx++] = cCurve[i];
        cL[clIdx++] = (Vector2){-45, -22};
        
        Vector2 safeCenter = TransformPola(-20, -20, x, y, sf, angle);
        for(int i=0; i<clIdx; i++) {
            Vector2 p1 = TransformPola(cL[i].x, cL[i].y, x, y, sf, angle);
            Vector2 p2 = TransformPola(cL[(i+1)%clIdx].x, cL[(i+1)%clIdx].y, x, y, sf, angle);
            DrawTriangle(safeCenter, p1, p2, cWhite); DrawTriangle(safeCenter, p2, p1, cWhite);
        }
    } else {
        Vector2 cOut[30]; int coIdx=0;
        cOut[coIdx++] = (Vector2){-45, -6}; cOut[coIdx++] = (Vector2){35, -6}; cOut[coIdx++] = (Vector2){20, -50};
        cOut[coIdx++] = (Vector2){-10, -50}; cOut[coIdx++] = (Vector2){-10, -32};
        for(int i=0; i<ccIdx; i++) cOut[coIdx++] = cCurve[i];
        for(int i=0; i<coIdx; i++) {
            Vector2 p1 = TransformPola(cOut[i].x, cOut[i].y, x, y, sf, angle);
            Vector2 p2 = TransformPola(cOut[(i+1)%coIdx].x, cOut[(i+1)%coIdx].y, x, y, sf, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }
    
    // 6. Menggambar deret jendela
    DrawCustomRect(-5, -45, 5, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(4, -45, 5, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(13, -45, 5, 4, x, y, sf, angle, cDark, outLine, isWire);
    
    DrawCustomRect(-30, -28, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(-17, -28, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(-4, -28, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(9, -28, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    
    DrawCustomRect(-40, -15, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(-26, -15, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(-12, -15, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(2, -15, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(16, -15, 6, 4, x, y, sf, angle, cDark, outLine, isWire);
    
    // 7. Menggambar bentuk melengkung dek kapal
    Vector2 dc[30]; int dcIdx=0;
    dc[dcIdx++] = (Vector2){-140, -21}; dc[dcIdx++] = (Vector2){-110, -21}; dc[dcIdx++] = (Vector2){-110, -35};
    Vector2 cR0 = {-110, -35}, cR1 = {-110, -40}, cR2 = {-115, -40};
    for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; dc[dcIdx++] = (Vector2){inv*inv*cR0.x + 2.0f*inv*t*cR1.x + t*t*cR2.x, inv*inv*cR0.y + 2.0f*inv*t*cR1.y + t*t*cR2.y}; }
    dc[dcIdx++] = (Vector2){-135, -40};
    Vector2 cL0 = {-135, -40}, cL1 = {-140, -40}, cL2 = {-140, -35};
    for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; dc[dcIdx++] = (Vector2){inv*inv*cL0.x + 2.0f*inv*t*cL1.x + t*t*cL2.x, inv*inv*cL0.y + 2.0f*inv*t*cL1.y + t*t*cL2.y}; }
    
    if (!isWire) {
        Vector2 centerFront = TransformPola(-125, -30, x, y, sf, angle);
        for(int i=0; i<dcIdx; i++) {
            Vector2 p1 = TransformPola(dc[i].x, dc[i].y, x, y, sf, angle); Vector2 p2 = TransformPola(dc[(i+1)%dcIdx].x, dc[(i+1)%dcIdx].y, x, y, sf, angle);
            DrawTriangle(centerFront, p1, p2, cDark); DrawTriangle(centerFront, p2, p1, cDark);
        }
    } else {
        for(int i=0; i<dcIdx; i++) {
            Vector2 p1 = TransformPola(dc[i].x, dc[i].y, x, y, sf, angle); Vector2 p2 = TransformPola(dc[(i+1)%dcIdx].x, dc[(i+1)%dcIdx].y, x, y, sf, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }
    
    // 8. Menambahkan dekorasi pada belakang kapal
    DrawCustomRect(100, -26, 6, 5, x, y, sf, angle, cDark, outLine, isWire);
    DrawCustomRect(112, -26, 6, 5, x, y, sf, angle, cDark, outLine, isWire);
    
    // 9. Menggambar cerobong asap
    Vector2 funB[] = {{-30, -32}, {-14, -32}, {-16, -50}, {-28, -50}};
    DrawCustomPoly(funB, 4, x, y, sf, angle, cDark, outLine, isWire);
    Vector2 funT[] = {{-28, -50}, {-16, -50}, {-18, -68}, {-26, -68}};
    DrawCustomPoly(funT, 4, x, y, sf, angle, cWhite, outLine, isWire);
    
    // 10. Tiang radar atas
    for(float off = -1.0f; off <= 1.0f; off+=1.0f) {
        DrawCustomLine((Vector2){-5+off, -50}, (Vector2){-5+off, -85}, x, y, sf, angle, outMet);
        if(isWire) break;
    }
    DrawCustomRing((Vector2){-5, -89}, 4.0f, 1.5f, x, y, sf, angle, outMet, isWire);
    DrawCustomLine((Vector2){-11, -74}, (Vector2){1, -74}, x, y, sf, angle, outMet);
    DrawCustomLine((Vector2){-11, -81}, (Vector2){1, -81}, x, y, sf, angle, outMet);
    
    // 11. Tiang pengamat bagian depan
    for(float off = -0.5f; off <= 0.5f; off+=0.5f) {
        DrawCustomLine((Vector2){85+off, -21}, (Vector2){85+off, -110}, x, y, sf, angle, outMet);
        if (isWire) break;
    }
    DrawCustomLine((Vector2){75, -95}, (Vector2){95, -95}, x, y, sf, angle, outMet);
    DrawCustomLine((Vector2){75, -85}, (Vector2){95, -85}, x, y, sf, angle, outMet);
    DrawCustomLine((Vector2){78, -100}, (Vector2){78, -80}, x, y, sf, angle, outMet);
    DrawCustomLine((Vector2){92, -100}, (Vector2){92, -80}, x, y, sf, angle, outMet);
    
    Vector2 s1[] = {{85, -68}, {100, -68}, {100, -64}, {85, -62}};
    DrawCustomPoly(s1, 4, x, y, sf, angle, cWhite, outLine, isWire);
    Vector2 s2[] = {{85, -58}, {100, -58}, {100, -54}, {85, -52}};
    DrawCustomPoly(s2, 4, x, y, sf, angle, cWhite, outLine, isWire);

    DrawProceduralSmoke(-22, -68, x, y, sf, angle, isWire);
}

/**
 * @brief Mentransformasikan blok siluet Minimalis Kapal Yacht Mewah (Luxury Yacht).
 * 
 * Fungsi gambar kapal secara spesifik dan dioptimalkan dari kode lamanya.
 */
void DrawShip_Yacht(float x, float y, float scale, float angle, Color hullCol, Color detailCol, bool isWire) {
    float s = scale * 0.80f; // Menyesuaikan skala pengecilans

    Color cDarkBlue = (Color){35, 45, 80, 255};
    Color cMainHull = (Color){140, 150, 160, 255};
    Color cWinDark  = (Color){80, 90, 100, 255};
    Color cTrimLine = (Color){215, 210, 205, 255};
    Color cCabin    = (Color){235, 230, 225, 255};
    Color cCyanWin  = (Color){0, 150, 220, 255};
    Color cFunnel   = (Color){150, 160, 170, 255};

    // 1. Menggambar dasar kapal
    Vector2 bHull[] = {{-115, 15}, {95, 15}, {85, 25}, {-105, 25}};
    DrawCustomPoly(bHull, 4, x, y, s, angle, cDarkBlue, isWire ? GREEN : BLANK, isWire);

    // 2. Menggambar lambung kapal depan
    Vector2 mHull[30]; int mhIdx = 0;
    mHull[mhIdx++] = (Vector2){-125, 0};
    mHull[mhIdx++] = (Vector2){-115, 15};
    mHull[mhIdx++] = (Vector2){95, 15};
    mHull[mhIdx++] = (Vector2){140, -20};
    mHull[mhIdx++] = (Vector2){-40, -20};
    Vector2 hc_p0 = {-40, -20}, hc_p1 = {-45, 0}, hc_p2 = {-60, 0};
    for(int i=1; i<=10; i++) {
        float t = i/10.0f; float inv = 1.0f - t;
        mHull[mhIdx++] = (Vector2){inv*inv*hc_p0.x + 2.0f*inv*t*hc_p1.x + t*t*hc_p2.x, inv*inv*hc_p0.y + 2.0f*inv*t*hc_p1.y + t*t*hc_p2.y};
    }
    
    // Menggambar segitiga secara terpusat untuk mewarnai bagian kapal penuh
    if (!isWire) {
        Vector2 center = TransformPola(0, 0, x, y, s, angle);
        for(int i=0; i<mhIdx; i++) {
            Vector2 p1 = TransformPola(mHull[i].x, mHull[i].y, x, y, s, angle);
            Vector2 p2 = TransformPola(mHull[(i+1)%mhIdx].x, mHull[(i+1)%mhIdx].y, x, y, s, angle);
            DrawTriangle(center, p1, p2, cMainHull);
            DrawTriangle(center, p2, p1, cMainHull);
        }
    } else {
        for(int i=0; i<mhIdx; i++) {
            Vector2 p1 = TransformPola(mHull[i].x, mHull[i].y, x, y, s, angle);
            Vector2 p2 = TransformPola(mHull[(i+1)%mhIdx].x, mHull[(i+1)%mhIdx].y, x, y, s, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }

    // 3. Menggambar jendela pengamatan area kapal lambung
    for(int i=0; i<4; i++) {
        float bx = -105.0f + i * 18.0f;
        Vector2 wPts[] = {{bx, 5}, {bx+10, 5}, {bx+10, 8}, {bx, 8}};
        DrawCustomPoly(wPts, 4, x, y, s, angle, cWinDark, isWire ? GREEN : BLANK, isWire);
    }

    // 4. Garis pembatas badan kayu dan layar
    Color outTrim = isWire ? GREEN : cTrimLine;
    float th = isWire ? 0.0f : 1.0f;
    for (float off = -th; off <= th; off += 1.0f) {
        DrawCustomLine((Vector2){-125, 0 + off}, (Vector2){-60, 0 + off}, x, y, s, angle, outTrim);
        DrawCustomBezier((Vector2){-60, 0 + off}, (Vector2){-45, 0 + off}, (Vector2){-40, -20 + off}, x, y, s, angle, BLANK, outTrim, isWire);
        DrawCustomLine((Vector2){-40, -20 + off}, (Vector2){140, -20 + off}, x, y, s, angle, outTrim);
        if (isWire) break; 
    }

    // 5. Menggambar kabin
    Vector2 cabin[] = {{-115, -45}, {-115, -60}, {50, -60}, {90, -20}, {-40, -20}, {-65, -45}};
    // Menjamin warna tidak tumpah berantakan dengan pergeseran titik
    if (!isWire) {
        Vector2 cCenter = TransformPola(0, -40, x, y, s, angle);
        for(int i=0; i<6; i++) {
            Vector2 p1 = TransformPola(cabin[i].x, cabin[i].y, x, y, s, angle);
            Vector2 p2 = TransformPola(cabin[(i+1)%6].x, cabin[(i+1)%6].y, x, y, s, angle);
            DrawTriangle(cCenter, p1, p2, cCabin);
            DrawTriangle(cCenter, p2, p1, cCabin);
        }
    } else {
        for(int i=0; i<6; i++) {
            Vector2 p1 = TransformPola(cabin[i].x, cabin[i].y, x, y, s, angle);
            Vector2 p2 = TransformPola(cabin[(i+1)%6].x, cabin[(i+1)%6].y, x, y, s, angle);
            AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
        }
    }

    // 6. Menggambar jejeran kaca pelindung luas
    Color outCW = isWire ? GREEN : BLANK;
    Vector2 cWin1[] = {{-10, -54}, {15, -54}, {29, -40}, {4, -40}};
    DrawCustomPoly(cWin1, 4, x, y, s, angle, cCyanWin, outCW, isWire);
    Vector2 cWin2[] = {{20, -54}, {45, -54}, {59, -40}, {34, -40}};
    DrawCustomPoly(cWin2, 4, x, y, s, angle, cCyanWin, outCW, isWire);

    // 7. Menggambar cerobong
    Color outFunnel = isWire ? GREEN : BLANK;
    Vector2 funnel[] = {{-5, -60}, {-18, -85}, {0, -85}, {12, -60}};
    DrawCustomPoly(funnel, 4, x, y, s, angle, cFunnel, outFunnel, isWire);

    // 8. Menggambar antena atas
    Color outAnt = isWire ? GREEN : cDarkBlue;
    for(float off = -th; off <= th; off += 1.0f) {
        DrawCustomLine((Vector2){-35 + off, -60}, (Vector2){-55 + off, -105}, x, y, s, angle, outAnt);
        DrawCustomLine((Vector2){-25 + off, -60}, (Vector2){-45 + off, -100}, x, y, s, angle, outAnt);
        DrawCustomLine((Vector2){-51, -95 + off}, (Vector2){-37, -95 + off}, x, y, s, angle, outAnt);
        if (isWire) break;
    }

    // 9. Menggambar pagar samping
    Color outRail = isWire ? GREEN : cDarkBlue;
    float firstTopX = 0, lastTopX = 0; float railYTop = -36.0f;
    for(int i=0; i<7; i++) {
        float xBase = -35.0f + i * 25.0f;
        float xTop = xBase + 13.0f;
        for(float off = -th; off <= th; off += 1.0f) {
            DrawCustomLine((Vector2){xBase + off, -20.0f}, (Vector2){xTop + off, railYTop}, x, y, s, angle, outRail);
            if (isWire) break;
        }
        if (i == 0) firstTopX = xTop;
        if (i == 6) lastTopX = xTop;
    }
    for(float off = -th; off <= th; off += 1.0f) {
        DrawCustomLine((Vector2){firstTopX, railYTop + off}, (Vector2){lastTopX, railYTop + off}, x, y, s, angle, outRail);
        if (isWire) break;
    }
}

/**
 * @brief Manajer Variasi Kapal Bajak Laut (Pirate Category Variant Manager).
 * 
 * Merouting rute perenderan kepada tiga arsitektur turunan: 
 * (0) Kapal Klasik Berlambung Hitam, (1) Heavy Galleon, atau (2) Eastern Junk Pirate.
 */
void DrawShip_Pirate_Variant(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire) {
    float s = scale * 0.65f; 
    
    if (variant == 0) {
        // --- VARIAN 0: CLASSIC PIRATE ---
        Color cDarkBrown = {75, 40, 25, 255}; 
        Color cMidBrown = {135, 75, 40, 255}; 
        Color cLightBrown = {210, 145, 80, 255}; 
        Color cBlackSail = {35, 25, 25, 255}; 
        Color cRedFlag = {235, 50, 40, 255}; 
        Color cYellowWin = {255, 230, 80, 255}; 
        Color cRope = {210, 180, 140, 255}; 
        Color cWhite = {245, 245, 250, 255};

        Color outRope = isWire ? GREEN : cRope;
        Color outDB = isWire ? GREEN : BLANK; // Menyembunyikan garis pinggir

        // Menggambar susunan tali
        DrawCustomLine((Vector2){-85, -115}, (Vector2){-110, -25}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){-85, -80},  (Vector2){-110, -25}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){-85, -140}, (Vector2){0, -110},   x, y, s, angle, outRope);
        DrawCustomLine((Vector2){0, -180},   (Vector2){75, -140},  x, y, s, angle, outRope);
        DrawCustomLine((Vector2){75, -115},  (Vector2){150, -45},  x, y, s, angle, outRope);
        DrawCustomLine((Vector2){75, -80},   (Vector2){130, -35},  x, y, s, angle, outRope);
        
        float jrLeftX = -15.0f + (sailFlap * 0.4f); float jrRightX = 15.0f + (sailFlap * 0.4f);
        DrawCustomBezier((Vector2){jrLeftX, -30}, (Vector2){-35, -10}, (Vector2){-45, 0}, x, y, s, angle, BLANK, outRope, isWire);
        DrawCustomBezier((Vector2){jrRightX, -30}, (Vector2){35, -10}, (Vector2){45, 0}, x, y, s, angle, BLANK, outRope, isWire);

        // Tiang layar yang memanjang condong ke depan batas kapal
        Vector2 boom[] = {{95, -12}, {160, -47}, {158, -53}, {93, -18}}; 
        DrawCustomPoly(boom, 4, x, y, s, angle, cDarkBrown, outDB, isWire);

        // Menggambar tiga tiang layar besar bersamaan
        Vector2 mast1[] = {{-88, -145}, {-82, -145}, {-82, -15}, {-88, -15}}; DrawCustomPoly(mast1, 4, x, y, s, angle, cDarkBrown, outDB, isWire);
        Vector2 mast2[] = {{-4, -185}, {4, -185}, {4, 0}, {-4, 0}}; DrawCustomPoly(mast2, 4, x, y, s, angle, cDarkBrown, outDB, isWire);
        Vector2 mast3[] = {{72, -145}, {78, -145}, {78, -15}, {72, -15}}; DrawCustomPoly(mast3, 4, x, y, s, angle, cDarkBrown, outDB, isWire);

        // Mewarnai badan kapal memutar
        if (!isWire) {
            Vector2 d1[] = {{-110,-25}, {-80,-25}, {-80,-15}, {-110,-15}}; DrawCustomPoly(d1, 4, x, y, s, angle, cDarkBrown, BLANK, false);
            Vector2 d2[] = {{-110,-15}, {-45,-15}, {-45,0}, {-110,0}}; DrawCustomPoly(d2, 4, x, y, s, angle, cDarkBrown, BLANK, false);
            Vector2 d3[] = {{45,-15}, {95,-15}, {95,0}, {45,0}}; DrawCustomPoly(d3, 4, x, y, s, angle, cDarkBrown, BLANK, false);
            Vector2 d4[] = {{95,-15}, {120,-30}, {95,0}}; DrawCustomPoly(d4, 3, x, y, s, angle, cDarkBrown, BLANK, false);
            
            // Sudut awal sisi lambung
            Vector2 bH[30]; int bIdx = 0;
            bH[bIdx++] = (Vector2){-110, 0}; bH[bIdx++] = (Vector2){-110, -10};
            Vector2 c2_p0 = {-110, -10}, c2_p1 = {-90, 45}, c2_p2 = {-40, 45};
            for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; bH[bIdx++] = (Vector2){inv*inv*c2_p0.x + 2.0f*inv*t*c2_p1.x + t*t*c2_p2.x, inv*inv*c2_p0.y + 2.0f*inv*t*c2_p1.y + t*t*c2_p2.y}; }
            bH[bIdx++] = (Vector2){20, 45};
            Vector2 c1_p0 = {20, 45}, c1_p1 = {80, 45}, c1_p2 = {120, -30};
            for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; bH[bIdx++] = (Vector2){inv*inv*c1_p0.x + 2.0f*inv*t*c1_p1.x + t*t*c1_p2.x, inv*inv*c1_p0.y + 2.0f*inv*t*c1_p1.y + t*t*c1_p2.y}; }
            bH[bIdx++] = (Vector2){95, 0}; bH[bIdx++] = (Vector2){45, 0}; bH[bIdx++] = (Vector2){-45, 0};
            
            Vector2 center = TransformPola(0, 15, x, y, s, angle);
            for(int i=0; i<bIdx; i++) {
                Vector2 p1 = TransformPola(bH[i].x, bH[i].y, x, y, s, angle);
                Vector2 p2 = TransformPola(bH[(i+1)%bIdx].x, bH[(i+1)%bIdx].y, x, y, s, angle);
                DrawTriangle(center, p1, p2, cDarkBrown);
                DrawTriangle(center, p2, p1, cDarkBrown);
            }
        }
        
        // Susunan garis luar kapal mode gambar kasar
        if (isWire) {
            Vector2 poly[40]; int pIdx = 0;
            poly[pIdx++] = (Vector2){-110, -25}; poly[pIdx++] = (Vector2){-80, -25}; poly[pIdx++] = (Vector2){-80, -15};
            poly[pIdx++] = (Vector2){-45, -15}; poly[pIdx++] = (Vector2){-45, 0}; poly[pIdx++] = (Vector2){45, 0};
            poly[pIdx++] = (Vector2){45, -15}; poly[pIdx++] = (Vector2){95, -15}; poly[pIdx++] = (Vector2){120, -30};
            Vector2 c1_p0 = (Vector2){120, -30}, c1_p1 = (Vector2){80, 45}, c1_p2 = (Vector2){20, 45};
            for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; poly[pIdx++] = (Vector2){inv*inv*c1_p0.x + 2.0f*inv*t*c1_p1.x + t*t*c1_p2.x, inv*inv*c1_p0.y + 2.0f*inv*t*c1_p1.y + t*t*c1_p2.y}; }
            poly[pIdx++] = (Vector2){-40, 45};
            Vector2 c2_p0 = (Vector2){-40, 45}, c2_p1 = (Vector2){-90, 45}, c2_p2 = (Vector2){-110, -10};
            for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; poly[pIdx++] = (Vector2){inv*inv*c2_p0.x + 2.0f*inv*t*c2_p1.x + t*t*c2_p2.x, inv*inv*c2_p0.y + 2.0f*inv*t*c2_p1.y + t*t*c2_p2.y}; }
            
            for (int i = 0; i < pIdx; i++) {
                Vector2 p1 = TransformPola(poly[i].x, poly[i].y, x, y, s, angle);
                Vector2 p2 = TransformPola(poly[(i+1)%pIdx].x, poly[(i+1)%pIdx].y, x, y, s, angle);
                AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
            }
        }

        // Memastikan garis hiasan terpotong mengikuti badan luar kapal
        DrawClippedStrip((Vector2){-120, 10}, (Vector2){0, 25}, (Vector2){120, 5}, x, y, s, angle, cMidBrown, isWire);
        DrawClippedStrip((Vector2){-120, 22}, (Vector2){0, 37}, (Vector2){120, 17}, x, y, s, angle, cMidBrown, isWire);

        // Menggambar lapisan bundar menjadi desain jendela yang baik.
        Color winIn = (Color){20, 10, 10, 255};
        Color outWin = isWire ? GREEN : cMidBrown; 
        DrawCustomCircle((Vector2){-70, 16}, 4.5f, x, y, s, angle, winIn, outWin, isWire);
        DrawCustomCircle((Vector2){-25, 16}, 4.5f, x, y, s, angle, winIn, outWin, isWire);
        DrawCustomCircle((Vector2){25, 16}, 4.5f, x, y, s, angle, winIn, outWin, isWire);
        DrawCustomCircle((Vector2){70, 16}, 4.5f, x, y, s, angle, winIn, outWin, isWire);

        // Menggambar tiupan angin pada badan layar lebar
        DrawPirateSail(-85, -115, 40, 25, cLightBrown, x, y, s, angle, sailFlap, isWire);
        DrawPirateSail(-85, -80,  50, 45, cLightBrown, x, y, s, angle, sailFlap, isWire);
        DrawPirateSail(75, -115,  40, 25, cLightBrown, x, y, s, angle, sailFlap, isWire);
        DrawPirateSail(75, -80,   50, 45, cLightBrown, x, y, s, angle, sailFlap, isWire);
        DrawPirateSail(0, -155,   50, 30, cLightBrown, x, y, s, angle, sailFlap, isWire);
        DrawPirateSail(0, -110,   80, 80, cBlackSail,  x, y, s, angle, sailFlap, isWire);
        
        // Menggambar tengkorak hiasan kapalnya
        float skullX = 0 + (sailFlap * 0.2f); float skullY = -70;
        Color outWhite = isWire ? GREEN : BLANK;
        DrawCustomCircle((Vector2){skullX, skullY - 4}, 8.0f, x, y, s, angle, cWhite, outWhite, isWire);
        Vector2 jaw[] = {{skullX - 4, skullY + 1}, {skullX + 4, skullY + 1}, {skullX + 4, skullY + 6}, {skullX - 4, skullY + 6}}; 
        DrawCustomPoly(jaw, 4, x, y, s, angle, cWhite, outWhite, isWire);
        
        DrawCustomCircle((Vector2){skullX - 3.5f, skullY - 4}, 2.5f, x, y, s, angle, cBlackSail, outWhite, isWire);
        DrawCustomCircle((Vector2){skullX + 3.5f, skullY - 4}, 2.5f, x, y, s, angle, cBlackSail, outWhite, isWire);
        
        // Lentera warna kuning dan tangkainya
        DrawCustomBezier((Vector2){skullX, skullY - 12}, (Vector2){skullX + 6, -90}, (Vector2){skullX + 14, -85}, x, y, s, angle, BLANK, isWire ? GREEN : cWhite, isWire);
        DrawCustomCircle((Vector2){skullX + 15, -84}, 2.0f, x, y, s, angle, cYellowWin, outWhite, isWire);
        
        // Bendera kapalnya dinaikkan
        Color outRedF = isWire ? GREEN : BLANK;
        Vector2 f1[] = {{-85, -145}, {-85.0f+28.0f, -145.0f + 6.0f + (sailFlap*0.5f)}, {-85, -145.0f + 16.0f}}; DrawCustomPoly(f1, 3, x, y, s, angle, cRedFlag, outRedF, isWire);
        Vector2 f2[] = {{0, -185}, {(0.0f+28.0f), -185.0f + 6.0f + (sailFlap*0.5f)}, {0, -185.0f + 16.0f}}; DrawCustomPoly(f2, 3, x, y, s, angle, cRedFlag, outRedF, isWire);
        Vector2 f3[] = {{75, -145}, {(75.0f+28.0f), -145.0f + 6.0f + (sailFlap*0.5f)}, {75, -145.0f + 16.0f}}; DrawCustomPoly(f3, 3, x, y, s, angle, cRedFlag, outRedF, isWire);

    } else if (variant == 1) {
        // Jenis tipe Galiung Kelas Besar
        Color cHullDark = (Color){45, 25, 15, 255}; 
        Color cHullLight = (Color){80, 40, 20, 255}; 
        Color cBlackSail = (Color){30, 30, 35, 255}; 
        Color cRedFlag = (Color){180, 30, 30, 255}; 
        Color cMast = (Color){30, 15, 10, 255}; 
        Color cRope = (Color){120, 100, 80, 255};
        Color outRope = isWire ? GREEN : cRope;
        Color outDB = isWire ? GREEN : BLANK;

        // Menggambar struktur tali temali silang
        DrawCustomLine((Vector2){170, -45}, (Vector2){60, -145}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){140, -25}, (Vector2){60, -105}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){60, -145}, (Vector2){0, -185}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){60, -115}, (Vector2){0, -140}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){0, -185}, (Vector2){-65, -135}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){0, -135}, (Vector2){-65, -95}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){-65, -115}, (Vector2){-100, -45}, x, y, s, angle, outRope);

        // Menggambar tata letak tiang atas
        Vector2 boom[] = {{105, -15}, {185, -50}, {183, -54}, {103, -19}}; DrawCustomPoly(boom, 4, x, y, s, angle, cMast, outDB, isWire);
        Vector2 mast1[] = {{-68, -145}, {-62, -145}, {-62, 5}, {-68, 5}}; DrawCustomPoly(mast1, 4, x, y, s, angle, cMast, outDB, isWire);
        Vector2 mast2[] = {{-4, -195}, {4, -195}, {4, 5}, {-4, 5}}; DrawCustomPoly(mast2, 4, x, y, s, angle, cMast, outDB, isWire);
        Vector2 mast3[] = {{57, -155}, {63, -155}, {63, 5}, {57, 5}}; DrawCustomPoly(mast3, 4, x, y, s, angle, cMast, outDB, isWire);

        // Menyusun koordinat garis terluar khusus kapal ini.
        Vector2 outH[40]; int oIdx = 0;
        outH[oIdx++] = (Vector2){-105, -45}; outH[oIdx++] = (Vector2){-50, -45}; outH[oIdx++] = (Vector2){-50, -20};
        outH[oIdx++] = (Vector2){110, -20};  outH[oIdx++] = (Vector2){145, -35}; outH[oIdx++] = (Vector2){130, -10};
        Vector2 oc1_p0 = {130, -10}, oc1_p1 = {80, 45}, oc1_p2 = {10, 45};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; outH[oIdx++] = (Vector2){inv*inv*oc1_p0.x + 2.0f*inv*t*oc1_p1.x + t*t*oc1_p2.x, inv*inv*oc1_p0.y + 2.0f*inv*t*oc1_p1.y + t*t*oc1_p2.y}; }
        outH[oIdx++] = (Vector2){-40, 45};
        Vector2 oc2_p0 = {-40, 45}, oc2_p1 = {-95, 45}, oc2_p2 = {-115, 0};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; outH[oIdx++] = (Vector2){inv*inv*oc2_p0.x + 2.0f*inv*t*oc2_p1.x + t*t*oc2_p2.x, inv*inv*oc2_p0.y + 2.0f*inv*t*oc2_p1.y + t*t*oc2_p2.y}; }

        if (!isWire) {
            Vector2 d1[] = {{-105, -45}, {-50, -45}, {-50, -20}, {-105, -20}}; DrawCustomPoly(d1, 4, x, y, s, angle, cHullDark, BLANK, false);
            Vector2 d2[] = {{-105, -20}, {110, -20}, {110, -10}, {-105, -10}}; DrawCustomPoly(d2, 4, x, y, s, angle, cHullDark, BLANK, false);
            Vector2 d3[] = {{110, -20}, {145, -35}, {130, -10}, {110, -10}}; DrawCustomPoly(d3, 4, x, y, s, angle, cHullDark, BLANK, false);
            Vector2 tail[] = {{-105, -45}, {-115, 0}, {-105, 0}}; DrawCustomPoly(tail, 3, x, y, s, angle, cHullDark, BLANK, false);
            
            Vector2 center = TransformPola(0, -10, x, y, s, angle);
            Vector2 bH[30]; int bIdx = 0;
            Vector2 c1_p0 = {130, -10}, c1_p1 = {80, 45}, c1_p2 = {10, 45};
            for(int i=0; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; bH[bIdx++] = (Vector2){inv*inv*c1_p0.x + 2.0f*inv*t*c1_p1.x + t*t*c1_p2.x, inv*inv*c1_p0.y + 2.0f*inv*t*c1_p1.y + t*t*c1_p2.y}; }
            bH[bIdx++] = (Vector2){-40, 45};
            Vector2 c2_p0 = {-40, 45}, c2_p1 = {-95, 45}, c2_p2 = {-115, 0};
            for(int i=0; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; bH[bIdx++] = (Vector2){inv*inv*c2_p0.x + 2.0f*inv*t*c2_p1.x + t*t*c2_p2.x, inv*inv*c2_p0.y + 2.0f*inv*t*c2_p1.y + t*t*c2_p2.y}; }
            bH[bIdx++] = (Vector2){-105, -10}; bH[bIdx++] = (Vector2){130, -10};
            for(int i=0; i<bIdx-1; i++) {
                Vector2 p1 = TransformPola(bH[i].x, bH[i].y, x, y, s, angle);
                Vector2 p2 = TransformPola(bH[i+1].x, bH[i+1].y, x, y, s, angle);
                DrawTriangle(center, p1, p2, cHullDark); DrawTriangle(center, p2, p1, cHullDark);
            }

            for (float py = -8; py <= 8; py += 1.0f) {
                for (float px = -120; px <= 140; px += 1.0f) {
                    Vector2 pTest = {px, py};
                    bool inside = false;
                    for (int i = 0, j = oIdx - 1; i < oIdx; j = i++) {
                        if (((outH[i].y > pTest.y) != (outH[j].y > pTest.y)) && (pTest.x < (outH[j].x - outH[i].x) * (pTest.y - outH[i].y) / (outH[j].y - outH[i].y) + outH[i].x)) { inside = !inside; }
                    }
                    if (inside) {
                        Color col = cHullLight;
                        if (py >= -4.0f && py <= 4.0f) {
                            for (int i=0; i<8; i++) { float bx = -105.0f + i * 28.0f; if (px >= bx && px <= bx + 12.0f) { col = cHullDark; break; } }
                        }
                        Vector2 curr = TransformPola(px, py, x, y, s, angle); Vector2 next = TransformPola(px + 1.5f, py, x, y, s, angle);
                        AlgoLine((int)curr.x, (int)curr.y, (int)next.x, (int)next.y, col);
                    }
                }
            }
        } else {
            for(int i=0; i<oIdx; i++) {
                Vector2 p1 = TransformPola(outH[i].x, outH[i].y, x, y, s, angle); Vector2 p2 = TransformPola(outH[(i+1)%oIdx].x, outH[(i+1)%oIdx].y, x, y, s, angle);
                AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
            }
            
            for (float px = -120; px <= 140; px += 2.0f) {
                Vector2 top = {px, -8}, bot = {px, 8}; bool inTop = false, inBot = false;
                for (int i = 0, j = oIdx - 1; i < oIdx; j = i++) {
                    if (((outH[i].y > top.y) != (outH[j].y > top.y)) && (top.x < (outH[j].x - outH[i].x) * (top.y - outH[i].y) / (outH[j].y - outH[i].y) + outH[i].x)) inTop = !inTop;
                    if (((outH[i].y > bot.y) != (outH[j].y > bot.y)) && (bot.x < (outH[j].x - outH[i].x) * (bot.y - outH[i].y) / (outH[j].y - outH[i].y) + outH[i].x)) inBot = !inBot;
                }
                if (inTop) { Vector2 p1 = TransformPola(px, -8, x, y, s, angle); Vector2 p2 = TransformPola(px+2, -8, x, y, s, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
                if (inBot) { Vector2 p1 = TransformPola(px, 8, x, y, s, angle); Vector2 p2 = TransformPola(px+2, 8, x, y, s, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
            }

            for (int i=0; i<8; i++) {
                float bx = -105.0f + i * 28.0f; Vector2 bPts[4] = {{bx, -4}, {bx+12, -4}, {bx+12, 4}, {bx, 4}}; bool inHole = false; Vector2 tp = {bx+6, 0};
                for (int k = 0, j = oIdx - 1; k < oIdx; j = k++) {
                    if (((outH[k].y > tp.y) != (outH[j].y > tp.y)) && (tp.x < (outH[j].x - outH[k].x) * (tp.y - outH[k].y) / (outH[j].y - outH[k].y) + outH[k].x)) inHole = !inHole;
                }
                if(inHole) {
                    for(int j=0; j<4; j++) {
                        Vector2 p1 = TransformPola(bPts[j].x, bPts[j].y, x, y, s, angle); Vector2 p2 = TransformPola(bPts[(j+1)%4].x, bPts[(j+1)%4].y, x, y, s, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
                    }
                }
            }
        }

        Color outRedF = isWire ? GREEN : BLANK;
        Vector2 f1[] = {{-65, -145}, {-65.0f-22.0f, -145.0f + 4.0f + (sailFlap*0.2f)}, {-65, -145.0f + 10.0f}}; DrawCustomPoly(f1, 3, x, y, s, angle, cRedFlag, outRedF, isWire);
        Vector2 f2[] = {{0, -195}, {0.0f-22.0f, -195.0f + 4.0f + (sailFlap*0.2f)}, {0, -195.0f + 10.0f}}; DrawCustomPoly(f2, 3, x, y, s, angle, cRedFlag, outRedF, isWire);
        Vector2 f3[] = {{60, -155}, {60.0f-22.0f, -155.0f + 4.0f + (sailFlap*0.2f)}, {60, -155.0f + 10.0f}}; DrawCustomPoly(f3, 3, x, y, s, angle, cRedFlag, outRedF, isWire);

        DrawGalleonSail(60, -140, 25, 20, cBlackSail, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(60, -110, 40, 30, cBlackSail, x, y, s, angle, sailFlap, isWire);
        DrawGalleonSail(60, -70,  50, 45, cBlackSail, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(0, -180,  30, 25, cBlackSail, x, y, s, angle, sailFlap, isWire);
        DrawGalleonSail(0, -145,  50, 40, cBlackSail, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(0, -95,   65, 55, cBlackSail, x, y, s, angle, sailFlap, isWire);
        DrawGalleonSail(-65, -135, 25, 20, cBlackSail, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(-65, -105, 35, 30, cBlackSail, x, y, s, angle, sailFlap, isWire);

    } else if (variant == 2) {
        // Jenis tipe Kapal Jong Timur Lanjut
        float sf = s * 0.8f; 
        Color cHull = (Color){50, 30, 20, 255}; 
        Color cWinDark = (Color){20, 10, 5, 255}; 
        Color cBlack = (Color){30, 30, 35, 255}; 
        Color cRedSail = (Color){160, 25, 25, 255}; 
        Color cMast = (Color){40, 20, 10, 255}; 
        Color cRope = (Color){130, 110, 90, 255}; 
        Color outRope = isWire ? GREEN : cRope;
        Color outDB = isWire ? GREEN : BLANK;
        
        float m1 = -75; float m2 = 15; float m3 = 105;

        // 1. Tali Temali
        DrawCustomLine((Vector2){-190, -65}, (Vector2){m1, -150}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m1, -150},  (Vector2){m2, -200}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m1, -110},  (Vector2){m2, -150}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m2, -200},  (Vector2){m3, -165}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m2, -150},  (Vector2){m3, -125}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m3, -165},  (Vector2){300, -48}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m3, -125},  (Vector2){230, -35}, x, y, sf, angle, outRope);

        // 2. Boom & Tiang (Mast)
        Vector2 boom[] = {{150, -20}, {310, -50}, {308, -56}, {148, -26}}; 
        DrawCustomPoly(boom, 4, x, y, sf, angle, cMast, outDB, isWire);
        
        DrawCustomRect(m1 - 3.0f, -170.0f, 6.0f, 150.0f, x, y, sf, angle, cMast, outDB, isWire);
        DrawCustomRect(m2 - 4.0f, -220.0f, 8.0f, 200.0f, x, y, sf, angle, cMast, outDB, isWire);
        DrawCustomRect(m3 - 3.0f, -185.0f, 6.0f, 165.0f, x, y, sf, angle, cMast, outDB, isWire);

        // 3. Lambung Kapal
        Vector2 curvePts[20]; int cpIdx = 0;
        Vector2 curP0 = {230, -10}, curP1 = {140, 45}, curP2 = {20, 45};
        curvePts[cpIdx++] = curP0;
        for(int i=1; i<=10; i++) {
            float t = i/10.0f; float inv = 1.0f - t;
            curvePts[cpIdx++] = (Vector2){inv*inv*curP0.x + 2.0f*inv*t*curP1.x + t*t*curP2.x, inv*inv*curP0.y + 2.0f*inv*t*curP1.y + t*t*curP2.y};
        }

        if (!isWire) {
            // Menghitung pemotongan warna kapal segitiga manual
            Vector2 tA = TransformPola(-190, -65, x, y, sf, angle);
            Vector2 tB = TransformPola(-140, -65, x, y, sf, angle);
            Vector2 tC = TransformPola(-140, -20, x, y, sf, angle); // Sudut Lantai Kiri
            Vector2 tD = TransformPola(150, -20, x, y, sf, angle);  // Sudut Lantai Kanan
            Vector2 tE = TransformPola(260, -40, x, y, sf, angle);
            Vector2 tF = TransformPola(230, -10, x, y, sf, angle);
            Vector2 tG = TransformPola(20, 45, x, y, sf, angle);
            Vector2 tH = TransformPola(-150, 45, x, y, sf, angle);

            // Bagian 1: Ekor belakang kapal
            DrawTriangle(tA, tB, tC, cHull); DrawTriangle(tA, tC, tB, cHull);
            DrawTriangle(tA, tC, tH, cHull); DrawTriangle(tA, tH, tC, cHull);

            // Bagian 2: Ruangan perut di lantai tengah
            DrawTriangle(tC, tD, tG, cHull); DrawTriangle(tC, tG, tD, cHull);
            DrawTriangle(tC, tG, tH, cHull); DrawTriangle(tC, tH, tG, cHull);

            // Bagian 3: Kurva bagian moncong kapal bawah dek
            DrawTriangle(tD, tE, tF, cHull); DrawTriangle(tD, tF, tE, cHull);
            for(int i=0; i<cpIdx-1; i++) {
                Vector2 pt1 = TransformPola(curvePts[i].x, curvePts[i].y, x, y, sf, angle);
                Vector2 pt2 = TransformPola(curvePts[i+1].x, curvePts[i+1].y, x, y, sf, angle);
                DrawTriangle(tD, pt1, pt2, cHull);
                DrawTriangle(tD, pt2, pt1, cHull);
            }
        } else {
            // Garis kasar kerangkanya jika aktif khusus layar ini
            Vector2 junkHull[40]; int jhIdx = 0;
            junkHull[jhIdx++] = (Vector2){-190, -65};
            junkHull[jhIdx++] = (Vector2){-140, -65};
            junkHull[jhIdx++] = (Vector2){-140, -20};
            junkHull[jhIdx++] = (Vector2){150, -20};
            junkHull[jhIdx++] = (Vector2){260, -40};
            for(int i=0; i<cpIdx; i++) junkHull[jhIdx++] = curvePts[i];
            junkHull[jhIdx++] = (Vector2){-150, 45};

            for(int i=0; i<jhIdx; i++) {
                Vector2 p1 = TransformPola(junkHull[i].x, junkHull[i].y, x, y, sf, angle);
                Vector2 p2 = TransformPola(junkHull[(i+1)%jhIdx].x, junkHull[(i+1)%jhIdx].y, x, y, sf, angle);
                AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
            }
        }

        // 4. Jendela Kotak
        Color outWin = isWire ? GREEN : BLANK;
        for(int r=0; r<3; r++) {
            for(int c=0; c<3; c++) {
                DrawCustomRect(-180.0f + c*12.0f, -58.0f + r*10.0f, 6.0f, 6.0f, x, y, sf, angle, cWinDark, outWin, isWire);
            }
        }
        for(int i=0; i<13; i++) {
            DrawCustomRect(-125.0f + i*22.0f, -10.0f, 8.0f, 6.0f, x, y, sf, angle, cWinDark, outWin, isWire);
        }

        // 5. Square Sails
        DrawGalleonSail(m1, -160, 40, 25, cBlack,   x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m1, -125, 55, 35, cBlack,   x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m1, -80,  70, 45, cBlack,   x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m2, -210, 45, 30, cRedSail, x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m2, -165, 65, 45, cRedSail, x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m2, -105, 85, 60, cRedSail, x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m3, -175, 40, 25, cBlack,   x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m3, -140, 55, 35, cBlack,   x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m3, -95,  70, 45, cBlack,   x, y, sf, angle, sailFlap, isWire);

        // 6. Front Jib Sail 
        Vector2 jib[30]; int jIdx = 0;
        jib[jIdx++] = (Vector2){m3, -135}; jib[jIdx++] = (Vector2){280, -46};
        Vector2 j1_p0 = {280, -46}, j1_p1 = {230, -40}, j1_p2 = {180, -32};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; jib[jIdx++] = (Vector2){inv*inv*j1_p0.x + 2.0f*inv*t*j1_p1.x + t*t*j1_p2.x, inv*inv*j1_p0.y + 2.0f*inv*t*j1_p1.y + t*t*j1_p2.y}; }
        Vector2 j2_p0 = {180, -32}, j2_p1 = {155 - sailFlap, -85}, j2_p2 = {m3, -135};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; jib[jIdx++] = (Vector2){inv*inv*j2_p0.x + 2.0f*inv*t*j2_p1.x + t*t*j2_p2.x, inv*inv*j2_p0.y + 2.0f*inv*t*j2_p1.y + t*t*j2_p2.y}; }
        
        Color outJib = isWire ? GREEN : BLANK;
        DrawCustomPoly(jib, jIdx, x, y, sf, angle, cBlack, outJib, isWire);
    }
}

/**
 * @brief Manajer Variasi Kapal Sloop Bertiang Tunggal (Sloop Category Variant Manager).
 * 
 * Mengeksekusi instruksi perenderan kepada tiga skema warna:
 * (0) Merah-Putih standar, (1) Merah-Deck Polos, (2) Kuning Terang Eksotis.
 */
void DrawShip_Sloop_Variant(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire) {
    float s = scale * 0.8f; 
    // Sembunyikan warna putih agar menyatu dengan baik pada background laut.
    Color outline = isWire ? GREEN : BLANK; 
    
    if (variant == 0) {
        Color cWhite = {248, 248, 250, 255}; Color cSailShadow = {230, 230, 235, 255}; Color cGreen = {46, 172, 109, 255};  
        Color cBrownHull = {82, 54, 52, 255}; Color cRedHull = {226, 44, 44, 255}; Color cMast = {70, 45, 40, 255};    
        Vector2 mast[] = {{7, -155}, {13, -155}, {13, -5}, {7, -5}}; DrawCustomPoly(mast, 4, x, y, s, angle, cMast, outline, isWire);
        Vector2 bsTri[] = {{5, -145}, {5, -25}, {-75, -25}}; DrawCustomPoly(bsTri, 3, x, y, s, angle, cWhite, outline, isWire);
        DrawCustomBezier((Vector2){-75, -25}, (Vector2){-40 + sailFlap*0.8f, -85}, (Vector2){5, -145}, x, y, s, angle, cWhite, outline, isWire);
        Vector2 bsShadow[] = {{5, -145}, {-30, -25}, {5, -25}}; DrawCustomPoly(bsShadow, 3, x, y, s, angle, cSailShadow, outline, isWire);
        Vector2 fsTri[] = {{15, -135}, {20, -10}, {82, -10}}; DrawCustomPoly(fsTri, 3, x, y, s, angle, cWhite, outline, isWire);
        DrawCustomBezier((Vector2){82, -10}, (Vector2){55 + sailFlap*0.8f, -65}, (Vector2){15, -135}, x, y, s, angle, cWhite, outline, isWire);
        Vector2 boom[] = {{13, -27.5f}, {-85, -27.5f}, {-85, -22.5f}, {13, -22.5f}}; DrawCustomPoly(boom, 4, x, y, s, angle, cMast, outline, isWire);
        Vector2 hTop[] = {{-75, -5}, {85, -5}, {75, 12}, {-65, 12}}; DrawCustomPoly(hTop, 4, x, y, s, angle, cWhite, outline, isWire);
        Vector2 hMid[] = {{-65, 12}, {75, 12}, {65, 24}, {-55, 24}}; DrawCustomPoly(hMid, 4, x, y, s, angle, cBrownHull, outline, isWire);
        Vector2 hBot[] = {{-55, 24}, {65, 24}, {55, 36}, {-45, 36}}; DrawCustomPoly(hBot, 4, x, y, s, angle, cRedHull, outline, isWire);
        Vector2 trim[] = {{-80, -7.5f}, {90, -7.5f}, {90, -2.5f}, {-80, -2.5f}}; DrawCustomPoly(trim, 4, x, y, s, angle, cGreen, outline, isWire);
    } else if (variant == 1) {
        Color cWhite = {250, 250, 255, 255}; Color cSailShadow = {235, 230, 235, 255}; Color cRedDeck = {220, 40, 40, 255}; Color cMast = {20, 30, 40, 255};
        Vector2 mast[] = {{15, -165}, {20, -165}, {20, -5}, {15, -5}}; DrawCustomPoly(mast, 4, x, y, s, angle, cMast, outline, isWire);
        Vector2 mTop[] = {{14, -167}, {17.5f, -170.5f}, {21, -167}, {17.5f, -163.5f}}; DrawCustomPoly(mTop, 4, x, y, s, angle, cRedDeck, outline, isWire);
        Vector2 fsTri[] = {{20, -145}, {25, -15}, {82, -15}}; DrawCustomPoly(fsTri, 3, x, y, s, angle, cWhite, outline, isWire);
        DrawCustomBezier((Vector2){82, -15}, (Vector2){55 + sailFlap*0.8f, -65}, (Vector2){20, -145}, x, y, s, angle, cWhite, outline, isWire);
        Vector2 bsTri[] = {{10, -155}, {10, -25}, {-75, -25}}; DrawCustomPoly(bsTri, 3, x, y, s, angle, cWhite, outline, isWire);
        DrawCustomBezier((Vector2){-75, -25}, (Vector2){-40 + sailFlap*0.8f, -85}, (Vector2){10, -155}, x, y, s, angle, cWhite, outline, isWire);
        Vector2 bsShadow[] = {{10, -155}, {-30, -25}, {10, -25}}; DrawCustomPoly(bsShadow, 3, x, y, s, angle, cSailShadow, outline, isWire);
        Vector2 boom[] = {{15, -27}, {-85, -27}, {-85, -23}, {15, -23}}; DrawCustomPoly(boom, 4, x, y, s, angle, cMast, outline, isWire);
        Vector2 hull[] = {{-70, -10}, {85, -10}, {70, 10}, {-55, 10}}; DrawCustomPoly(hull, 4, x, y, s, angle, cWhite, outline, isWire);
        Vector2 trim[] = {{-73, -13}, {88, -13}, {88, -7}, {-73, -7}}; DrawCustomPoly(trim, 4, x, y, s, angle, cRedDeck, outline, isWire);
    } else {
        Color cYellowHull = {255, 210, 40, 255}; Color cDarkYellow = {220, 160, 20, 255}; Color cWhiteSail  = {250, 250, 255, 255};
        Color cShadowSail = {210, 230, 250, 255}; Color cBlueWindow = {30, 130, 200, 255}; Color cMastBrown  = {160, 90, 40, 255}; Color cRedFlag = {255, 40, 40, 255};
        Vector2 mast[] = {{10, -155}, {16, -155}, {16, -5}, {10, -5}}; DrawCustomPoly(mast, 4, x, y, s, angle, cMastBrown, outline, isWire);
        Vector2 flag[] = {{16, -155}, {38, -155}, {38, -143}, {16, -143}}; DrawCustomPoly(flag, 4, x, y, s, angle, cRedFlag, outline, isWire);
        Vector2 bsTri[] = {{8, -145}, {8, -25}, {-75, -25}}; DrawCustomPoly(bsTri, 3, x, y, s, angle, cWhiteSail, outline, isWire);
        DrawCustomBezier((Vector2){-75, -25}, (Vector2){-40 + sailFlap*0.8f, -85}, (Vector2){8, -145}, x, y, s, angle, cWhiteSail, outline, isWire);
        Vector2 bsShadow[] = {{8, -145}, {-25, -25}, {8, -25}}; DrawCustomPoly(bsShadow, 3, x, y, s, angle, cShadowSail, outline, isWire);
        Vector2 fsTri[] = {{18, -140}, {22, -15}, {80, -15}}; DrawCustomPoly(fsTri, 3, x, y, s, angle, cShadowSail, outline, isWire);
        DrawCustomBezier((Vector2){80, -15}, (Vector2){55 + sailFlap*0.8f, -80}, (Vector2){18, -140}, x, y, s, angle, cShadowSail, outline, isWire);
        Vector2 fsHigh[] = {{18, -140}, {22, -15}, {45, -15}}; DrawCustomPoly(fsHigh, 3, x, y, s, angle, cWhiteSail, outline, isWire);
        if (!isWire) {
            Vector2 p0 = TransformPola(-70, -20, x, y, s, angle); Vector2 p1 = TransformPola(-20, -20, x, y, s, angle); Vector2 p2 = TransformPola(-10, -5, x, y, s, angle);
            Vector2 p3 = TransformPola(40, -5, x, y, s, angle); Vector2 p4 = TransformPola(50, -15, x, y, s, angle); Vector2 p5 = TransformPola(85, -15, x, y, s, angle);
            Vector2 p6 = TransformPola(65, 15, x, y, s, angle); Vector2 p7 = TransformPola(-50, 15, x, y, s, angle);
            DrawTriangle(p0, p1, p7, cYellowHull); DrawTriangle(p0, p7, p1, cYellowHull); DrawTriangle(p1, p2, p7, cYellowHull); DrawTriangle(p1, p7, p2, cYellowHull);
            DrawTriangle(p2, p6, p7, cYellowHull); DrawTriangle(p2, p7, p6, cYellowHull); DrawTriangle(p2, p3, p6, cYellowHull); DrawTriangle(p2, p6, p3, cYellowHull);
            DrawTriangle(p3, p4, p6, cYellowHull); DrawTriangle(p3, p6, p4, cYellowHull); DrawTriangle(p4, p5, p6, cYellowHull); DrawTriangle(p4, p6, p5, cYellowHull);
        }
        Vector2 hullTop[] = {{-70, -20}, {-20, -20}, {-10, -5}, {40, -5}, {50, -15}, {85, -15}, {65, 15}, {-50, 15}}; DrawCustomPoly(hullTop, 8, x, y, s, angle, BLANK, outline, isWire);
        Vector2 hullBot[] = {{-50, 15}, {65, 15}, {60, 22}, {-45, 22}}; DrawCustomPoly(hullBot, 4, x, y, s, angle, cDarkYellow, outline, isWire);
        Vector2 w1[] = {{-62, -16}, {-44, -16}, {-40, -8}, {-58, -8}}; DrawCustomPoly(w1, 4, x, y, s, angle, cBlueWindow, outline, isWire);
        Vector2 w2[] = {{-38, -16}, {-24, -16}, {-20, -8}, {-34, -8}}; DrawCustomPoly(w2, 4, x, y, s, angle, cBlueWindow, outline, isWire);
    }
}

/**
 * @brief Perender independen Galiung Kargo (Merchant Galleon / Junk).
 * 
 * Melakukan delegasi rutinitas perenderan statis (tanpa manipulasi tengkorak) 
 * kepada arsitektur perahu layar dagang besar tipe Eropa maupun lambung Junk wilayah Timur.
 */


void DrawShip_Galleon(float x, float y, float scale, float angle, float sailFlap, int variant, bool isWire) {
    if (variant == 0) {
        // Model Klasik Kapal Utama Emas Layar Putih Standar.
        float s = scale * 0.65f;
        Color cHullDark  = (Color){95, 45, 15, 255}; 
        Color cHullLight = (Color){170, 75, 30, 255}; 
        Color cGold      = (Color){240, 195, 60, 255}; 
        Color cRed       = (Color){210, 40, 40, 255}; 
        Color cMast      = (Color){90, 45, 25, 255}; 
        Color cRope      = (Color){200, 180, 140, 255};
        Color outRope    = isWire ? GREEN : cRope;
        Color outDB      = isWire ? GREEN : BLANK;

        // 1. Tali Temali
        DrawCustomLine((Vector2){170, -45}, (Vector2){60, -145}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){140, -25}, (Vector2){60, -105}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){60, -145}, (Vector2){0, -185}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){60, -115}, (Vector2){0, -140}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){0, -185}, (Vector2){-65, -135}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){0, -135}, (Vector2){-65, -95}, x, y, s, angle, outRope);
        DrawCustomLine((Vector2){-65, -115}, (Vector2){-100, -45}, x, y, s, angle, outRope);

        // 2. Boom & Tiang Utama
        Vector2 boom[] = {{105, -15}, {185, -50}, {183, -54}, {103, -19}}; DrawCustomPoly(boom, 4, x, y, s, angle, cMast, outDB, isWire);
        DrawCustomRect(-68, -145, 6, 150, x, y, s, angle, cMast, outDB, isWire);
        DrawCustomRect(-4, -195, 8, 200, x, y, s, angle, cMast, outDB, isWire);
        DrawCustomRect(57, -155, 6, 160, x, y, s, angle, cMast, outDB, isWire);

        // 3. Outline Lambung Galiung
        Vector2 outH[40]; int oIdx = 0;
        outH[oIdx++] = (Vector2){-105, -45}; outH[oIdx++] = (Vector2){-50, -45}; outH[oIdx++] = (Vector2){-50, -20};
        outH[oIdx++] = (Vector2){110, -20};  outH[oIdx++] = (Vector2){145, -35}; outH[oIdx++] = (Vector2){130, -10};
        Vector2 oc1_p0 = {130, -10}, oc1_p1 = {80, 45}, oc1_p2 = {10, 45};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; outH[oIdx++] = (Vector2){inv*inv*oc1_p0.x + 2.0f*inv*t*oc1_p1.x + t*t*oc1_p2.x, inv*inv*oc1_p0.y + 2.0f*inv*t*oc1_p1.y + t*t*oc1_p2.y}; }
        outH[oIdx++] = (Vector2){-40, 45};
        Vector2 oc2_p0 = {-40, 45}, oc2_p1 = {-95, 45}, oc2_p2 = {-115, 0};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; outH[oIdx++] = (Vector2){inv*inv*oc2_p0.x + 2.0f*inv*t*oc2_p1.x + t*t*oc2_p2.x, inv*inv*oc2_p0.y + 2.0f*inv*t*oc2_p1.y + t*t*oc2_p2.y}; }

        // 4. Mewarnai kapal bagian lambungnya.
        if (!isWire) {
            Vector2 d1[] = {{-105, -45}, {-50, -45}, {-50, -20}, {-105, -20}}; DrawCustomPoly(d1, 4, x, y, s, angle, cHullDark, BLANK, false);
            Vector2 d2[] = {{-105, -20}, {110, -20}, {110, -10}, {-105, -10}}; DrawCustomPoly(d2, 4, x, y, s, angle, cHullDark, BLANK, false);
            Vector2 d3[] = {{110, -20}, {145, -35}, {130, -10}, {110, -10}}; DrawCustomPoly(d3, 4, x, y, s, angle, cHullDark, BLANK, false);
            Vector2 tail[] = {{-105, -45}, {-115, 0}, {-105, 0}}; DrawCustomPoly(tail, 3, x, y, s, angle, cHullDark, BLANK, false);
            
            Vector2 center = TransformPola(0, -10, x, y, s, angle);
            Vector2 bH[30]; int bIdx = 0;
            Vector2 c1_p0 = {130, -10}, c1_p1 = {80, 45}, c1_p2 = {10, 45};
            for(int i=0; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; bH[bIdx++] = (Vector2){inv*inv*c1_p0.x + 2.0f*inv*t*c1_p1.x + t*t*c1_p2.x, inv*inv*c1_p0.y + 2.0f*inv*t*c1_p1.y + t*t*c1_p2.y}; }
            bH[bIdx++] = (Vector2){-40, 45};
            Vector2 c2_p0 = {-40, 45}, c2_p1 = {-95, 45}, c2_p2 = {-115, 0};
            for(int i=0; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; bH[bIdx++] = (Vector2){inv*inv*c2_p0.x + 2.0f*inv*t*c2_p1.x + t*t*c2_p2.x, inv*inv*c2_p0.y + 2.0f*inv*t*c2_p1.y + t*t*c2_p2.y}; }
            bH[bIdx++] = (Vector2){-105, -10}; bH[bIdx++] = (Vector2){130, -10};
            for(int i=0; i<bIdx-1; i++) {
                Vector2 p1 = TransformPola(bH[i].x, bH[i].y, x, y, s, angle); Vector2 p2 = TransformPola(bH[i+1].x, bH[i+1].y, x, y, s, angle);
                DrawTriangle(center, p1, p2, cHullDark); DrawTriangle(center, p2, p1, cHullDark);
            }

            // Menghitung tempat yang perlu menaruh kayu secara logis hit-test polygon
            for (float py = -8; py <= 8; py += 1.0f) {
                for (float px = -130; px <= 150; px += 1.0f) {
                    Vector2 pTest = {px, py}; bool inside = false;
                    for (int i = 0, j = oIdx - 1; i < oIdx; j = i++) { if (((outH[i].y > pTest.y) != (outH[j].y > pTest.y)) && (pTest.x < (outH[j].x - outH[i].x) * (pTest.y - outH[i].y) / (outH[j].y - outH[i].y) + outH[i].x)) { inside = !inside; } }
                    if (inside) {
                        Color col = cHullLight;
                        if (py >= -4.0f && py <= 4.0f) { for (int i=0; i<8; i++) { float bx = -105.0f + i * 28.0f; if (px >= bx && px <= bx + 12.0f) { col = cHullDark; break; } } }
                        Vector2 curr = TransformPola(px, py, x, y, s, angle); Vector2 next = TransformPola(px + 1.5f, py, x, y, s, angle);
                        AlgoLine((int)curr.x, (int)curr.y, (int)next.x, (int)next.y, col);
                    }
                }
            }
        } else {
            for(int i=0; i<oIdx; i++) {
                Vector2 p1 = TransformPola(outH[i].x, outH[i].y, x, y, s, angle); Vector2 p2 = TransformPola(outH[(i+1)%oIdx].x, outH[(i+1)%oIdx].y, x, y, s, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN);
            }
            for (float px = -130; px <= 150; px += 2.0f) {
                Vector2 top = {px, -8}, bot = {px, 8}; bool inTop = false, inBot = false;
                for (int i = 0, j = oIdx - 1; i < oIdx; j = i++) {
                    if (((outH[i].y > top.y) != (outH[j].y > top.y)) && (top.x < (outH[j].x - outH[i].x) * (top.y - outH[i].y) / (outH[j].y - outH[i].y) + outH[i].x)) inTop = !inTop;
                    if (((outH[i].y > bot.y) != (outH[j].y > bot.y)) && (bot.x < (outH[j].x - outH[i].x) * (bot.y - outH[i].y) / (outH[j].y - outH[i].y) + outH[i].x)) inBot = !inBot;
                }
                if (inTop) { Vector2 p1 = TransformPola(px, -8, x, y, s, angle); Vector2 p2 = TransformPola(px+2, -8, x, y, s, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
                if (inBot) { Vector2 p1 = TransformPola(px, 8, x, y, s, angle); Vector2 p2 = TransformPola(px+2, 8, x, y, s, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
            }
            for (int i=0; i<8; i++) {
                float bx = -105.0f + i * 28.0f; Vector2 bPts[4] = {{bx, -4}, {bx+12, -4}, {bx+12, 4}, {bx, 4}}; bool inHole = false; Vector2 tp = {bx+6, 0};
                for (int k = 0, j = oIdx - 1; k < oIdx; j = k++) { if (((outH[k].y > tp.y) != (outH[j].y > tp.y)) && (tp.x < (outH[j].x - outH[k].x) * (tp.y - outH[k].y) / (outH[j].y - outH[k].y) + outH[k].x)) inHole = !inHole; }
                if(inHole) { for(int j=0; j<4; j++) { Vector2 p1 = TransformPola(bPts[j].x, bPts[j].y, x, y, s, angle); Vector2 p2 = TransformPola(bPts[(j+1)%4].x, bPts[(j+1)%4].y, x, y, s, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); } }
            }
        }

        // 5. Bendera Merah
        Color outRedF = isWire ? GREEN : BLANK;
        Vector2 f1[] = {{-65, -145}, {-65.0f-22.0f, -145.0f + 4.0f + (sailFlap*0.2f)}, {-65, -145.0f + 10.0f}}; DrawCustomPoly(f1, 3, x, y, s, angle, cRed, outRedF, isWire);
        Vector2 f2[] = {{0, -195}, {0.0f-22.0f, -195.0f + 4.0f + (sailFlap*0.2f)}, {0, -195.0f + 10.0f}}; DrawCustomPoly(f2, 3, x, y, s, angle, cRed, outRedF, isWire);
        Vector2 f3[] = {{60, -155}, {60.0f-22.0f, -155.0f + 4.0f + (sailFlap*0.2f)}, {60, -155.0f + 10.0f}}; DrawCustomPoly(f3, 3, x, y, s, angle, cRed, outRedF, isWire);

        // 6. Layar Emas Menggelembung
        DrawGalleonSail(60, -140, 25, 20, cGold, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(60, -110, 40, 30, cGold, x, y, s, angle, sailFlap, isWire);
        DrawGalleonSail(60, -70,  50, 45, cGold, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(0, -180,  30, 25, cGold, x, y, s, angle, sailFlap, isWire);
        DrawGalleonSail(0, -145,  50, 40, cGold, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(0, -95,   65, 55, cGold, x, y, s, angle, sailFlap, isWire);
        DrawGalleonSail(-65, -135, 25, 20, cGold, x, y, s, angle, sailFlap, isWire); DrawGalleonSail(-65, -105, 35, 30, cGold, x, y, s, angle, sailFlap, isWire);

    } else if (variant == 1) {
        // Bodi Kapal Timur dengan model cat lambung layar lebar.
        float sf = scale * 0.52f; // 0.65 * 0.8 (Skala gabungan)
        Color cHull    = (Color){140, 95, 60, 255}; 
        Color cWinDark = (Color){65, 45, 30, 255}; 
        Color cWhite   = (Color){250, 252, 255, 255}; 
        Color cMast    = (Color){100, 60, 35, 255}; 
        Color cRope    = (Color){200, 180, 140, 255}; 
        Color outRope  = isWire ? GREEN : cRope;
        Color outDB    = isWire ? GREEN : BLANK;
        
        float m1 = -75; float m2 = 15; float m3 = 105;

        // 1. Tali Temali
        DrawCustomLine((Vector2){-190, -65}, (Vector2){m1, -150}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m1, -150},  (Vector2){m2, -200}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m1, -110},  (Vector2){m2, -150}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m2, -200},  (Vector2){m3, -165}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m2, -150},  (Vector2){m3, -125}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m3, -165},  (Vector2){300, -48}, x, y, sf, angle, outRope);
        DrawCustomLine((Vector2){m3, -125},  (Vector2){230, -35}, x, y, sf, angle, outRope);

        // 2. Boom & Tiang (Mast) Simetris
        Vector2 boom[] = {{150, -20}, {310, -50}, {308, -56}, {148, -26}}; 
        DrawCustomPoly(boom, 4, x, y, sf, angle, cMast, outDB, isWire);
        
        DrawCustomRect(m1 - 3.0f, -170.0f, 6.0f, 150.0f, x, y, sf, angle, cMast, outDB, isWire);
        DrawCustomRect(m2 - 4.0f, -220.0f, 8.0f, 200.0f, x, y, sf, angle, cMast, outDB, isWire);
        DrawCustomRect(m3 - 3.0f, -185.0f, 6.0f, 165.0f, x, y, sf, angle, cMast, outDB, isWire);

        // 3. Lambung Kapal Dipecah Anti Bocor
        Vector2 curvePts[20]; int cpIdx = 0;
        Vector2 curP0 = {230, -10}, curP1 = {140, 45}, curP2 = {20, 45};
        curvePts[cpIdx++] = curP0;
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; curvePts[cpIdx++] = (Vector2){inv*inv*curP0.x + 2.0f*inv*t*curP1.x + t*t*curP2.x, inv*inv*curP0.y + 2.0f*inv*t*curP1.y + t*t*curP2.y}; }

        if (!isWire) {
            Vector2 tA = TransformPola(-190, -65, x, y, sf, angle); Vector2 tB = TransformPola(-140, -65, x, y, sf, angle);
            Vector2 tC = TransformPola(-140, -20, x, y, sf, angle); Vector2 tD = TransformPola(150, -20, x, y, sf, angle); 
            Vector2 tE = TransformPola(260, -40, x, y, sf, angle);  Vector2 tF = TransformPola(230, -10, x, y, sf, angle);
            Vector2 tG = TransformPola(20, 45, x, y, sf, angle);    Vector2 tH = TransformPola(-150, 45, x, y, sf, angle);
            DrawTriangle(tA, tB, tC, cHull); DrawTriangle(tA, tC, tB, cHull); DrawTriangle(tA, tC, tH, cHull); DrawTriangle(tA, tH, tC, cHull);
            DrawTriangle(tC, tD, tG, cHull); DrawTriangle(tC, tG, tD, cHull); DrawTriangle(tC, tG, tH, cHull); DrawTriangle(tC, tH, tG, cHull);
            DrawTriangle(tD, tE, tF, cHull); DrawTriangle(tD, tF, tE, cHull);
            for(int i=0; i<cpIdx-1; i++) {
                Vector2 pt1 = TransformPola(curvePts[i].x, curvePts[i].y, x, y, sf, angle); Vector2 pt2 = TransformPola(curvePts[i+1].x, curvePts[i+1].y, x, y, sf, angle);
                DrawTriangle(tD, pt1, pt2, cHull); DrawTriangle(tD, pt2, pt1, cHull);
            }
        } else {
            Vector2 junkHull[40]; int jhIdx = 0;
            junkHull[jhIdx++] = (Vector2){-190, -65}; junkHull[jhIdx++] = (Vector2){-140, -65}; junkHull[jhIdx++] = (Vector2){-140, -20};
            junkHull[jhIdx++] = (Vector2){150, -20};  junkHull[jhIdx++] = (Vector2){260, -40};
            for(int i=0; i<cpIdx; i++) junkHull[jhIdx++] = curvePts[i];
            junkHull[jhIdx++] = (Vector2){-150, 45};
            for(int i=0; i<jhIdx; i++) { Vector2 p1 = TransformPola(junkHull[i].x, junkHull[i].y, x, y, sf, angle); Vector2 p2 = TransformPola(junkHull[(i+1)%jhIdx].x, junkHull[(i+1)%jhIdx].y, x, y, sf, angle); AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, GREEN); }
        }

        // 4. Jendela Kotak Simetris
        Color outWin = isWire ? GREEN : BLANK;
        for(int r=0; r<3; r++) { for(int c=0; c<3; c++) { DrawCustomRect(-180.0f + c*12.0f, -58.0f + r*10.0f, 6.0f, 6.0f, x, y, sf, angle, cWinDark, outWin, isWire); } }
        for(int i=0; i<13; i++) { DrawCustomRect(-125.0f + i*22.0f, -10.0f, 8.0f, 6.0f, x, y, sf, angle, cWinDark, outWin, isWire); }

        // 5. Square Sails (Putih Terang)
        DrawGalleonSail(m1, -160, 40, 25, cWhite, x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m1, -125, 55, 35, cWhite, x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m1, -80,  70, 45, cWhite, x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m2, -210, 45, 30, cWhite, x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m2, -165, 65, 45, cWhite, x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m2, -105, 85, 60, cWhite, x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m3, -175, 40, 25, cWhite, x, y, sf, angle, sailFlap, isWire); DrawGalleonSail(m3, -140, 55, 35, cWhite, x, y, sf, angle, sailFlap, isWire);
        DrawGalleonSail(m3, -95,  70, 45, cWhite, x, y, sf, angle, sailFlap, isWire);

        // 6. Front Jib Sail (Layar Depan Putih)
        Vector2 jib[30]; int jIdx = 0;
        jib[jIdx++] = (Vector2){m3, -135}; jib[jIdx++] = (Vector2){280, -46};
        Vector2 j1_p0 = {280, -46}, j1_p1 = {230, -40}, j1_p2 = {180, -32};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; jib[jIdx++] = (Vector2){inv*inv*j1_p0.x + 2.0f*inv*t*j1_p1.x + t*t*j1_p2.x, inv*inv*j1_p0.y + 2.0f*inv*t*j1_p1.y + t*t*j1_p2.y}; }
        Vector2 j2_p0 = {180, -32}, j2_p1 = {155 - sailFlap, -85}, j2_p2 = {m3, -135};
        for(int i=1; i<=10; i++) { float t = i/10.0f; float inv = 1.0f - t; jib[jIdx++] = (Vector2){inv*inv*j2_p0.x + 2.0f*inv*t*j2_p1.x + t*t*j2_p2.x, inv*inv*j2_p0.y + 2.0f*inv*t*j2_p1.y + t*t*j2_p2.y}; }
        DrawCustomPoly(jib, jIdx, x, y, sf, angle, cWhite, isWire ? GREEN : BLANK, isWire);
    }
}