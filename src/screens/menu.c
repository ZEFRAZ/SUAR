/**
 * @file menu.c
 * @brief Layar Menu Utama aplikasi Proyek SUAR.
 *
 * File ini mengurusi tampilan menu utama yang terdiri dari:
 * - Judul besar "S U A R" dengan efek bayangan dan warna emas pada huruf A.
 * - Tombol-tombol navigasi menuju setiap layar (Pola 2D, Animasi 2D, About, Guide, Exit).
 * - Efek hover pada tombol (perubahan warna dan ukuran secara mulus).
 * - Dekorasi tali penghubung antar tombol layaknya kapal.
 */

#include "menu.h"
#include <string.h>
#include <math.h>

#define NUM_BUTTONS 5

// Data tombol navigasi
static CustomButton buttons[NUM_BUTTONS];
static const char*  btnLabels[NUM_BUTTONS] = {"Pola 2D", "Animasi 2D", "About", "Guide", "Exit"};

// Aset visual menu
static Texture2D bgTexture;   // Gambar latar belakang
static Font      customFont;  // Font untuk teks tombol
static Font      titleFont;   // Font khusus untuk judul besar "SUAR"

// =============================================================================
// BAGIAN 1: UTILITAS INTERNAL
// =============================================================================

/**
 * @brief Menghitung nilai antara dua angka berdasarkan persentase (interpolasi linear).
 *
 * Digunakan untuk membuat animasi hover yang bertransisi secara mulus,
 * misalnya perubahan warna dan ukuran tombol saat kursor menyentuhnya.
 *
 * @param start  Nilai awal.
 * @param end    Nilai akhir.
 * @param amount Persentase (0.0 = awal, 1.0 = akhir).
 * @return       Nilai hasil interpolasi.
 */
static float Lerp(float start, float end, float amount) {
    return start + amount * (end - start);
}

// =============================================================================
// BAGIAN 2: INISIALISASI DAN PEMBERSIHAN ASET
// =============================================================================

/**
 * @brief Memuat semua aset menu (gambar latar, font) dan mengatur nilai awal tombol.
 *
 * Wajib dipanggil sekali sebelum UpdateDrawMenu() pertama kali dijalankan.
 */
void InitMenu(void) {
    bgTexture  = LoadTexture("background.png");
    customFont = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0); // Font untuk teks tombol
    titleFont  = LoadFontEx("Starbim.ttf", 150, 0, 0);           // Font untuk judul besar

    // Atur nilai awal semua tombol
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].text          = btnLabels[i];
        buttons[i].hoverProgress = 0.0f;
    }
}

/**
 * @brief Melepaskan semua aset menu dari memori saat program ditutup.
 *
 * Wajib dipanggil sebelum program selesai agar tidak ada memori yang bocor.
 */
void UnloadMenu(void) {
    UnloadTexture(bgTexture);
    UnloadFont(customFont);
    UnloadFont(titleFont);
}

// =============================================================================
// BAGIAN 3: UPDATE DAN PENGGAMBARAN MENU
// =============================================================================

/**
 * @brief Menjalankan logika dan penggambaran Menu Utama setiap frame.
 *
 * Fungsi ini menghitung animasi hover setiap tombol, menggambar judul,
 * dekorasi tali, semua tombol navigasi beserta ikonnya, dan mendeteksi
 * klik tombol untuk menentukan layar mana yang akan dibuka berikutnya.
 * Semua ukuran UI menyesuaikan ukuran window secara otomatis.
 *
 * @return ScreenState Layar berikutnya yang akan dibuka sesuai tombol yang ditekan.
 */
ScreenState UpdateDrawMenu(void) {
    float   dt          = GetFrameTime();
    Vector2 mousePoint  = GetMousePosition();
    ScreenState nextScreen = SCREEN_MENU;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Gambar latar belakang penuh
    ClearBackground(RAYWHITE);
    Rectangle sourceRec = { 0.0f, 0.0f, (float)bgTexture.width, (float)bgTexture.height };
    Rectangle destRec   = { 0.0f, 0.0f, (float)screenW, (float)screenH };
    DrawTexturePro(bgTexture, sourceRec, destRec, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);

    // Hitung faktor skala agar UI proporsional di semua ukuran layar
    float scaleX = (float)screenW / 1920.0f;
    float scaleY = (float)screenH / 1080.0f;
    float scale  = (scaleX < scaleY) ? scaleX : scaleY;
    if (scale < 0.4f) scale = 0.4f;

    // =========================================================================
    // Hitung ukuran dan posisi setiap tombol navigasi
    // =========================================================================
    float btnWidth    = 540.0f * scale;
    float btnHeight   = 85.0f  * scale;
    float btnSpacing  = 105.0f * scale;
    float btnFontSize = 50.0f  * scale;

    float rightMargin = 220.0f * scale;
    float baseX       = screenW - btnWidth - rightMargin;

    float buttonsBlockHeight = NUM_BUTTONS * btnSpacing;
    float yOffset            = 180.0f * scale;
    float startY             = ((screenH - buttonsBlockHeight) / 2.0f) + yOffset;

    // =========================================================================
    // Gambar tali dekoratif di belakang tombol (dua garis vertikal cokelat)
    // =========================================================================
    float ropeLeftX  = baseX + (60.0f * scale);
    float ropeRightX = baseX + btnWidth - (60.0f * scale);
    float ropeStartY = startY - (220.0f * scale);
    float ropeEndY   = startY + buttonsBlockHeight - btnSpacing + btnHeight + (25.0f * scale);

    // Tali kiri (dua lapisan: tebal gelap + tipis terang)
    DrawLineEx((Vector2){ropeLeftX, ropeStartY}, (Vector2){ropeLeftX, ropeEndY}, 12.0f * scale, (Color){ 101, 67, 33, 255 });
    DrawLineEx((Vector2){ropeLeftX - 3.0f*scale, ropeStartY}, (Vector2){ropeLeftX - 3.0f*scale, ropeEndY}, 4.0f * scale, (Color){ 205, 133, 63, 255 });

    // Tali kanan (dua lapisan: tebal gelap + tipis terang)
    DrawLineEx((Vector2){ropeRightX, ropeStartY}, (Vector2){ropeRightX, ropeEndY}, 12.0f * scale, (Color){ 101, 67, 33, 255 });
    DrawLineEx((Vector2){ropeRightX - 3.0f*scale, ropeStartY}, (Vector2){ropeRightX - 3.0f*scale, ropeEndY}, 4.0f * scale, (Color){ 205, 133, 63, 255 });

    // =========================================================================
    // Gambar kotak judul "SUAR" di atas tombol
    // =========================================================================
    float titleBoxWidth  = btnWidth + (80.0f * scale);
    float titleBoxHeight = 170.0f * scale;
    float titleBoxX      = baseX - (40.0f * scale);
    float gapTitleToBtn  = 50.0f * scale;
    float titleBoxY      = startY - titleBoxHeight - gapTitleToBtn;

    // A. Bayangan di belakang kotak judul
    DrawRectangleRounded((Rectangle){titleBoxX + 12*scale, titleBoxY + 15*scale, titleBoxWidth, titleBoxHeight}, 0.5f, 24, (Color){ 0, 0, 0, 80 });

    // B. Garis tepi kotak judul (cokelat tua)
    float tStroke = 8.0f * scale;
    DrawRectangleRounded((Rectangle){titleBoxX - tStroke, titleBoxY - tStroke, titleBoxWidth + (tStroke*2), titleBoxHeight + (tStroke*2)}, 0.5f, 24, (Color){ 101, 67, 33, 255 });

    // C. Warna dasar kotak judul (merah tua)
    DrawRectangleRounded((Rectangle){titleBoxX, titleBoxY, titleBoxWidth, titleBoxHeight}, 0.5f, 24, (Color){ 160, 30, 45, 255 });
    DrawRectangleRounded((Rectangle){titleBoxX + 5*scale, titleBoxY + 5*scale, titleBoxWidth - 10*scale, titleBoxHeight - 10*scale}, 0.4f, 24, (Color){ 180, 40, 55, 255 });

    // D. Teks judul "S U A R" — huruf A diberi warna emas agar berbeda
    const char* titleText         = "S U A R";
    float       titleFontSize     = 140.0f * scale;
    float       titleLetterSpacing = 10.0f * scale;

    Vector2 titleSize  = MeasureTextEx(titleFont, titleText, titleFontSize, titleLetterSpacing);
    float   titleTextX = titleBoxX + (titleBoxWidth / 2.0f) - (titleSize.x / 2.0f);
    float   titleTextY = titleBoxY + (titleBoxHeight / 2.0f) - (titleSize.y / 2.0f) - (5.0f * scale);

    // Teks dipecah tiga bagian agar huruf A bisa diberi warna berbeda
    const char* part1 = "S U ";
    const char* part2 = "A";
    const char* part3 = " R";

    float x1 = titleTextX;
    float x2 = titleTextX + MeasureTextEx(titleFont, part1, titleFontSize, titleLetterSpacing).x;
    float x3 = titleTextX + MeasureTextEx(titleFont, "S U A", titleFontSize, titleLetterSpacing).x;

    Color shadowCol = (Color){ 60, 10, 15, 180 };
    Color goldCol   = (Color){ 255, 220, 20, 255 };

    // Gambar efek bayangan teks (geser sedikit ke bawah-kanan)
    DrawTextEx(titleFont, part1, (Vector2){x1 + 6*scale, titleTextY + 6*scale}, titleFontSize, titleLetterSpacing, shadowCol);
    DrawTextEx(titleFont, part2, (Vector2){x2 + 6*scale, titleTextY + 6*scale}, titleFontSize, titleLetterSpacing, shadowCol);
    DrawTextEx(titleFont, part3, (Vector2){x3 + 6*scale, titleTextY + 6*scale}, titleFontSize, titleLetterSpacing, shadowCol);

    // Gambar teks judul utama (putih kecuali huruf A yang emas)
    DrawTextEx(titleFont, part1, (Vector2){x1, titleTextY}, titleFontSize, titleLetterSpacing, WHITE);
    DrawTextEx(titleFont, part2, (Vector2){x2, titleTextY}, titleFontSize, titleLetterSpacing, goldCol);
    DrawTextEx(titleFont, part3, (Vector2){x3, titleTextY}, titleFontSize, titleLetterSpacing, WHITE);

    // E. Sub-judul "PROYEK ANIMASI MERCUSUAR" di bawah kotak judul
    const char* subText   = "PROYEK ANIMASI MERCUSUAR";
    float       subFontSize = 20.0f * scale;
    Vector2     subSize   = MeasureTextEx(customFont, subText, subFontSize, 2.0f);

    Rectangle subBg = {
        titleBoxX + (titleBoxWidth / 2.0f) - (subSize.x / 2.0f) - 20*scale,
        titleBoxY + titleBoxHeight - (15*scale),
        subSize.x + 40*scale, subSize.y + 16*scale
    };

    DrawRectangleRounded((Rectangle){subBg.x - 4*scale, subBg.y - 4*scale, subBg.width + 8*scale, subBg.height + 8*scale}, 0.8f, 16, (Color){ 101, 67, 33, 255 }); // Garis tepi
    DrawRectangleRounded(subBg, 0.8f, 16, WHITE);   // Kotak latar putih
    DrawTextEx(customFont, subText, (Vector2){baseX + (btnWidth / 2.0f) - (subSize.x / 2.0f), subBg.y + 7*scale}, subFontSize, 2.0f, (Color){ 160, 30, 45, 255 }); // Teks merah

    // =========================================================================
    // Proses animasi hover dan gambar setiap tombol navigasi
    // =========================================================================
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].rect = (Rectangle){ baseX, startY + (i * btnSpacing), btnWidth, btnHeight };
        bool isHovered  = CheckCollisionPointRec(mousePoint, buttons[i].rect);

        // Perbarui progres hover dan deteksi klik
        if (isHovered) {
            buttons[i].hoverProgress += dt * 8.0f;
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if      (i == 0) nextScreen = SCREEN_POLA_2D;
                else if (i == 1) nextScreen = SCREEN_ANIMASI_2D;
                else if (i == 2) nextScreen = SCREEN_ABOUT;
                else if (i == 3) nextScreen = SCREEN_GUIDE;
                else if (i == 4) nextScreen = SCREEN_EXIT;
            }
        } else {
            buttons[i].hoverProgress -= dt * 6.0f;
        }
        if (buttons[i].hoverProgress > 1.0f) buttons[i].hoverProgress = 1.0f;
        if (buttons[i].hoverProgress < 0.0f) buttons[i].hoverProgress = 0.0f;

        // Tombol melebar ke kiri saat di-hover
        float hoverExtension = 50.0f * scale;
        float currentWidth   = Lerp(btnWidth, btnWidth + hoverExtension, buttons[i].hoverProgress);
        float currentX       = Lerp(baseX, baseX - (hoverExtension / 2.0f), buttons[i].hoverProgress);
        Rectangle outerRec   = { currentX, buttons[i].rect.y, currentWidth, btnHeight };

        // Layer 1: Bayangan tombol
        Rectangle shadowRec = { outerRec.x + 8.0f, outerRec.y + 10.0f, outerRec.width, outerRec.height };
        DrawRectangleRounded(shadowRec, 0.6f, 16, (Color){ 0, 0, 0, 60 });

        // Layer 2: Garis tepi tombol yang menyala saat di-hover
        float outlineIdle  = 4.0f * scale;
        float outlineHover = 8.0f * scale;
        float currentThick = Lerp(outlineIdle, outlineHover, buttons[i].hoverProgress);

        Color outColorIdle  = (Color){ 30, 100, 150, 255 };
        Color outColorHover = (Color){ 0, 255, 255, 255 };
        Color currentOutCol = {
            (unsigned char)Lerp(outColorIdle.r, outColorHover.r, buttons[i].hoverProgress),
            (unsigned char)Lerp(outColorIdle.g, outColorHover.g, buttons[i].hoverProgress),
            (unsigned char)Lerp(outColorIdle.b, outColorHover.b, buttons[i].hoverProgress), 255
        };
        Rectangle outlineRec = {
            outerRec.x - currentThick, outerRec.y - currentThick,
            outerRec.width + (currentThick * 2), outerRec.height + (currentThick * 2)
        };
        DrawRectangleRounded(outlineRec, 0.6f, 24, currentOutCol);

        // Layer 3: Warna dasar tombol (berubah biru saat hover)
        Color idleColor  = (Color){ 10, 45, 80, 255 };
        Color hoverColor = (Color){ 0, 130, 180, 255 };

        unsigned char r          = (unsigned char)Lerp(idleColor.r, hoverColor.r, buttons[i].hoverProgress);
        unsigned char g          = (unsigned char)Lerp(idleColor.g, hoverColor.g, buttons[i].hoverProgress);
        unsigned char b          = (unsigned char)Lerp(idleColor.b, hoverColor.b, buttons[i].hoverProgress);
        Color baseSolidColor     = (Color){ r, g, b, 255 };
        DrawRectangleRounded(outerRec, 0.6f, 24, baseSolidColor);

        // Layer 4: Efek kilap di bagian atas tombol
        Rectangle glossRec = { outerRec.x + 2, outerRec.y + 2, outerRec.width - 4, (outerRec.height * 0.45f) };
        DrawRectangleRounded(glossRec, 0.6f, 16, (Color){ 255, 255, 255, 25 });

        // Layer 5: Ikon unik untuk setiap tombol
        float iconX = outerRec.x + (60.0f * scale);
        float iconY = outerRec.y + (btnHeight / 2.0f);

        Color iconIdle = (Color){ 150, 160, 170, 255 };
        Color iconGold = (Color){ 255, 215,   0, 255 };
        unsigned char ir = (unsigned char)Lerp(iconIdle.r, iconGold.r, buttons[i].hoverProgress);
        unsigned char ig = (unsigned char)Lerp(iconIdle.g, iconGold.g, buttons[i].hoverProgress);
        unsigned char ib = (unsigned char)Lerp(iconIdle.b, iconGold.b, buttons[i].hoverProgress);
        Color iconCol = (Color){ ir, ig, ib, 255 };
        float s = scale;

        switch (i) {
            case 0: // Ikon Pola 2D — bentuk kompas lingkaran dengan jarum
                DrawCircleLines((int)iconX, (int)iconY, 12*s, iconCol);
                DrawCircleLines((int)iconX, (int)iconY, 8*s, iconCol);
                DrawTriangle((Vector2){iconX-4*s, iconY}, (Vector2){iconX+4*s, iconY}, (Vector2){iconX, iconY-14*s}, iconCol);
                DrawTriangle((Vector2){iconX-4*s, iconY}, (Vector2){iconX, iconY+14*s}, (Vector2){iconX+4*s, iconY}, iconCol);
                break;
            case 1: // Ikon Animasi 2D — bentuk sosok manusia sederhana
                DrawCircleLines((int)iconX, (int)iconY - 10*s, 4*s, iconCol);
                DrawLineEx((Vector2){iconX - 10*s, iconY - 4*s}, (Vector2){iconX + 10*s, iconY - 4*s}, 2*s, iconCol);
                DrawLineEx((Vector2){iconX, iconY - 6*s}, (Vector2){iconX, iconY + 12*s}, 3*s, iconCol);
                DrawLineEx((Vector2){iconX - 12*s, iconY + 4*s}, (Vector2){iconX, iconY + 14*s}, 3*s, iconCol);
                DrawLineEx((Vector2){iconX + 12*s, iconY + 4*s}, (Vector2){iconX, iconY + 14*s}, 3*s, iconCol);
                break;
            case 2: // Ikon About — bentuk crosshair dengan garis merah
                DrawRing((Vector2){iconX, iconY}, 6*s, 12*s, 0, 360, 32, iconCol);
                DrawRectanglePro((Rectangle){iconX, iconY-12*s, 4*s, 6*s}, (Vector2){2*s, 0}, 0, (buttons[i].hoverProgress > 0.5f) ? RED : (Color){200,50,50,255});
                DrawRectanglePro((Rectangle){iconX, iconY+6*s,  4*s, 6*s}, (Vector2){2*s, 0}, 0, (buttons[i].hoverProgress > 0.5f) ? RED : (Color){200,50,50,255});
                DrawRectanglePro((Rectangle){iconX-12*s, iconY, 6*s, 4*s}, (Vector2){0, 2*s}, 0, (buttons[i].hoverProgress > 0.5f) ? RED : (Color){200,50,50,255});
                DrawRectanglePro((Rectangle){iconX+6*s,  iconY, 6*s, 4*s}, (Vector2){0, 2*s}, 0, (buttons[i].hoverProgress > 0.5f) ? RED : (Color){200,50,50,255});
                break;
            case 3: // Ikon Guide — bentuk roda/matahari dengan 8 jari
                DrawCircleLines((int)iconX, (int)iconY, 9*s, iconCol);
                DrawCircle((int)iconX, (int)iconY, 3*s, iconCol);
                for (int j = 0; j < 8; j++) {
                    float angle = j * 45.0f * DEG2RAD;
                    DrawLineEx((Vector2){iconX, iconY}, (Vector2){iconX + cosf(angle)*14*s, iconY + sinf(angle)*14*s}, 2*s, iconCol);
                }
                break;
            case 4: // Ikon Exit — bentuk tanda tanya atau pintu keluar
                DrawLineEx((Vector2){iconX+2*s, iconY-12*s}, (Vector2){iconX+2*s, iconY+4*s}, 2*s, iconCol);
                DrawRing((Vector2){iconX-2*s, iconY+4*s}, 2*s, 4*s, 0, 180, 16, iconCol);
                DrawTriangle((Vector2){iconX-8*s, iconY+4*s}, (Vector2){iconX-4*s, iconY}, (Vector2){iconX-8*s, iconY-2*s}, iconCol);
                DrawCircleLines((int)iconX+2*s, (int)iconY-14*s, 2*s, iconCol);
                break;
        }

        // Layer 6: Teks label di tengah tombol
        Color textColor = (buttons[i].hoverProgress > 0.5f) ? WHITE : (Color){ 200, 220, 240, 255 };

        Vector2 textSize = MeasureTextEx(customFont, buttons[i].text, btnFontSize, 2.0f);
        float   textX    = outerRec.x + (outerRec.width / 2.0f) - (textSize.x / 2.0f);
        float   textY    = outerRec.y + (outerRec.height / 2.0f) - (textSize.y / 2.0f);

        DrawTextEx(customFont, buttons[i].text, (Vector2){textX, textY + 2.0f}, btnFontSize, 2.0f, (Color){ 10, 20, 30, 150 }); // Bayangan teks
        DrawTextEx(customFont, buttons[i].text, (Vector2){textX, textY}, btnFontSize, 2.0f, textColor);                           // Teks utama
    }

    return nextScreen;
}