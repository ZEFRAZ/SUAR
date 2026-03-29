#include "about.h"
#include "back_button.h"
#include "raymath.h"

static Font aboutFont = { 0 };
static Texture2D bgTextureAbout = { 0 };
static bool isAboutAssetsLoaded = false;

/**
 * @brief Fungsi perulangan (Update & Draw) untuk layar About (Tentang Kami).
 * 
 * Mengatur tampilan teks profil pembuat (nama, NIM, dll). 
 * Menggunakan fitur Camera2D dari Raylib agar teks tetap berada di tengah layar
 * dan menyesuaikan ukuran (scaling) ketika resolusi window diubah secara dinamis.
 * 
 * @return ScreenState Layar target berikutnya (kembali ke SCREEN_MENU jika tombol Back ditekan).
 */
ScreenState UpdateDrawAbout(void) {
    if (!isAboutAssetsLoaded) {
        aboutFont = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0);
        bgTextureAbout = LoadTexture("background_pola2d.png");
        isAboutAssetsLoaded = true;
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
    if (bgTextureAbout.id != 0) {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)bgTextureAbout.width, (float)bgTextureAbout.height };
        Rectangle destRec = { 0.0f, 0.0f, (float)W, (float)H };
        DrawTexturePro(bgTextureAbout, sourceRec, destRec, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);
    } else {
        ClearBackground((Color){ 20, 25, 35, 255 });
    }

    // 3. MULAI RENDER UI DENGAN KAMERA VIRTUAL
    BeginMode2D(camera);

    float canvasW = 1050.0f; 
    float canvasH = 550.0f; 
    float canvasX = (1920.0f - canvasW) / 2.0f;
    float canvasY = (1080.0f - canvasH) / 2.0f + 20.0f;

    // Panel Bayangan
    Rectangle shadowRec = { canvasX + 10.0f, canvasY + 15.0f, canvasW, canvasH };
    DrawRectangleRounded(shadowRec, 0.05f, 16, (Color){ 0, 0, 0, 100 }); 
    
    // Panel Utama (Biru Baja)
    Rectangle canvasRec = { canvasX, canvasY, canvasW, canvasH };
    DrawRectangleRounded(canvasRec, 0.05f, 16, (Color){ 85, 135, 175, 240 }); 
    DrawRectangleRoundedLines(canvasRec, 0.05f, 16, (Color){ 180, 220, 240, 200 });

    // Label Judul
    const char* title = "ABOUT";
    float titleFontSize = 45.0f;
    Vector2 tSize = MeasureTextEx(aboutFont, title, titleFontSize, 2.0f);
    Rectangle titleBox = { 1920.0f/2.0f - tSize.x/2 - 30.0f, canvasY - (tSize.y/2) - 10.0f, tSize.x + 60.0f, tSize.y + 20.0f };
    
    DrawRectangleRounded((Rectangle){titleBox.x - 6.0f, titleBox.y - 6.0f, titleBox.width + 12.0f, titleBox.height + 12.0f}, 0.5f, 16, (Color){ 101, 67, 33, 255 }); 
    DrawRectangleRounded(titleBox, 0.5f, 16, (Color){ 160, 30, 45, 255 }); 
    DrawTextEx(aboutFont, title, (Vector2){ titleBox.x + 30.0f, titleBox.y + 10.0f }, titleFontSize, 2.0f, WHITE);

    // Isi Konten About
    float cx = 1920.0f / 2.0f;
    float cy = canvasY + 130.0f; 
    float fs = 30.0f;
    Color labelCol = (Color){ 255, 215, 0, 255 }; 
    Color textCol  = WHITE;

    // PERBAIKAN: Menggeser X jauh ke kiri agar blok tulisan pas di tengah panel
    #define DRAW_ROW(label, text, yOffset) \
        DrawTextEx(aboutFont, label, (Vector2){ cx - 350.0f, cy + yOffset }, fs, 1.5f, labelCol); \
        DrawTextEx(aboutFont, text, (Vector2){ cx - 90.0f, cy + yOffset }, fs, 1.5f, textCol);

    DRAW_ROW("Nama", ": Rangga Muhamad Fajar", 0);
    DRAW_ROW("NIM", ": 241524026", 90); 
    DRAW_ROW("Mata Kuliah", ": Komputer Grafis", 180);
    DRAW_ROW("Proyek", ": Animasi 2D Mercusuar & Kapal Laut", 270);

    EndMode2D();

    DrawBackButton(); 
    return SCREEN_ABOUT;
}