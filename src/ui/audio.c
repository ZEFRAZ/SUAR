/**
 * @file audio.c
 * @brief Pengelolaan musik latar dan tombol mute/unmute.
 *
 * File ini bertanggung jawab memuat file musik, memutar dan menghentikannya,
 * serta menggambar tombol mute di pojok kanan atas layar.
 */

#include "audio.h"
#include "raylib.h"
#include <math.h>
#include <stddef.h>
#include "draw_helpers.h"

// =============================================================================
// BAGIAN 1: VARIABEL STATUS INTERNAL
// =============================================================================
static Music bgmMusic;
static bool  isMuted    = false;
static bool  audioReady = false;

// Status visual tombol mute (untuk animasi hover dan font)
static float audioHoverProgress  = 0.0f;
static Font  audioFont            = { 0 };
static bool  isAudioFontLoaded    = false;

// =============================================================================
// BAGIAN 2: INISIALISASI, UPDATE, DAN PEMBERSIHAN AUDIO
// =============================================================================

/**
 * @brief Membuka perangkat audio dan mulai memutar musik latar secara berulang.
 */
void InitAudio_BGM(void) {
    InitAudioDevice();
    bgmMusic = LoadMusicStream("Lost Saga OST_ Water Park.mp3");
    if (bgmMusic.stream.buffer != NULL) {
        bgmMusic.looping = true;
        SetMusicVolume(bgmMusic, 0.6f);
        PlayMusicStream(bgmMusic);
        audioReady = true;
    }
}

/**
 * @brief Memperbarui buffer pemutaran musik setiap frame.
 *
 * Fungsi ini wajib dipanggil di setiap iterasi game loop agar musik tidak terputus.
 */
void UpdateAudio_BGM(void) {
    if (audioReady) UpdateMusicStream(bgmMusic);
}

/**
 * @brief Menghentikan musik dan melepaskan semua sumber daya audio dari memori.
 */
void UnloadAudio_BGM(void) {
    if (audioReady) {
        StopMusicStream(bgmMusic);
        UnloadMusicStream(bgmMusic);
    }
    if (isAudioFontLoaded) UnloadFont(audioFont);
    CloseAudioDevice();
}

// =============================================================================
// BAGIAN 3: UTILITAS INTERNAL TOMBOL
// =============================================================================

/**
 * @brief Menghitung nilai antara dua angka berdasarkan persentase (interpolasi linear).
 *
 * Digunakan untuk membuat animasi hover yang bertransisi secara mulus.
 *
 * @param start  Nilai awal.
 * @param end    Nilai akhir.
 * @param amount Persentase (0.0 = awal, 1.0 = akhir).
 * @return       Nilai hasil interpolasi.
 */
static float LerpBtn(float start, float end, float amount) {
    return start + amount * (end - start);
}

/**
 * @brief Menghitung ukuran dan posisi tombol mute sesuai ukuran layar saat ini.
 *
 * Tombol selalu diposisikan di pojok kanan atas dan menyesuaikan skala otomatis.
 *
 * @return Rectangle Area tombol mute di layar.
 */
static Rectangle GetAudioBtnHitbox(void) {
    int   screenW = GetScreenWidth();
    int   screenH = GetScreenHeight();
    float scaleX  = (float)screenW / 1920.0f;
    float scaleY  = (float)screenH / 1080.0f;
    float scale   = (scaleX < scaleY) ? scaleX : scaleY;
    if (scale < 0.5f) scale = 0.5f;

    float btnWidth  = 160.0f * scale;
    float btnHeight = 55.0f  * scale;
    float baseX     = screenW - btnWidth - (30.0f * scale);
    float baseY     = 30.0f * scale;

    return (Rectangle){ baseX, baseY, btnWidth, btnHeight };
}

// =============================================================================
// BAGIAN 4: PENGGAMBARAN TOMBOL MUTE
// =============================================================================

/**
 * @brief Menggambar tombol mute/unmute di pojok kanan atas layar.
 *
 * Tombol ini memiliki efek hover (berubah warna saat kursor berada di atasnya)
 * dan ikon speaker yang berbeda saat audio dimatikan.
 * Klik tombol ini akan menyalakan atau mematikan musik latar.
 */
void DrawMuteButton(void) {
    if (!isAudioFontLoaded) {
        audioFont         = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0);
        isAudioFontLoaded = true;
    }

    float   dt         = GetFrameTime();
    Vector2 mousePoint = GetMousePosition();

    Rectangle outerRec = GetAudioBtnHitbox();
    bool      isHovered = CheckCollisionPointRec(mousePoint, outerRec);

    // Perbarui progres animasi hover (0.0 = normal, 1.0 = hover penuh)
    if (isHovered) audioHoverProgress += dt * 8.0f;
    else           audioHoverProgress -= dt * 6.0f;
    if (audioHoverProgress > 1.0f) audioHoverProgress = 1.0f;
    if (audioHoverProgress < 0.0f) audioHoverProgress = 0.0f;

    // Proses klik: ganti status mute dan sesuaikan volume
    if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isMuted = !isMuted;
        if (audioReady) SetMusicVolume(bgmMusic, isMuted ? 0.0f : 0.6f);
    }

    float scale = outerRec.height / 55.0f;

    // Layer 1: Bayangan lembut di belakang tombol
    Rectangle shadowRec = { outerRec.x + (6.0f * scale), outerRec.y + (8.0f * scale), outerRec.width, outerRec.height };
    DrawRectangleRounded(shadowRec, 0.6f, 16, (Color){ 0, 0, 0, 60 });

    // Layer 2: Garis tepi bercahaya yang makin tebal saat hover
    float currentThick  = LerpBtn(3.0f * scale, 6.0f * scale, audioHoverProgress);
    Color outColorIdle  = (Color){ 30, 100, 150, 255 };
    Color outColorHover = (Color){ 0, 255, 255, 255 };
    Color currentOutCol = {
        (unsigned char)LerpBtn(outColorIdle.r, outColorHover.r, audioHoverProgress),
        (unsigned char)LerpBtn(outColorIdle.g, outColorHover.g, audioHoverProgress),
        (unsigned char)LerpBtn(outColorIdle.b, outColorHover.b, audioHoverProgress), 255
    };
    Rectangle outlineRec = { outerRec.x - currentThick, outerRec.y - currentThick, outerRec.width + (currentThick * 2), outerRec.height + (currentThick * 2) };
    DrawRectangleRounded(outlineRec, 0.6f, 24, currentOutCol);

    // Layer 3: Warna latar tombol (berubah saat hover)
    Color idleColor       = (Color){ 10, 45, 80, 255 };
    Color hoverColor      = (Color){ 0, 130, 180, 255 };
    Color baseSolidColor  = (Color){
        (unsigned char)LerpBtn(idleColor.r, hoverColor.r, audioHoverProgress),
        (unsigned char)LerpBtn(idleColor.g, hoverColor.g, audioHoverProgress),
        (unsigned char)LerpBtn(idleColor.b, hoverColor.b, audioHoverProgress), 255
    };
    DrawRectangleRounded(outerRec, 0.6f, 24, baseSolidColor);

    // Layer 4: Efek kilap di bagian atas tombol
    Rectangle glossRec = { outerRec.x + 2, outerRec.y + 2, outerRec.width - 4, (outerRec.height * 0.45f) };
    DrawRectangleRounded(glossRec, 0.6f, 16, (Color){ 255, 255, 255, 25 });

    // Layer 5: Ikon speaker
    float iconX = outerRec.x + (25.0f * scale);
    float iconY = outerRec.y + (outerRec.height / 2.0f);

    Color iconIdle = (Color){ 150, 160, 170, 255 };
    Color iconGold = (Color){ 255, 215,   0, 255 };
    Color iconCol  = (Color){
        (unsigned char)LerpBtn(iconIdle.r, iconGold.r, audioHoverProgress),
        (unsigned char)LerpBtn(iconIdle.g, iconGold.g, audioHoverProgress),
        (unsigned char)LerpBtn(iconIdle.b, iconGold.b, audioHoverProgress), 255
    };

    // Kotak dasar speaker
    DrawRectangle((int)(iconX - 6*scale), (int)(iconY - 5*scale), (int)(6*scale), (int)(10*scale), iconCol);

    // Corong speaker (dua segitiga)
    FillTri_Fast((Vector2){iconX, iconY - 5*scale}, (Vector2){iconX, iconY + 5*scale}, (Vector2){iconX + 10*scale, iconY - 12*scale}, iconCol);
    FillTri_Fast((Vector2){iconX, iconY + 5*scale}, (Vector2){iconX + 10*scale, iconY + 12*scale}, (Vector2){iconX + 10*scale, iconY - 12*scale}, iconCol);

    if (!isMuted) {
        // Ikon gelombang suara saat audio menyala
        DrawRing((Vector2){iconX + 8*scale, iconY}, 6*scale, 9*scale, -45, 45, 10, iconCol);
        DrawRing((Vector2){iconX + 8*scale, iconY}, 13*scale, 16*scale, -40, 40, 10, iconCol);
    } else {
        // Tanda silang merah saat audio dimatikan
        Color redCross = (Color){ 255, 60, 60, 255 };
        float cx       = iconX + 18*scale;
        DrawLineEx((Vector2){cx - 6*scale, iconY - 6*scale}, (Vector2){cx + 6*scale, iconY + 6*scale}, 4*scale, redCross);
        DrawLineEx((Vector2){cx - 6*scale, iconY + 6*scale}, (Vector2){cx + 6*scale, iconY - 6*scale}, 4*scale, redCross);
    }

    // Layer 6: Teks label di dalam tombol
    Color       textColor = (audioHoverProgress > 0.5f) ? WHITE : (Color){ 200, 220, 240, 255 };
    const char* text      = isMuted ? "MUTE" : "AUDIO";
    float       fontSize  = 21.0f * scale;
    float       textX     = iconX + (36.0f * scale);
    Vector2     textSize  = MeasureTextEx(audioFont, text, fontSize, 2.0f);
    float       textY     = outerRec.y + (outerRec.height / 2.0f) - (textSize.y / 2.0f);

    // Bayangan teks
    DrawTextEx(audioFont, text, (Vector2){textX, textY + 2.0f}, fontSize, 2.0f, (Color){ 10, 20, 30, 150 });
    // Teks utama
    DrawTextEx(audioFont, text, (Vector2){textX, textY}, fontSize, 2.0f, textColor);
}