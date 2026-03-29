/**
 * @file main.c
 * @brief Titik Awal Eksekusi (Entry Point) untuk Proyek Grafika Komputer "Mercusuar".
 * 
 * File ini mengatur instansiasi konteks grafis (Raylib Window Context),
 * merancang struktur tata letak kendali state (Scene Manager), memuat kerangka aset
 * awal seperti icon dan BGM, serta mengekskusi iterasi tiada henti (Render Loop) pada grafis utama.
 */
#include "raylib.h"
#include "menu.h"
#include "about.h"
#include "guide.h"
#include "pola2d.h"
#include "animasi2d.h"
#include "audio.h"
#include <stddef.h> 

/**
 * @brief Fungsi Penggerak Utama Proyek.
 * 
 * Melakukan inisialisasi lingkungan 2D berbasis OpenGL, memompa delegasi fungsi 
 * ke berbagai modul layar (Pola 2D, Animasi 2D, Guide, Halaman Utama),
 * serta melepaskan memori kembali ke sistem OS saat siklus Window dihentikan.
 * 
 * @return int Nilai kembalian 0 penanda eksekusi keluar dengan sukses (EXIT_SUCCESS).
 */
int main(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);
    InitWindow(1280, 720, "S U А R");
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

    Animasi2DScreen animasi;
    InitAnimasi2DScreen(&animasi);

    ScreenState currentScreen = SCREEN_MENU;

    while (!WindowShouldClose() && currentScreen != SCREEN_EXIT) {
        UpdateAudio_BGM();
        BeginDrawing();

        switch (currentScreen) {
            case SCREEN_MENU:
                ClearBackground(BLACK);
                currentScreen = UpdateDrawMenu();
                break;
            case SCREEN_POLA_2D:
                currentScreen = UpdateDrawPola2D();
                break;
            case SCREEN_ANIMASI_2D:
                UpdateAnimasi2DScreen(&animasi);
                DrawAnimasi2DScreen(&animasi);
                if (IsKeyPressed(KEY_BACKSPACE)) {
                    InitAnimasi2DScreen(&animasi);
                    currentScreen = SCREEN_MENU;
                }
                break;
            case SCREEN_ABOUT:
                currentScreen = UpdateDrawAbout();
                break;
            case SCREEN_GUIDE:
                currentScreen = UpdateDrawGuide();
                break;
            default: break;
        }
        DrawMuteButton();
        EndDrawing();
    }
    UnloadAnimasi2DScreen(&animasi);
    UnloadMenu();
    UnloadAudio_BGM();
    CloseWindow();
    return 0;
}