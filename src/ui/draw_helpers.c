/**
 * @file draw_helpers.c
 * @brief Kumpulan fungsi bantu untuk menggambar objek 2D di layar.
 *
 * File ini berisi dua kelompok fungsi utama:
 * 1. Pengisi bidang padat (segitiga dan lingkaran) menggunakan algoritma Scanline.
 * 2. Fungsi transformasi dan penggambaran objek yang mendukung rotasi, skala, dan translasi.
 */

#include "draw_helpers.h"
#include "bresenham.h"
#include "dda.h"
#include "midcircle.h"
#include <math.h>


// =============================================================================
// BAGIAN 1: PENGISIAN BIDANG SOLID (SCANLINE RASTERIZER)
// =============================================================================

/**
 * @brief Mengisi bidang segitiga dengan warna menggunakan metode Scanline.
 *
 * Cara kerja: titik-titik segitiga diurutkan dari atas ke bawah, lalu
 * untuk setiap baris piksel (sumbu Y), algoritma menghitung batas kiri dan
 * kanan segitiga, kemudian menggambar garis horisontal untuk mengisi area tersebut.
 *
 * @param p1 Titik pertama segitiga.
 * @param p2 Titik kedua segitiga.
 * @param p3 Titik ketiga segitiga.
 * @param c  Warna isi segitiga.
 */
void FillTri_Fast(Vector2 p1, Vector2 p2, Vector2 p3, Color c) {
    // Urutkan titik dari Y terkecil (atas layar) ke Y terbesar (bawah layar)
    if (p1.y > p2.y) { Vector2 t = p1; p1 = p2; p2 = t; }
    if (p1.y > p3.y) { Vector2 t = p1; p1 = p3; p3 = t; }
    if (p2.y > p3.y) { Vector2 t = p2; p2 = p3; p3 = t; }

    // Bulatkan batas Y ke bilangan bulat
    int y_start = (int)roundf(p1.y);
    int y_end   = (int)roundf(p3.y);

    float tinggi_total = p3.y - p1.y;
    if (tinggi_total <= 0.0001f) return; // Segitiga terlalu tipis, lewati

    // Gunakan < (bukan <=) agar tepi bawah tidak ditimpa oleh segitiga berikutnya
    for (int y = y_start; y < y_end; y++) {
        // Sampling di tengah piksel untuk akurasi pembagian area
        float fY = (float)y + 0.5f;

        bool bagian_bawah = fY > p2.y;
        float tinggi_segmen = bagian_bawah ? (p3.y - p2.y) : (p2.y - p1.y);
        if (tinggi_segmen <= 0.0001f) tinggi_segmen = 0.0001f;

        float alpha = (fY - p1.y) / tinggi_total;
        float beta  = (fY - (bagian_bawah ? p2.y : p1.y)) / tinggi_segmen;

        // Hitung batas kiri dan kanan untuk baris Y ini
        float xa = p1.x + (alpha * (p3.x - p1.x));
        float xb = bagian_bawah ? (p2.x + (beta * (p3.x - p2.x))) : (p1.x + (beta * (p2.x - p1.x)));

        if (xa > xb) { float t = xa; xa = xb; xb = t; }

        // Bulatkan ke bilangan bulat agar tidak ada celah antar segitiga
        int kiri  = (int)roundf(xa);
        int kanan = (int)roundf(xb);

        int lebar = kanan - kiri;
        if (lebar > 0) {
            DrawRectangle(kiri, y, lebar, 1, c);
        }
    }
}

/**
 * @brief Mengisi lingkaran penuh dengan warna menggunakan metode Scanline berbasis Midpoint.
 *
 * Cara kerja: algoritma Midpoint digunakan untuk mencari batas lingkaran di
 * setiap baris, lalu garis horisontal digambar untuk mengisi area di dalam lingkaran.
 *
 * @param centerX Koordinat X pusat lingkaran.
 * @param centerY Koordinat Y pusat lingkaran.
 * @param radius  Jari-jari lingkaran dalam piksel.
 * @param color   Warna isi lingkaran.
 */
void FillCircle_Fast(int centerX, int centerY, int radius, Color color) {
    if (radius <= 0) {
        DrawPixel(centerX, centerY, color);
        return;
    }
    int x   = radius;
    int y   = 0;
    int err = 0;

    while (x >= y) {
        // Gambar 4 garis horisontal untuk mengisi lingkaran di 4 posisi simetris
        DrawRectangle(centerX - x, centerY + y, (x * 2) + 1, 1, color);
        DrawRectangle(centerX - x, centerY - y, (x * 2) + 1, 1, color);
        DrawRectangle(centerX - y, centerY + x, (y * 2) + 1, 1, color);
        DrawRectangle(centerX - y, centerY - x, (y * 2) + 1, 1, color);

        if (err <= 0) {
            y   += 1;
            err += 2 * y + 1;
        }
        if (err > 0) {
            x   -= 1;
            err -= 2 * x + 1;
        }
    }
}


// =============================================================================
// BAGIAN 2: TRANSFORMASI DAN PENGGAMBARAN OBJEK
// =============================================================================

// Status algoritma garis yang sedang aktif (Bresenham atau DDA)
static bool g_useBresenhamAlgo = true;

/**
 * @brief Mengatur algoritma garis yang akan digunakan secara global.
 *
 * @param useBresenham Isi true untuk memakai Bresenham, false untuk memakai DDA.
 */
void SetDrawHelperAlgorithm(bool useBresenham) {
    g_useBresenhamAlgo = useBresenham;
}

/**
 * @brief Menggambar garis menggunakan algoritma yang sedang aktif.
 *
 * Fungsi ini memilih antara Bresenham atau DDA sesuai pengaturan global.
 * Semua fungsi penggambar garis di proyek ini memanggil fungsi ini.
 *
 * @param x1 Koordinat X titik awal.
 * @param y1 Koordinat Y titik awal.
 * @param x2 Koordinat X titik akhir.
 * @param y2 Koordinat Y titik akhir.
 * @param c  Warna garis.
 */
void AlgoLine(int x1, int y1, int x2, int y2, Color c) {
    if (g_useBresenhamAlgo) BresenhamLine(x1, y1, x2, y2, c);
    else                    DDALine(x1, y1, x2, y2, c);
}

/**
 * @brief Mengubah posisi sebuah titik berdasarkan translasi, rotasi, dan skala.
 *
 * Titik lokal (koordinat relatif dari pusat objek) dikonversi menjadi
 * koordinat layar yang sesungguhnya dengan menerapkan rotasi dan skala,
 * kemudian menggesernya ke posisi yang ditentukan.
 *
 * @param px    Koordinat X titik lokal.
 * @param py    Koordinat Y titik lokal.
 * @param tx    Pergeseran ke kanan/kiri (posisi X di layar).
 * @param ty    Pergeseran ke atas/bawah (posisi Y di layar).
 * @param scale Faktor skala (1.0 = ukuran normal).
 * @param angle Sudut rotasi dalam radian.
 * @return      Koordinat titik yang sudah ditransformasi.
 */
Vector2 TransformPola(float px, float py, float tx, float ty, float scale, float angle) {
    float cosA = cosf(angle);
    float sinA = sinf(angle);
    float rx = (px * cosA - py * sinA) * scale;
    float ry = (px * sinA + py * cosA) * scale;
    return (Vector2){ rx + tx, ry + ty };
}

/**
 * @brief Menggambar poligon (bentuk bersisi banyak) dengan warna dan garis tepi.
 *
 * Semua titik yang diberikan terlebih dahulu ditransformasi ke koordinat layar,
 * kemudian bidangnya diisi dengan metode Scanline (mode solid) atau digambar
 * garis tepinya saja (mode wireframe).
 *
 * @param localPts Array titik-titik lokal penyusun poligon.
 * @param count    Jumlah titik dalam array.
 * @param tx       Posisi X objek di layar.
 * @param ty       Posisi Y objek di layar.
 * @param scale    Faktor skala objek.
 * @param angle    Sudut rotasi objek dalam radian.
 * @param fill     Warna isi bidang (mode solid).
 * @param outline  Warna garis tepi.
 * @param isWire   Jika true, hanya garis tepi yang digambar (mode wireframe).
 */
void DrawCustomPoly(Vector2* localPts, int count, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    Vector2 worldPts[50];
    for (int i = 0; i < count; i++) {
        worldPts[i] = TransformPola(localPts[i].x, localPts[i].y, tx, ty, scale, angle);
    }

    // Isi bidang poligon dengan memecahnya menjadi kumpulan segitiga
    if (!isWire && count >= 3 && fill.a > 0) {
        for (int i = 1; i < count - 1; i++) {
            FillTri_Fast(worldPts[0], worldPts[i], worldPts[i+1], fill);
        }
    }

    // Gambar garis tepi menghubungkan setiap titik
    if (outline.a > 0) {
        for (int i = 0; i < count; i++) {
            Vector2 a = worldPts[i];
            Vector2 b = worldPts[(i+1) % count];
            AlgoLine((int)a.x, (int)a.y, (int)b.x, (int)b.y, outline);
        }
    }
}

/**
 * @brief Menggambar kurva Bezier kuadratik (kurva melengkung dari 3 titik).
 *
 * Kurva dibentuk dari tiga titik: titik awal (p0), titik kontrol yang menentukan
 * arah lengkungan (p1), dan titik akhir (p2). Kurva dipotong menjadi beberapa
 * segmen garis lurus kecil sehingga terlihat mulus.
 *
 * @param p0      Titik awal kurva.
 * @param p1      Titik kontrol (menentukan arah lengkungan).
 * @param p2      Titik akhir kurva.
 * @param tx      Posisi X objek di layar.
 * @param ty      Posisi Y objek di layar.
 * @param scale   Faktor skala objek.
 * @param angle   Sudut rotasi objek dalam radian.
 * @param fill    Warna isi (jika kurva membentuk area tertutup).
 * @param outline Warna garis tepi kurva.
 * @param isWire  Jika true, hanya garis tepi yang digambar.
 */
void DrawCustomBezier(Vector2 p0, Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    Vector2 t0 = TransformPola(p0.x, p0.y, tx, ty, scale, angle);
    Vector2 t1 = TransformPola(p1.x, p1.y, tx, ty, scale, angle);
    Vector2 t2 = TransformPola(p2.x, p2.y, tx, ty, scale, angle);

    int     segmen = 12; // Jumlah segmen kurva (semakin banyak, semakin halus)
    Vector2 prev   = t0;

    for (int i = 1; i <= segmen; i++) {
        float t    = (float)i / segmen;
        float invT = 1.0f - t;

        // Rumus Bezier kuadratik: B(t) = (1-t)^2*P0 + 2(1-t)*t*P1 + t^2*P2
        float px = invT*invT*t0.x + 2*invT*t*t1.x + t*t*t2.x;
        float py = invT*invT*t0.y + 2*invT*t*t1.y + t*t*t2.y;
        Vector2 titik_sekarang = (Vector2){px, py};

        if (!isWire && fill.a > 0) {
            FillTri_Fast(t0, prev, titik_sekarang, fill);
        }
        AlgoLine((int)prev.x, (int)prev.y, (int)titik_sekarang.x, (int)titik_sekarang.y, outline);
        prev = titik_sekarang;
    }
}

/**
 * @brief Menggambar satu segmen garis lurus dengan transformasi otomatis.
 *
 * Titik awal dan akhir garis akan dikonversi terlebih dahulu sesuai
 * posisi, rotasi, dan skala objek sebelum digambar ke layar.
 *
 * @param p1    Titik awal garis (koordinat lokal).
 * @param p2    Titik akhir garis (koordinat lokal).
 * @param tx    Posisi X objek di layar.
 * @param ty    Posisi Y objek di layar.
 * @param scale Faktor skala objek.
 * @param angle Sudut rotasi objek dalam radian.
 * @param col   Warna garis.
 */
void DrawCustomLine(Vector2 p1, Vector2 p2, float tx, float ty, float scale, float angle, Color col) {
    Vector2 t1 = TransformPola(p1.x, p1.y, tx, ty, scale, angle);
    Vector2 t2 = TransformPola(p2.x, p2.y, tx, ty, scale, angle);
    AlgoLine((int)t1.x, (int)t1.y, (int)t2.x, (int)t2.y, col);
}

/**
 * @brief Menggambar lingkaran dengan warna isi dan garis tepi, disesuaikan dengan transformasi objek.
 *
 * Pada mode solid, lingkaran diisi penuh menggunakan FillCircle_Fast.
 * Pada mode wireframe, hanya tepi lingkaran yang digambar menggunakan Midcircle.
 *
 * @param center  Posisi pusat lingkaran (koordinat lokal).
 * @param radius  Jari-jari lingkaran.
 * @param tx      Posisi X objek di layar.
 * @param ty      Posisi Y objek di layar.
 * @param scale   Faktor skala objek.
 * @param angle   Sudut rotasi objek dalam radian.
 * @param fill    Warna isi lingkaran.
 * @param outline Warna tepi lingkaran (digambar sedikit lebih besar dari isi).
 * @param isWire  Jika true, hanya garis tepi yang digambar.
 */
void DrawCustomCircle(Vector2 center, float radius, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    Vector2 c      = TransformPola(center.x, center.y, tx, ty, scale, angle);
    float   r      = radius * scale;
    float   strokeR = r + (2.5f * scale); // Tepi sedikit lebih besar dari isi

    if (!isWire) {
        if (outline.a > 0) FillCircle_Fast((int)c.x, (int)c.y, (int)strokeR, outline);
        if (fill.a   > 0) FillCircle_Fast((int)c.x, (int)c.y, (int)r,       fill);
    } else {
        if (outline.a > 0) Midcircle((int)c.x, (int)c.y, (int)strokeR, GREEN);
        if (fill.a   > 0) Midcircle((int)c.x, (int)c.y, (int)r,       GREEN);
    }
}

/**
 * @brief Menggambar cincin (donat) dengan warna tertentu, disesuaikan dengan transformasi objek.
 *
 * Cincin dibentuk dengan membagi lingkaran menjadi beberapa segmen trapesium tipis,
 * lalu masing-masing segmen diisi menggunakan dua segitiga.
 * Digunakan untuk menggambar lampu atau tepi bulat pada objek.
 *
 * @param center    Posisi pusat cincin (koordinat lokal).
 * @param radius    Jari-jari tengah cincin.
 * @param thickness Ketebalan cincin.
 * @param tx        Posisi X objek di layar.
 * @param ty        Posisi Y objek di layar.
 * @param scale     Faktor skala objek.
 * @param angle     Sudut rotasi objek dalam radian.
 * @param color     Warna cincin.
 * @param isWire    Jika true, hanya garis tepi yang digambar.
 */
void DrawCustomRing(Vector2 center, float radius, float thickness, float tx, float ty, float scale, float angle, Color color, bool isWire) {
    Vector2 c  = TransformPola(center.x, center.y, tx, ty, scale, angle);
    float   r  = radius    * scale;
    float   th = thickness * scale;
    int     segmen = 24;

    Vector2 prevDalam = {c.x + (r - th/2.0f), c.y};
    Vector2 prevLuar  = {c.x + (r + th/2.0f), c.y};

    for (int i = 1; i <= segmen; i++) {
        float   t       = (float)i / segmen * 2.0f * PI;
        Vector2 currDalam = {c.x + cosf(t) * (r - th/2.0f), c.y + sinf(t) * (r - th/2.0f)};
        Vector2 currLuar  = {c.x + cosf(t) * (r + th/2.0f), c.y + sinf(t) * (r + th/2.0f)};

        if (!isWire) {
            // Isi setiap segmen cincin dengan dua segitiga
            FillTri_Fast(prevLuar, prevDalam, currDalam, color);
            FillTri_Fast(prevLuar, currDalam, currLuar,  color);
        } else {
            // Gambar hanya tepi dalam dan luar cincin
            AlgoLine((int)prevDalam.x, (int)prevDalam.y, (int)currDalam.x, (int)currDalam.y, GREEN);
            AlgoLine((int)prevLuar.x,  (int)prevLuar.y,  (int)currLuar.x,  (int)currLuar.y,  GREEN);
        }
        prevDalam = currDalam;
        prevLuar  = currLuar;
    }
}

/**
 * @brief Menggambar persegi panjang dengan transformasi rotasi dan skala.
 *
 * Persegi panjang dibentuk dari 4 titik yang ditransformasi, lalu diisi
 * menggunakan dua segitiga (mode solid) atau digambar garis tepinya saja.
 *
 * @param rx      Koordinat X lokal pojok kiri atas.
 * @param ry      Koordinat Y lokal pojok kiri atas.
 * @param rw      Lebar persegi panjang.
 * @param rh      Tinggi persegi panjang.
 * @param tx      Posisi X objek di layar.
 * @param ty      Posisi Y objek di layar.
 * @param scale   Faktor skala objek.
 * @param angle   Sudut rotasi objek dalam radian.
 * @param fill    Warna isi.
 * @param outline Warna garis tepi.
 * @param isWire  Jika true, hanya garis tepi yang digambar.
 */
void DrawCustomRect(float rx, float ry, float rw, float rh, float tx, float ty, float scale, float angle, Color fill, Color outline, bool isWire) {
    Vector2 p1 = TransformPola(rx,      ry,      tx, ty, scale, angle);
    Vector2 p2 = TransformPola(rx + rw, ry,      tx, ty, scale, angle);
    Vector2 p3 = TransformPola(rx + rw, ry + rh, tx, ty, scale, angle);
    Vector2 p4 = TransformPola(rx,      ry + rh, tx, ty, scale, angle);

    // Isi dengan dua segitiga yang membentuk persegi panjang
    if (!isWire && fill.a > 0) {
        FillTri_Fast(p1, p2, p3, fill);
        FillTri_Fast(p1, p3, p4, fill);
    }

    // Gambar 4 sisi tepi persegi panjang
    Color warnaSisi = isWire ? GREEN : outline;
    if (warnaSisi.a > 0 || isWire) {
        AlgoLine((int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y, warnaSisi);
        AlgoLine((int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y, warnaSisi);
        AlgoLine((int)p3.x, (int)p3.y, (int)p4.x, (int)p4.y, warnaSisi);
        AlgoLine((int)p4.x, (int)p4.y, (int)p1.x, (int)p1.y, warnaSisi);
    }
}

/**
 * @brief Menggambar segitiga sorot cahaya dengan pemangkasan batas layar.
 *
 * Digunakan khusus untuk efek sinar lampu mercusuar. Segitiga dipangkas agar
 * tidak keluar dari batas area yang ditentukan. Proses pemangkasan dilakukan
 * terhadap empat sisi batas (kiri, kanan, atas, bawah) secara berurutan.
 *
 * @param p0   Titik pusat sumber cahaya.
 * @param p1   Titik tepi kiri sorotan.
 * @param p2   Titik tepi kanan sorotan.
 * @param minX Batas kiri area tampil.
 * @param minY Batas atas area tampil.
 * @param maxX Batas kanan area tampil.
 * @param maxY Batas bawah area tampil.
 * @param col  Warna sorotan cahaya.
 */
void DrawClippedBeamTriangle(Vector2 p0, Vector2 p1, Vector2 p2, float minX, float minY, float maxX, float maxY, Color col) {
    Vector2 poly[10]; 
    int polyCount = 3;
    poly[0] = p0; 
    poly[1] = p1; 
    poly[2] = p2;
    Vector2 temp[10]; 
    int tempCount;

    // Pangkas sisi kiri
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { 
        Vector2 curr = poly[i]; 
        Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; 
        bool currIn = curr.x >= minX; 
        bool prevIn = prev.x >= minX; 
        if (currIn != prevIn) temp[tempCount++] = (Vector2){ 
            minX, prev.y + (minX - prev.x) / (curr.x - prev.x) * (curr.y - prev.y) }; 
            if (currIn) temp[tempCount++] = curr; 
    }
    polyCount = tempCount; 
    for(int i=0;i<polyCount;i++) poly[i]=temp[i];

    // Pangkas sisi kanan
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { 
        Vector2 curr = poly[i]; 
        Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; 
        bool currIn = curr.x <= maxX; 
        bool prevIn = prev.x <= maxX; 
        if (currIn != prevIn) temp[tempCount++] = (Vector2){ 
            maxX, prev.y + (maxX - prev.x) / (curr.x - prev.x) * (curr.y - prev.y) 
        }; 
        if (currIn) temp[tempCount++] = curr; 
    }
    polyCount = tempCount; 
    for(int i=0;i<polyCount;i++) poly[i]=temp[i];

    // Pangkas sisi atas
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { Vector2 curr = poly[i]; 
        Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; 
        bool currIn = curr.y >= minY; 
        bool prevIn = prev.y >= minY; 
        if (currIn != prevIn) temp[tempCount++] = (Vector2){ 
            prev.x + (minY - prev.y) / (curr.y - prev.y) * (curr.x - prev.x), minY 
        }; 
        if (currIn) temp[tempCount++] = curr; 
    }
    polyCount = tempCount; 
    for(int i=0;i<polyCount;i++) poly[i]=temp[i];

    // Pangkas sisi bawah
    tempCount = 0;
    for (int i=0; i<polyCount; i++) { 
        Vector2 curr = poly[i]; 
        Vector2 prev = poly[(i - 1 + polyCount) % polyCount]; 
        bool currIn = curr.y <= maxY; 
        bool prevIn = prev.y <= maxY; 
        if (currIn != prevIn) temp[tempCount++] = (Vector2){ 
            prev.x + (maxY - prev.y) / (curr.y - prev.y) * (curr.x - prev.x), maxY 
        }; 
        if (currIn) temp[tempCount++] = curr; 
    }
    polyCount = tempCount; 
    for(int i=0;i<polyCount;i++) poly[i]=temp[i];

    // Isi area segitiga yang sudah dipangkas
    if (polyCount >= 3) {
        for (int i=1; i<polyCount-1; i++) {
            FillTri_Fast(poly[0], poly[i], poly[i+1], col);
        }
    }
}