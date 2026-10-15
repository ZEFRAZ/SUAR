/**
 * @file main.c
 * @brief File Utama (Entry Point) untuk Proyek Grafika Komputer "Mercusuar".
 * 
 * File ini bertugas untuk membuka window Raylib pertama kali, mengatur perpindahan antar menu
 * (Scene Manager), meload audio, dan menjalankan perulangan game utama (Game Loop).
 */
#include "raylib.h"
#include "menu.h"
#include "about.h"
#include "guide.h"
#include "pola2d.h"
#include "animasi2d.h"
#include "audio.h"
#include <stddef.h> 

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

// Variabel status aplikasi yang dapat diakses oleh frame loop
static Animasi2DScreen g_animasi;
static ScreenState g_currentScreen = SCREEN_MENU;

/**
 * @brief Fungsi callback per-frame untuk rendering dan logika interaksi.
 */
static void UpdateDrawFrame(void) {
    UpdateAudio_BGM();
    BeginDrawing();

    switch (g_currentScreen) {
        case SCREEN_MENU:
            ClearBackground(BLACK);
            g_currentScreen = UpdateDrawMenu();
            break;
        case SCREEN_POLA_2D:
            g_currentScreen = UpdateDrawPola2D();
            break;
        case SCREEN_ANIMASI_2D:
            UpdateAnimasi2DScreen(&g_animasi);
            DrawAnimasi2DScreen(&g_animasi);
            if (IsKeyPressed(KEY_BACKSPACE)) {
                InitAnimasi2DScreen(&g_animasi);
                g_currentScreen = SCREEN_MENU;
            }
            break;
        case SCREEN_ABOUT:
            g_currentScreen = UpdateDrawAbout();
            break;
        case SCREEN_GUIDE:
            g_currentScreen = UpdateDrawGuide();
            break;
        default: break;
    }

#if defined(PLATFORM_WEB)
    // Di browser web, tombol Exit tidak menutup tab, melainkan tetap di menu
    if (g_currentScreen == SCREEN_EXIT) {
        g_currentScreen = SCREEN_MENU;
    }
#endif

    DrawMuteButton();
    EndDrawing();
}

/**
 * @brief Fungsi Penggerak Utama Proyek.
 * 
 * Melakukan inisialisasi lingkungan 2D, memanggil fungsi update/draw
 * dari masing-masing layar (Pola 2D, Animasi 2D, Guide, Menu Utama),
 * serta membersihkan memori kembali saat aplikasi ditutup.
 * 
 * @return int 0 jika program berhasil dijalankan sampai selesai.
 */
int main(void) {
#if !defined(PLATFORM_WEB)
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
#else
    SetConfigFlags(FLAG_VSYNC_HINT);
#endif
    InitWindow(1280, 720, "S U A R");
    SetWindowMinSize(800, 600);
    SetTargetFPS(60);

    Image icon = LoadImage("icon.png");
    if (icon.data != NULL) {
        ImageFormat(&icon, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
        ImageResize(&icon, 128, 128); 
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    InitMenu();
    InitAudio_BGM();

    InitAnimasi2DScreen(&g_animasi);

#if defined(PLATFORM_WEB)
    // Di WebAssembly / Browser, game loop diatur oleh requestAnimationFrame via Emscripten
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    while (!WindowShouldClose() && g_currentScreen != SCREEN_EXIT) {
        UpdateDrawFrame();
    }
    
    // --- AREA PERBAIKAN: PEMBERSIHAN MEMORI (MENCEGAH LEAK) ---
    UnloadAnimasi2DScreen(&g_animasi);
    UnloadMenu();
    UnloadPola2D();
    UnloadAbout();
    UnloadGuide();
    UnloadAudio_BGM();
    
    CloseWindow();
#endif

    return 0;
}