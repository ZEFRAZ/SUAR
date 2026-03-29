#ifndef MENU_H
#define MENU_H

#include "raylib.h"

typedef enum { 
    SCREEN_MENU, 
    SCREEN_POLA_2D, 
    SCREEN_ANIMASI_2D, 
    SCREEN_ABOUT, 
    SCREEN_GUIDE, 
    SCREEN_EXIT 
} ScreenState;

typedef struct {
    Rectangle rect;
    const char *text;
    float hoverProgress; 
} CustomButton;

void InitMenu(void);
ScreenState UpdateDrawMenu(void);

// Membersihkan tekstur gambar dari memori GPU saat keluar aplikasi
void UnloadMenu(void); 

#endif // MENU_H