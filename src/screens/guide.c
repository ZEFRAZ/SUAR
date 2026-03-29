#include "guide.h"
#include "back_button.h"
#include "raymath.h"

static Font guideFont = { 0 };
static Texture2D bgTextureGuide = { 0 };
static bool isGuideAssetsLoaded = false;

/**
 * @brief Fungsi perulangan (Update & Draw) untuk layar Panduan (Guide).
 * 
 * Mengatur tampilan teks instruksi penggunaan aplikasi.
 * Sama seperti menu About, menggunakan fitur Camera2D agar panel panduan
 * tetap terlihat rapi dan tidak rusak ketika ukuran window diubah (di-resize).
 * 
 * @return ScreenState Layar target berikutnya (kembali ke SCREEN_MENU jika ditekan Back).
 */
ScreenState UpdateDrawGuide(void) {
    if (!isGuideAssetsLoaded) {
        guideFont = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0);
        bgTextureGuide = LoadTexture("background_pola2d.png");
        isGuideAssetsLoaded = true;
    }

    int W = GetScreenWidth();
    int H = GetScreenHeight();
    
    // 1. SETUP KAMERA VIRTUAL (Skalabilitas Resolusi Dinamis)
    Camera2D camera = { 0 };
    camera.target = (Vector2){ 1920.0f / 2.0f, 1080.0f / 2.0f };
    camera.offset = (Vector2){ W / 2.0f, H / 2.0f };
    camera.rotation = 0.0f;
    float scaleX = (float)W / 1920.0f;
    float scaleY = (float)H / 1080.0f;
    camera.zoom = (scaleX < scaleY) ? scaleX : scaleY;

    if (BackButtonPressed()) return SCREEN_MENU;

    // 2. MENGGAMBAR BACKGROUND WALLPAPER
    if (bgTextureGuide.id != 0) {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)bgTextureGuide.width, (float)bgTextureGuide.height };
        Rectangle destRec = { 0.0f, 0.0f, (float)W, (float)H };
        DrawTexturePro(bgTextureGuide, sourceRec, destRec, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        ClearBackground((Color){ 20, 25, 35, 255 });
    }

    // 3. MULAI RENDER UI DENGAN KAMERA VIRTUAL
    BeginMode2D(camera);

    float canvasW = 1100.0f; // Ekstensi Margin Horizontal Kanvas Informasional
    float canvasH = 750.0f;
    float canvasX = (1920.0f - canvasW) / 2.0f;
    float canvasY = (1080.0f - canvasH) / 2.0f + 20.0f;

    // Panel Bayangan
    Rectangle shadowRec = { canvasX + 10.0f, canvasY + 15.0f, canvasW, canvasH };
    DrawRectangleRounded(shadowRec, 0.05f, 16, (Color){ 0, 0, 0, 100 }); 
    
    // Panel Utama (Biru Baja seperti Pola2D)
    Rectangle canvasRec = { canvasX, canvasY, canvasW, canvasH };
    DrawRectangleRounded(canvasRec, 0.05f, 16, (Color){ 85, 135, 175, 240 }); 
    DrawRectangleRoundedLines(canvasRec, 0.05f, 16, (Color){ 180, 220, 240, 200 });

    // Label Judul
    const char* title = "GUIDE & KONTROL";
    float titleFontSize = 45.0f;
    Vector2 tSize = MeasureTextEx(guideFont, title, titleFontSize, 2.0f);
    Rectangle titleBox = { 1920.0f/2.0f - tSize.x/2 - 30.0f, canvasY - (tSize.y/2) - 10.0f, tSize.x + 60.0f, tSize.y + 20.0f };
    
    DrawRectangleRounded((Rectangle){titleBox.x - 6.0f, titleBox.y - 6.0f, titleBox.width + 12.0f, titleBox.height + 12.0f}, 0.5f, 16, (Color){ 101, 67, 33, 255 }); 
    DrawRectangleRounded(titleBox, 0.5f, 16, (Color){ 160, 30, 45, 255 }); 
    DrawTextEx(guideFont, title, (Vector2){ titleBox.x + 30.0f, titleBox.y + 10.0f }, titleFontSize, 2.0f, WHITE);

    // Isi Konten Guide
    float cx = canvasX + 80.0f;
    float cy = canvasY + 80.0f;
    float fs = 26.0f;
    Color highlight = (Color){ 255, 215, 0, 255 }; // Kuning
    Color textCol   = WHITE;

    #define DRAW_GUIDE(key, desc, yOffset) \
        DrawTextEx(guideFont, key, (Vector2){ cx, cy + yOffset }, fs, 1.5f, highlight); \
        DrawTextEx(guideFont, desc, (Vector2){ cx + 250.0f, cy + yOffset }, fs, 1.5f, textCol);

    DrawTextEx(guideFont, "--- SHORTCUT KEYBOARD ---", (Vector2){ cx, cy }, 30.0f, 1.5f, (Color){ 30, 255, 150, 255 });
    DRAW_GUIDE("[ H ]", "- Sembunyikan / Tampilkan Panel UI (HUD)", 60);
    DRAW_GUIDE("[ SPACE ]", "- Play / Pause Animasi", 110);
    DRAW_GUIDE("[ W ]", "- Toggle Mode Wireframe (Garis Poligon)", 160);
    DRAW_GUIDE("[ UP / DOWN ]", "- Tambah / Kurangi Kecepatan Global", 210);
    DRAW_GUIDE("[ BACKSPACE ]", "- Kembali ke Menu Utama (Berlaku di semua layar)", 260);

    DrawTextEx(guideFont, "--- INTERAKSI MOUSE ---", (Vector2){ cx, cy + 320.0f }, 30.0f, 1.5f, (Color){ 30, 255, 150, 255 });
    DRAW_GUIDE("Klik Kiri (Langit)", "- Membuat Rasi Bintang Kustom (Malam Hari)", 380);
    DRAW_GUIDE("Klik Kanan", "- Mereset / Menghapus Rasi Bintang", 430);
    DRAW_GUIDE("UI Armada", "- Gunakan Tab Armada untuk memunculkan kapal", 480);
    DRAW_GUIDE("UI Kontrol", "- Atur Waktu, Ombak, dan Cuaca dari panel kiri", 530);

    EndMode2D();

    DrawBackButton();
    return SCREEN_GUIDE;
}