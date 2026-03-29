/**
 * @file back_button.c
 * @brief Algoritma pemroses antarmuka peraga komponen tombol kembali (Return Nav-Button Component).
 * 
 * Mengawasi siklus interaksi *mouse hover* secara mulus via transisi linier tak kasat mata
 * beserta pemrosesan topologi gelombang internal tombol sebagai dekorasi tematik Samudera.
 */

#include "back_button.h"
#include "raylib.h"
#include <math.h>

// Parameter Status Memori Visual (Visual Memory State Parameters)
static float backHoverProgress = 0.0f;
static Font backFont = { 0 };
static bool isFontLoaded = false;

/** @brief Interpolasi titik delta transisional (Linear Extrapolation Function). */
static float LerpBtn(float start, float end, float amount) {
    return start + amount * (end - start);
}

/** @brief Memproyeksikan rasio bidang sentuh (Hitbox Allocation) bernavigasi statis responsif. */
static Rectangle GetBackBtnHitbox(void) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    float scaleX = (float)screenW / 1920.0f;
    float scaleY = (float)screenH / 1080.0f;
    float scale = (scaleX < scaleY) ? scaleX : scaleY; 
    if (scale < 0.5f) scale = 0.5f; 

    // Lebar diperpendek menjadi 160.0f agar pas dengan teks dan tidak ada ruang kosong
    float btnWidth = 160.0f * scale;
    float btnHeight = 55.0f * scale;
    float baseX = 30.0f * scale;
    float baseY = 30.0f * scale;

    return (Rectangle){ baseX, baseY, btnWidth, btnHeight };
}

int BackButtonPressed(void) {
    Rectangle hitBox = GetBackBtnHitbox();
    Vector2 mouse = GetMousePosition();
    
    if (CheckCollisionPointRec(mouse, hitBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        return 1;
    }
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_BACKSPACE)) {
        return 1;
    }
    return 0;
}

void DrawBackButton(void) {
    // Lazy-load Font khusus untuk tombol back (hanya dimuat sekali)
    if (!isFontLoaded) {
        backFont = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0);
        isFontLoaded = true;
    }

    float dt = GetFrameTime();
    float time = (float)GetTime();
    Vector2 mousePoint = GetMousePosition();

    // UKURAN STATIS (Tidak berubah / melar sama sekali saat hover)
    Rectangle outerRec = GetBackBtnHitbox(); 
    bool isHovered = CheckCollisionPointRec(mousePoint, outerRec);

    // LOGIKA HOVER (Hanya untuk mengendalikan warna & partikel di dalam tombol)
    if (isHovered) {
        backHoverProgress += dt * 8.0f; 
    } else {
        backHoverProgress -= dt * 6.0f; 
    }
    if (backHoverProgress > 1.0f) backHoverProgress = 1.0f;
    if (backHoverProgress < 0.0f) backHoverProgress = 0.0f;

    float scale = outerRec.height / 55.0f;

    // --- LAYER A: BAYANGAN LEMBUT ---
    Rectangle shadowRec = { outerRec.x + (6.0f * scale), outerRec.y + (8.0f * scale), outerRec.width, outerRec.height };
    DrawRectangleRounded(shadowRec, 0.6f, 16, (Color){ 0, 0, 0, 60 });

    // --- LAYER B: OUTLINE TEBAL GLOWING ---
    float outlineIdle = 3.0f * scale;   
    float outlineHover = 6.0f * scale;  
    float currentThick = LerpBtn(outlineIdle, outlineHover, backHoverProgress);
    
    Color outColorIdle = (Color){ 30, 100, 150, 255 }; 
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

    // --- LAYER C: SOLID OCEAN BASE & EFEK GLOSSY ---
    Color idleColor  = (Color){ 10, 45, 80, 255 };  
    Color hoverColor = (Color){ 0, 130, 180, 255 }; 
    Color baseSolidColor = (Color){
        (unsigned char)LerpBtn(idleColor.r, hoverColor.r, backHoverProgress),
        (unsigned char)LerpBtn(idleColor.g, hoverColor.g, backHoverProgress),
        (unsigned char)LerpBtn(idleColor.b, hoverColor.b, backHoverProgress), 255 
    }; 
    DrawRectangleRounded(outerRec, 0.6f, 24, baseSolidColor);
    
    Rectangle glossRec = { outerRec.x + 2, outerRec.y + 2, outerRec.width - 4, (outerRec.height * 0.45f) };
    DrawRectangleRounded(glossRec, 0.6f, 16, (Color){ 255, 255, 255, 25 });

    // --- LAYER D: ANIMASI OMBAK & GELEMBUNG (SCISSOR MODE) ---
    BeginScissorMode((int)outerRec.x, (int)outerRec.y, (int)outerRec.width, (int)outerRec.height);
    if (backHoverProgress > 0.0f) {
        float waveAlpha = backHoverProgress;
        
        // Gelembung air
        int numBubbles = 15;
        for (int b = 0; b < numBubbles; b++) {
            float seed = (float)(b * 17);
            float speed = 40.0f + fmodf(seed, 40.0f); 
            float bx = outerRec.x + fmodf(seed * 93.0f, outerRec.width) + sinf(time * 3.0f + seed) * (10.0f * scale);
            float by = outerRec.y + outerRec.height - fmodf((time * speed * scale) + (seed * 77.0f), outerRec.height + (20.0f * scale));
            float radius = (1.5f + fmodf(seed, 3.0f)) * scale;
            
            if (by > outerRec.y && by < outerRec.y + outerRec.height) {
                if (b % 3 == 0) DrawCircleV((Vector2){bx, by}, radius, Fade(WHITE, 0.25f * waveAlpha)); 
                else DrawCircleLines((int)bx, (int)by, radius, Fade(WHITE, 0.5f * waveAlpha)); 
            }
        }

        // Ombak lapis bawah
        for(float wx = outerRec.x + (10.0f * scale); wx < outerRec.x + outerRec.width - (10.0f * scale); wx += 4.0f * scale) {
            float wy = outerRec.y + outerRec.height - (12.0f * scale) + sinf(wx * 0.03f + time * 4.0f) * (4.0f * scale) * waveAlpha;
            DrawCircleV((Vector2){wx, wy}, 3.5f * scale, Fade((Color){ 0, 170, 220, 255 }, waveAlpha));
        }
        // Ombak lapis atas
        for(float wx = outerRec.x + (10.0f * scale); wx < outerRec.x + outerRec.width - (10.0f * scale); wx += 4.0f * scale) {
            float wy = outerRec.y + outerRec.height - (6.0f * scale) + sinf(wx * 0.04f + time * 5.0f + 2.0f) * (3.0f * scale) * waveAlpha;
            DrawCircleV((Vector2){wx, wy}, 2.5f * scale, Fade((Color){ 173, 216, 230, 255 }, waveAlpha));
        }

        // Sapuan cahaya reflektif
        float sweepProgress = fmodf(time * 1.5f, 2.0f); 
        if (sweepProgress < 1.0f) { 
            float sweepX = outerRec.x - (50.0f * scale) + (outerRec.width + (100.0f * scale)) * sweepProgress;
            Vector2 p1 = { sweepX, outerRec.y }; 
            Vector2 p2 = { sweepX + (30.0f * scale), outerRec.y };
            Vector2 p3 = { sweepX - (10.0f * scale), outerRec.y + outerRec.height }; 
            Vector2 p4 = { sweepX - (40.0f * scale), outerRec.y + outerRec.height };
            Color beamColor = Fade(WHITE, 0.15f * waveAlpha);
            DrawTriangle(p1, p4, p3, beamColor); DrawTriangle(p1, p3, p2, beamColor);
        }
    }
    EndScissorMode();

    // --- LAYER E: ORNAMEN IKON PANAH KELAUTAN ---
    float iconX = outerRec.x + (28.0f * scale);
    float iconY = outerRec.y + (outerRec.height / 2.0f);
    
    Color iconIdle = (Color){ 150, 160, 170, 255 }; 
    Color iconGold = (Color){ 255, 215,   0, 255 }; 
    Color iconCol = (Color){
        (unsigned char)LerpBtn(iconIdle.r, iconGold.r, backHoverProgress),
        (unsigned char)LerpBtn(iconIdle.g, iconGold.g, backHoverProgress),
        (unsigned char)LerpBtn(iconIdle.b, iconGold.b, backHoverProgress), 255
    };

    DrawCircleLines((int)iconX, (int)iconY, 14 * scale, iconCol);
    DrawCircleLines((int)iconX, (int)iconY, 10 * scale, Fade(iconCol, 0.5f));
    
    float thick = 2.5f * scale;
    DrawLineEx((Vector2){iconX - (4*scale), iconY}, (Vector2){iconX + (6*scale), iconY}, thick, iconCol); 
    DrawLineEx((Vector2){iconX - (5*scale), iconY}, (Vector2){iconX + (1*scale), iconY - (6*scale)}, thick, iconCol); 
    DrawLineEx((Vector2){iconX - (5*scale), iconY}, (Vector2){iconX + (1*scale), iconY + (6*scale)}, thick, iconCol); 

    // --- LAYER F: TEKS MENGGUNAKAN FREDOKA ONE ---
    Color textColor = (backHoverProgress > 0.5f) ? WHITE : (Color){ 200, 220, 240, 255 };
    const char* text = "KEMBALI";
    
    float fontSize = 21.0f * scale; // Ukuran teks disesuaikan dengan tombol yang lebih pendek
    
    // Posisi teks dibuat lebih rapat ke ikon agar padat dan simetris
    float textX = iconX + (22.0f * scale); 
    Vector2 textSize = MeasureTextEx(backFont, text, fontSize, 2.0f);
    float textY = outerRec.y + (outerRec.height / 2.0f) - (textSize.y / 2.0f);
    
    // Shadow Teks
    DrawTextEx(backFont, text, (Vector2){textX, textY + 2.0f}, fontSize, 2.0f, (Color){ 10, 20, 30, 150 }); 
    // Teks Utama
    DrawTextEx(backFont, text, (Vector2){textX, textY}, fontSize, 2.0f, textColor); 
}