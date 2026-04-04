/**
 * @file back_button.c
 * @brief Penggambaran dan pendeteksian klik tombol "Kembali".
 *
 * File ini mengurusi tampilan tombol Kembali di pojok kiri atas layar,
 * termasuk animasi hover dan deteksi klik mouse maupun keyboard.
 */

#include "back_button.h"
#include "raylib.h"
#include <math.h>

// =============================================================================
// BAGIAN 1: VARIABEL STATUS INTERNAL
// =============================================================================

// Progres animasi hover (0.0 = normal, 1.0 = hover penuh)
static float backHoverProgress = 0.0f;
static Font  backFont          = { 0 };
static bool  isFontLoaded      = false;

// =============================================================================
// BAGIAN 2: UTILITAS INTERNAL
// =============================================================================

/**
 * @brief Menghitung nilai antara dua angka berdasarkan persentase (interpolasi linear).
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
 * @brief Menghitung ukuran dan posisi tombol Kembali sesuai ukuran layar saat ini.
 *
 * Tombol selalu diposisikan di pojok kiri atas dan menyesuaikan skala secara otomatis.
 *
 * @return Rectangle Area tombol di layar.
 */
static Rectangle GetBackBtnHitbox(void) {
    int   screenW = GetScreenWidth();
    int   screenH = GetScreenHeight();
    float scaleX  = (float)screenW / 1920.0f;
    float scaleY  = (float)screenH / 1080.0f;
    float scale   = (scaleX < scaleY) ? scaleX : scaleY;
    if (scale < 0.5f) scale = 0.5f;

    float btnWidth  = 160.0f * scale;
    float btnHeight = 55.0f  * scale;
    float baseX     = 30.0f  * scale;
    float baseY     = 30.0f  * scale;

    return (Rectangle){ baseX, baseY, btnWidth, btnHeight };
}

// =============================================================================
// BAGIAN 3: FUNGSI UTAMA
// =============================================================================

/**
 * @brief Mengecek apakah tombol Kembali ditekan.
 *
 * Tombol dianggap ditekan jika pengguna mengklik area tombol di layar,
 * atau menekan tombol ESC maupun Backspace di keyboard.
 *
 * @return 1 jika tombol ditekan, 0 jika tidak.
 */
int BackButtonPressed(void) {
    Rectangle hitBox = GetBackBtnHitbox();
    Vector2   mouse  = GetMousePosition();

    if (CheckCollisionPointRec(mouse, hitBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) return 1;
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) return 1;
    return 0;
}

/**
 * @brief Menggambar tombol Kembali di pojok kiri atas layar.
 *
 * Tombol memiliki efek hover (berubah warna saat kursor berada di atasnya)
 * dan ikon panah ke kiri sebagai penanda navigasi.
 */
void DrawBackButton(void) {
    // Muat font khusus sekali saja saat pertama kali dipanggil
    if (!isFontLoaded) {
        backFont     = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0);
        isFontLoaded = true;
    }

    float   dt         = GetFrameTime();
    Vector2 mousePoint = GetMousePosition();

    Rectangle outerRec = GetBackBtnHitbox();
    bool      isHovered = CheckCollisionPointRec(mousePoint, outerRec);

    // Perbarui progres animasi hover (0.0 = normal, 1.0 = hover penuh)
    if (isHovered) backHoverProgress += dt * 8.0f;
    else           backHoverProgress -= dt * 6.0f;
    if (backHoverProgress > 1.0f) backHoverProgress = 1.0f;
    if (backHoverProgress < 0.0f) backHoverProgress = 0.0f;

    float scale = outerRec.height / 55.0f;

    // Layer 1: Bayangan lembut di belakang tombol
    Rectangle shadowRec = { outerRec.x + (6.0f * scale), outerRec.y + (8.0f * scale), outerRec.width, outerRec.height };
    DrawRectangleRounded(shadowRec, 0.6f, 16, (Color){ 0, 0, 0, 60 });

    // Layer 2: Garis tepi bercahaya yang makin tebal saat hover
    float outlineIdle  = 3.0f * scale;
    float outlineHover = 6.0f * scale;
    float currentThick = LerpBtn(outlineIdle, outlineHover, backHoverProgress);

    Color outColorIdle  = (Color){ 30, 100, 150, 255 };
    Color outColorHover = (Color){ 0, 255, 255, 255 };
    Color currentOutCol = {
        (unsigned char)LerpBtn(outColorIdle.r, outColorHover.r, backHoverProgress),
        (unsigned char)LerpBtn(outColorIdle.g, outColorHover.g, backHoverProgress),
        (unsigned char)LerpBtn(outColorIdle.b, outColorHover.b, backHoverProgress), 255
    };
    Rectangle outlineRec = {
        outerRec.x - currentThick, outerRec.y - currentThick,
        outerRec.width + (currentThick * 2), outerRec.height + (currentThick * 2)
    };
    DrawRectangleRounded(outlineRec, 0.6f, 24, currentOutCol);

    // Layer 3: Warna latar tombol (berubah saat hover)
    Color idleColor      = (Color){ 10, 45, 80, 255 };
    Color hoverColor     = (Color){ 0, 130, 180, 255 };
    Color baseSolidColor = (Color){
        (unsigned char)LerpBtn(idleColor.r, hoverColor.r, backHoverProgress),
        (unsigned char)LerpBtn(idleColor.g, hoverColor.g, backHoverProgress),
        (unsigned char)LerpBtn(idleColor.b, hoverColor.b, backHoverProgress), 255
    };
    DrawRectangleRounded(outerRec, 0.6f, 24, baseSolidColor);

    // Layer 4: Efek kilap di bagian atas tombol
    Rectangle glossRec = { outerRec.x + 2, outerRec.y + 2, outerRec.width - 4, (outerRec.height * 0.45f) };
    DrawRectangleRounded(glossRec, 0.6f, 16, (Color){ 255, 255, 255, 25 });

    // Layer 5: Ikon panah kiri
    float iconX = outerRec.x + (28.0f * scale);
    float iconY = outerRec.y + (outerRec.height / 2.0f);

    Color iconIdle = (Color){ 150, 160, 170, 255 };
    Color iconGold = (Color){ 255, 215,   0, 255 };
    Color iconCol  = (Color){
        (unsigned char)LerpBtn(iconIdle.r, iconGold.r, backHoverProgress),
        (unsigned char)LerpBtn(iconIdle.g, iconGold.g, backHoverProgress),
        (unsigned char)LerpBtn(iconIdle.b, iconGold.b, backHoverProgress), 255
    };

    // Lingkaran dekoratif ikon
    DrawCircleLines((int)iconX, (int)iconY, 14 * scale, iconCol);
    DrawCircleLines((int)iconX, (int)iconY, 10 * scale, Fade(iconCol, 0.5f));

    // Bentuk panah ke kiri
    float thick = 2.5f * scale;
    DrawLineEx((Vector2){iconX - (4*scale), iconY}, (Vector2){iconX + (6*scale), iconY}, thick, iconCol);
    DrawLineEx((Vector2){iconX - (5*scale), iconY}, (Vector2){iconX + (1*scale), iconY - (6*scale)}, thick, iconCol);
    DrawLineEx((Vector2){iconX - (5*scale), iconY}, (Vector2){iconX + (1*scale), iconY + (6*scale)}, thick, iconCol);

    // Layer 6: Teks label "KEMBALI"
    Color       textColor = (backHoverProgress > 0.5f) ? WHITE : (Color){ 200, 220, 240, 255 };
    const char* text      = "KEMBALI";
    float       fontSize  = 21.0f * scale;
    float       textX     = iconX + (22.0f * scale);
    Vector2     textSize  = MeasureTextEx(backFont, text, fontSize, 2.0f);
    float       textY     = outerRec.y + (outerRec.height / 2.0f) - (textSize.y / 2.0f);

    DrawTextEx(backFont, text, (Vector2){textX, textY + 2.0f}, fontSize, 2.0f, (Color){ 10, 20, 30, 150 }); // Bayangan teks
    DrawTextEx(backFont, text, (Vector2){textX, textY}, fontSize, 2.0f, textColor);                           // Teks utama
}
