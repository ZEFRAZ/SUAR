/**
 * @file pola2d.c
 * @brief Layar Showcase Objek (Pola 2D).
 *
 * Layar ini berfungsi sebagai panggung presentasi — menampilkan satu per satu
 * objek yang ada dalam proyek (kapal, awan, mercusuar, dll) di tengah layar
 * agar bisa diperiksa detail bentuknya.
 *
 * Fitur utama:
 * - Navigasi antar objek dengan tombol panah atau keyboard (A/D).
 * - Toggle mode wireframe (hanya garis) vs solid (warna penuh) dengan tombol W.
 * - Toggle algoritma garis DDA vs Bresenham dengan tombol B.
 * - Beberapa objek memiliki sub-varian yang bisa dipilih.
 * - Semua ukuran UI menyesuaikan ukuran window secara otomatis (Camera2D).
 */

#include "pola2d.h"
#include "../ui/draw_helpers.h"
#include "../objects/alam.h"
#include "../objects/bangunan.h"
#include "../objects/kapal.h"
#include <math.h>
#include "raymath.h"
#include "back_button.h"
#include "menu.h"

// =============================================================================
// BAGIAN 1: VARIABEL STATUS INTERNAL
// =============================================================================

static bool g_useBresenham         = false; // Algoritma garis yang aktif
static int  currentItemIndex       = 0;     // Indeks objek yang sedang ditampilkan
static int  currentSloopVariant    = 0;     // Varian kapal sloop yang dipilih
static int  currentPirateVariant   = 0;     // Varian kapal bajak laut yang dipilih
static int  currentGalleonVariant  = 0;     // Varian kapal galleon yang dipilih
static int  currentWeatherVariant  = 0;     // Varian cuaca yang dipilih
static int  currentLighthouseVariant = 0;   // Varian mercusuar yang dipilih

#define MAX_ITEMS 19 // Jumlah total objek yang bisa ditampilkan

static bool showWireframe    = false; // Status mode wireframe
static bool useBresenhamAlgo = false; // Status pemilihan algoritma garis

float previewTime = 0.0f; // Waktu animasi preview yang berjalan terus (dipakai oleh objek.c)

// Aset layar Pola 2D (dimuat sekali saja)
Font      polaFont           = { 0 };
static Texture2D bgTexturePola   = { 0 };
static bool      isPolaAssetsLoaded = false;

// Status progres animasi hover tombol navigasi
static float leftHovProg      = 0.0f;
static float rightHovProg     = 0.0f;
static float varHovProg[3]    = {0.0f, 0.0f, 0.0f};

// =============================================================================
// BAGIAN 2: UTILITAS INTERNAL
// =============================================================================

/**
 * @brief Mengatur algoritma garis yang dipakai di seluruh layar Pola 2D.
 *
 * Fungsi ini meneruskan pilihan algoritma ke modul draw_helpers agar
 * semua fungsi penggambar garis memakai algoritma yang sama.
 *
 * @param useBresenhamAlgo true = Bresenham, false = DDA.
 */
void SetPolaAlgorithm(bool useBresenhamAlgo) {
    g_useBresenham = useBresenhamAlgo;
    SetDrawHelperAlgorithm(useBresenhamAlgo);
}

/**
 * @brief Menghitung warna campuran antara dua warna berdasarkan persentase.
 *
 * Digunakan untuk membuat efek hover tombol navigasi yang berubah warna
 * secara mulus (bukan seketika).
 *
 * @param a Warna awal.
 * @param b Warna akhir.
 * @param t Persentase transisi (0.0 = warna a, 1.0 = warna b).
 * @return  Warna campuran hasil interpolasi.
 */
static Color LerpColorUI(Color a, Color b, float t) {
    return (Color){
        (unsigned char)(a.r + t * (b.r - a.r)),
        (unsigned char)(a.g + t * (b.g - a.g)),
        (unsigned char)(a.b + t * (b.b - a.b)), 255
    };
}

// =============================================================================
// BAGIAN 3: LAYAR SHOWCASE UTAMA
// =============================================================================

/**
 * @brief Menjalankan logika dan penggambaran layar Pola 2D setiap frame.
 *
 * Fungsi ini mengatur semua aspek layar showcase:
 * - Memuat aset jika belum dimuat.
 * - Memproses input keyboard (navigasi, toggle wireframe, toggle algoritma).
 * - Memproses klik tombol navigasi dan tombol sub-varian.
 * - Menggambar latar, panel, objek yang dipilih, judul, dan info algoritma.
 *
 * Semua ukuran dan posisi menggunakan koordinat virtual 1920x1080 yang
 * kemudian diskalakan otomatis via Camera2D agar proporsional di semua resolusi.
 *
 * @return ScreenState Layar berikutnya (SCREEN_MENU jika tombol Kembali ditekan,
 *                     SCREEN_POLA_2D jika belum ditekan).
 */
ScreenState UpdateDrawPola2D(void) {
    if (!isPolaAssetsLoaded) {
        polaFont           = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0);
        bgTexturePola      = LoadTexture("background_pola2d.png");
        isPolaAssetsLoaded = true;
    }

    int W = GetScreenWidth();
    int H = GetScreenHeight();

    // Siapkan kamera virtual 1920x1080 agar tampilan tetap proporsional
    Camera2D camera    = { 0 };
    camera.target      = (Vector2){ 1920.0f / 2.0f, 1080.0f / 2.0f };
    camera.offset      = (Vector2){ W / 2.0f, H / 2.0f };
    camera.rotation    = 0.0f;
    float scaleX       = (float)W / 1920.0f;
    float scaleY       = (float)H / 1080.0f;
    camera.zoom        = (scaleX < scaleY) ? scaleX : scaleY; // Letterbox agar tidak gepeng

    // Konversi posisi kursor dari koordinat layar ke koordinat virtual
    Vector2 mouse = GetScreenToWorld2D(GetMousePosition(), camera);

    float dt = GetFrameTime();
    previewTime += dt; // Jam animasi preview terus berjalan

    // Proses input keyboard
    if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) currentItemIndex = (currentItemIndex + 1) % MAX_ITEMS;
    if (IsKeyPressed(KEY_LEFT)  || IsKeyPressed(KEY_A)) currentItemIndex = (currentItemIndex - 1 + MAX_ITEMS) % MAX_ITEMS;
    if (IsKeyPressed(KEY_W)) showWireframe    = !showWireframe;
    if (IsKeyPressed(KEY_B)) useBresenhamAlgo = !useBresenhamAlgo;

    SetPolaAlgorithm(useBresenhamAlgo);

    // Hitung posisi dan ukuran panel showcase
    float canvasW = 1400.0f;
    float canvasH = 850.0f;
    float canvasX = (1920.0f - canvasW) / 2.0f;
    float canvasY = (1080.0f - canvasH) / 2.0f + 20.0f;

    float cx = canvasX + (canvasW / 2.0f);        // Pusat horizontal panel
    float cy = canvasY + (canvasH / 2.0f) - 60.0f; // Pusat vertikal panel (agak naik)

    // Hitung posisi tombol navigasi kiri dan kanan
    float     navSize = 70.0f;
    Rectangle btnLeft  = { canvasX - navSize - 20.0f, canvasY + (canvasH / 2.0f) - (navSize / 2.0f), navSize, navSize };
    Rectangle btnRight = { canvasX + canvasW + 20.0f, canvasY + (canvasH / 2.0f) - (navSize / 2.0f), navSize, navSize };

    bool hoverLeft  = CheckCollisionPointRec(mouse, btnLeft);
    bool hoverRight = CheckCollisionPointRec(mouse, btnRight);

    // Perbarui progres hover tombol kiri dan kanan
    if (hoverLeft)  leftHovProg  += dt * 10.0f; else leftHovProg  -= dt * 8.0f;
    if (hoverRight) rightHovProg += dt * 10.0f; else rightHovProg -= dt * 8.0f;
    if (leftHovProg  > 1.0f) leftHovProg  = 1.0f;
    if (leftHovProg  < 0.0f) leftHovProg  = 0.0f;
    if (rightHovProg > 1.0f) rightHovProg = 1.0f;
    if (rightHovProg < 0.0f) rightHovProg = 0.0f;

    // Proses klik tombol navigasi
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (hoverLeft)  currentItemIndex = (currentItemIndex - 1 + MAX_ITEMS) % MAX_ITEMS;
        if (hoverRight) currentItemIndex = (currentItemIndex + 1) % MAX_ITEMS;
    }

    if (BackButtonPressed()) return SCREEN_MENU;

    // =========================================================================
    // Gambar latar belakang layar
    // =========================================================================
    if (bgTexturePola.id != 0) {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)bgTexturePola.width, (float)bgTexturePola.height };
        Rectangle destRec   = { 0.0f, 0.0f, (float)W, (float)H };
        DrawTexturePro(bgTexturePola, sourceRec, destRec, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        ClearBackground((Color){ 20, 25, 35, 255 });
    }

    // =========================================================================
    // Mulai menggambar UI dengan kamera virtual
    // =========================================================================
    BeginMode2D(camera);

    // Panel bayangan di belakang panel utama
    Rectangle shadowRec = { canvasX + 10.0f, canvasY + 15.0f, canvasW, canvasH };
    DrawRectangleRounded(shadowRec, 0.05f, 16, (Color){ 0, 0, 0, 100 });

    // Panel utama showcase (biru baja)
    Rectangle canvasRec = { canvasX, canvasY, canvasW, canvasH };
    DrawRectangleRounded(canvasRec, 0.05f, 16, (Color){ 85, 135, 175, 240 });
    DrawRectangleRoundedLines(canvasRec, 0.05f, 16, (Color){ 180, 220, 240, 200 });

    // Parameter animasi dimatikan di layar showcase agar objek terlihat diam
    float angle = 0.0f; // Tidak ada goyang kapal
    float flap  = 0.0f; // Tidak ada kepakan layar
    float baseShipScale  = 2.2f;
    const char* title    = "";

    // =========================================================================
    // Gambar objek yang dipilih sesuai indeks saat ini
    // =========================================================================
    switch (currentItemIndex) {
        // --- Kapal ---
        case 0:  title = "GIANT CARGO SHIP";          DrawShip_Cargo(cx, cy + 100.0f, baseShipScale, angle, (Color){80,60,40,255}, (Color){200,200,200,255}, showWireframe); break;
        case 1:  title = "LUXURY CRUISE SHIP";         DrawShip_Cruise(cx, cy + 200.0f, baseShipScale * 1.5f, angle, WHITE, LIGHTGRAY, showWireframe); break;
        case 2:  title = "MILITARY WARSHIP";           DrawShip_Military(cx, cy + 100.0f, baseShipScale * 2.0f, angle, GRAY, DARKGRAY, showWireframe); break;
        case 3:  title = "SCIENTIFIC RESEARCH SHIP";  DrawShip_Research(cx, cy + 150.0f, baseShipScale * 2.0f, angle, RED, WHITE, showWireframe); break;
        case 4:  title = "PRIVATE LUXURY YACHT";       DrawShip_Yacht(cx, cy + 100.0f, baseShipScale, angle, WHITE, LIGHTGRAY, showWireframe); break;
        case 5:  title = "CORSAIR PIRATE SHIP";        DrawShip_Pirate_Variant(cx, cy + 50.0f, baseShipScale * 1.3f, angle, flap, currentPirateVariant, showWireframe); break;
        case 6:  title = "SPANISH GALLEON SHIP";       DrawShip_Galleon(cx, cy + 100.0f, baseShipScale * 1.3f, angle, flap, currentGalleonVariant, showWireframe); break;
        case 7:  title = "FISHING BOAT";               DrawShip_Fishing(cx, cy, baseShipScale * 1.5f, angle, (Color){80,60,40,255}, ORANGE, showWireframe); break;
        case 8:  title = "LIGHT SLOOP SHIP";           DrawShip_Sloop_Variant(cx, cy + 60.0f, baseShipScale * 0.9f, angle, flap, currentSloopVariant, showWireframe); break;

        // --- Mercusuar ---
        case 9:
            title = (currentLighthouseVariant == 0) ? "MERCUSUAR (MALAM/NYALA)" : "MERCUSUAR (PAGI/MATI)";
            DrawMercusuar_Pola(cx, cy + 100.0f, baseShipScale * 0.8f, currentLighthouseVariant, showWireframe);
            break;

        // --- Elemen Alam dan Lingkungan ---
        case 10: title = "GUNUNG BESAR";       DrawGunung_Pola(cx, cy - 20.0f, canvasW - 2.0f, 600.0f, (Color){30, 40, 60, 255}, showWireframe); break;
        case 11: title = "TEBING PANTAI";      DrawTebing_Pola(cx + 40.0f, cy - 60.0f, baseShipScale * 0.9f, 0, BLANK, BLANK, showWireframe); break;
        case 12: title = "AWAN & ANGIN";       DrawAwan_Pola(0, cx, cy, baseShipScale * 1.5f, (Color){220, 225, 235, 255}, showWireframe); break;
        case 13:
            title = "KAWANAN BURUNG CAMAR";
            DrawBurung_Pola(cx,         cy,        baseShipScale * 5.0f, sinf(previewTime * 8.0f), (Color){34,34,34,255}, showWireframe); // Pemimpin kawanan
            DrawBurung_Pola(cx - 120.0f, cy - 80.0f, baseShipScale * 3.0f, sinf(previewTime * 9.5f), (Color){34,34,34,255}, showWireframe); // Pengikut kiri
            DrawBurung_Pola(cx + 140.0f, cy - 40.0f, baseShipScale * 3.5f, sinf(previewTime * 7.0f), (Color){34,34,34,255}, showWireframe); // Pengikut kanan
            break;
        case 14: title = "MATAHARI & BULAN"; DrawBulanMatahari_Pola(cx, cy, baseShipScale * 1.5f, showWireframe); break;
        case 15: title = "BINTANG MALAM";    DrawBintang_Pola(cx, cy, baseShipScale * 1.2f, WHITE, showWireframe); break;
        case 16: title = "KABUT ATMOSFER";   DrawKabut_Pola(cx, cy, baseShipScale, showWireframe); break;
        case 17: title = "OMBAK LAUT LAJU";  DrawOmbak_Pola(cx, cy, 1400.0f, 400.0f, showWireframe); break;
        case 18: {
            title = (currentWeatherVariant == 0) ? "CUACA: HUJAN DERAS" : "CUACA: BADAI PETIR";
            bool isStorm = (currentWeatherVariant == 1);
            DrawCuaca_HujanBadai(cx, cy + 60.0f, canvasW, canvasH, isStorm, showWireframe);
            break;
        }
    }

    // =========================================================================
    // Gambar kotak judul nama objek di bagian atas panel
    // =========================================================================
    float   titleFontSize = 36.0f;
    Vector2 tSize         = MeasureTextEx(polaFont, title, titleFontSize, 2.0f);
    Rectangle titleBox    = { cx - tSize.x/2 - 30.0f, canvasY - (tSize.y/2) - 10.0f, tSize.x + 60.0f, tSize.y + 20.0f };

    DrawRectangleRounded((Rectangle){titleBox.x - 6.0f, titleBox.y - 6.0f, titleBox.width + 12.0f, titleBox.height + 12.0f}, 0.5f, 16, (Color){ 101, 67, 33, 255 }); // Garis tepi
    DrawRectangleRounded(titleBox, 0.5f, 16, (Color){ 160, 30, 45, 255 }); // Kotak merah
    DrawTextEx(polaFont, title, (Vector2){ titleBox.x + 30.0f, titleBox.y + 10.0f }, titleFontSize, 2.0f, WHITE);

    // =========================================================================
    // Gambar info mode wireframe dan algoritma di bagian bawah panel
    // =========================================================================
    float     infoAreaY = canvasY + canvasH - 140.0f;
    float     infoBoxW  = 400.0f;
    float     infoBoxH  = 50.0f;
    float     gap       = 40.0f;
    Rectangle modeRec   = { cx - infoBoxW - (gap/2.0f), infoAreaY, infoBoxW, infoBoxH };
    Rectangle algoRec   = { cx + (gap/2.0f),            infoAreaY, infoBoxW, infoBoxH };

    DrawRectangleRounded(modeRec, 0.5f, 16, (Color){ 10, 45, 80, 200 });
    DrawRectangleRoundedLines(modeRec, 0.5f, 16, (Color){ 30, 150, 200, 255 });
    DrawRectangleRounded(algoRec, 0.5f, 16, (Color){ 10, 45, 80, 200 });
    DrawRectangleRoundedLines(algoRec, 0.5f, 16, (Color){ 30, 150, 200, 255 });

    const char* modeTxt = TextFormat("MODE : %s", showWireframe ? "WIREFRAME" : "SOLID (WARNA PENUH)");
    const char* algoTxt = TextFormat("ALGO : %s", useBresenhamAlgo ? "BRESENHAM LINE" : "DDA LINE");
    const char* hintTxt = "TEKAN [W] UBAH MODE   |   TEKAN [B] UBAH ALGORITMA";

    float   infoTextSize = 22.0f;
    Vector2 modeTSize    = MeasureTextEx(polaFont, modeTxt, infoTextSize, 1.5f);
    Vector2 algoTSize    = MeasureTextEx(polaFont, algoTxt, infoTextSize, 1.5f);

    DrawTextEx(polaFont, modeTxt, (Vector2){ modeRec.x + (modeRec.width/2) - (modeTSize.x/2), modeRec.y + (modeRec.height/2) - (modeTSize.y/2) }, infoTextSize, 1.5f, YELLOW);
    DrawTextEx(polaFont, algoTxt, (Vector2){ algoRec.x + (algoRec.width/2) - (algoTSize.x/2), algoRec.y + (algoRec.height/2) - (algoTSize.y/2) }, infoTextSize, 1.5f, SKYBLUE);

    float   hintTextSize = 18.0f;
    Vector2 hintSize     = MeasureTextEx(polaFont, hintTxt, hintTextSize, 1.5f);
    DrawTextEx(polaFont, hintTxt, (Vector2){ cx - hintSize.x/2, infoAreaY + infoBoxH + 20.0f }, hintTextSize, 1.5f, LIGHTGRAY);

    // =========================================================================
    // Gambar tombol sub-varian (hanya untuk objek yang memiliki lebih dari satu varian)
    // =========================================================================
    Color idleC  = (Color){ 10, 45, 80, 255 };
    Color hovC   = (Color){ 0, 130, 180, 255 };
    Color outIdl = (Color){ 30, 100, 150, 255 };
    Color outHov = (Color){ 0, 255, 255, 255 };

    if (currentItemIndex == 8 || currentItemIndex == 5 || currentItemIndex == 6 ||
        currentItemIndex == 18 || currentItemIndex == 9) {

        // Tentukan nama-nama varian yang tersedia untuk objek ini
        const char* varNames[3] = {"1: BOMB PIRATE", "2: OBSIDIAN", "3: CRIMSON BLOOD"};
        int numVars = 3;

        if      (currentItemIndex == 8)  { varNames[0] = "1: CLASSIC";   varNames[1] = "2: RED TRIM"; varNames[2] = "3: GOLD HULL"; }
        else if (currentItemIndex == 6)  { varNames[0] = "1: WESTERN";   varNames[1] = "2: EASTERN"; numVars = 2; }
        else if (currentItemIndex == 18) { varNames[0] = "HUJAN DERAS";  varNames[1] = "BADAI PETIR"; numVars = 2; }
        else if (currentItemIndex == 9)  { varNames[0] = "LAMPU NYALA";  varNames[1] = "LAMPU MATI";  numVars = 2; }

        float varBtnW   = 180.0f;
        float varBtnH   = 45.0f;
        float varGap    = 20.0f;
        float totalW    = (numVars * varBtnW) + ((numVars - 1) * varGap);
        float varStartX = cx - (totalW / 2.0f);
        float varStartY = infoAreaY - varBtnH - 30.0f;

        for (int i = 0; i < numVars; i++) {
            Rectangle varBtn = { varStartX + i * (varBtnW + varGap), varStartY, varBtnW, varBtnH };
            bool      isHov  = CheckCollisionPointRec(mouse, varBtn);

            // Deteksi klik tombol varian dan simpan pilihan
            if (isHov && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                if      (currentItemIndex == 8)  currentSloopVariant      = i;
                else if (currentItemIndex == 5)  currentPirateVariant     = i;
                else if (currentItemIndex == 6)  currentGalleonVariant    = i;
                else if (currentItemIndex == 18) currentWeatherVariant    = i;
                else if (currentItemIndex == 9)  currentLighthouseVariant = i;
            }

            // Perbarui progres hover tiap tombol varian
            if (isHov) varHovProg[i] += dt * 10.0f; else varHovProg[i] -= dt * 8.0f;
            if (varHovProg[i] > 1.0f) varHovProg[i] = 1.0f;
            if (varHovProg[i] < 0.0f) varHovProg[i] = 0.0f;

            // Varian yang sedang aktif ditampilkan dengan warna penuh (prog = 1.0)
            int currentVar = 0;
            if      (currentItemIndex == 8)  currentVar = currentSloopVariant;
            else if (currentItemIndex == 5)  currentVar = currentPirateVariant;
            else if (currentItemIndex == 6)  currentVar = currentGalleonVariant;
            else if (currentItemIndex == 18) currentVar = currentWeatherVariant;
            else if (currentItemIndex == 9)  currentVar = currentLighthouseVariant;

            float prog = (currentVar == i) ? 1.0f : varHovProg[i];
            DrawRectangleRounded(varBtn, 0.5f, 10, LerpColorUI(idleC, hovC, prog));
            DrawRectangleRoundedLines(varBtn, 0.5f, 10, LerpColorUI(outIdl, outHov, prog));

            float   varTextSize = 20.0f;
            Vector2 vnSize      = MeasureTextEx(polaFont, varNames[i], varTextSize, 1.5f);
            DrawTextEx(polaFont, varNames[i], (Vector2){varBtn.x + varBtnW/2 - vnSize.x/2, varBtn.y + varBtnH/2 - vnSize.y/2}, varTextSize, 1.5f, WHITE);
        }
    }

    // =========================================================================
    // Gambar tombol navigasi kiri "<" dan kanan ">"
    // =========================================================================
    float arrowFontSize = 40.0f;

    DrawRectangleRounded(btnLeft, 0.5f, 16, LerpColorUI(idleC, hovC, leftHovProg));
    DrawRectangleRoundedLines(btnLeft, 0.5f, 16, LerpColorUI(outIdl, outHov, leftHovProg));
    Vector2 leftArrSize = MeasureTextEx(polaFont, "<", arrowFontSize, 2.0f);
    DrawTextEx(polaFont, "<", (Vector2){ btnLeft.x + navSize/2 - leftArrSize.x/2, btnLeft.y + navSize/2 - leftArrSize.y/2 }, arrowFontSize, 2.0f, WHITE);

    DrawRectangleRounded(btnRight, 0.5f, 16, LerpColorUI(idleC, hovC, rightHovProg));
    DrawRectangleRoundedLines(btnRight, 0.5f, 16, LerpColorUI(outIdl, outHov, rightHovProg));
    Vector2 rightArrSize = MeasureTextEx(polaFont, ">", arrowFontSize, 2.0f);
    DrawTextEx(polaFont, ">", (Vector2){ btnRight.x + navSize/2 - rightArrSize.x/2, btnRight.y + navSize/2 - rightArrSize.y/2 }, arrowFontSize, 2.0f, WHITE);

    EndMode2D(); // Keluar dari kamera virtual

    DrawBackButton(); // Tombol Kembali tetap di koordinat layar asli
    return SCREEN_POLA_2D;
}

void UnloadPola2D(void) {
    if (isPolaAssetsLoaded) {
        UnloadFont(polaFont);
        UnloadTexture(bgTexturePola);
        isPolaAssetsLoaded = false;
    }
}