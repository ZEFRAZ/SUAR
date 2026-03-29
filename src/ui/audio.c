/**
 * @file audio.c
 * @brief Implementasi logika layanan dekoding deret audio (Audio Stream Service).
 * 
 * Melayani muat sirkulasi aliran audio terkompresi dengan komutasi asinkron di latar belakang
 * sekaligus mengelola perenderan grafis terintegrasi tombol navigasi matikan-hidupkan.
 */

#include "audio.h"
#include "raylib.h"
#include <math.h>
#include <stddef.h>

// =============================================================================
// Variabel Status Internal (Private State Declarations)
// =============================================================================
static Music  bgmMusic;
static bool   isMuted    = false;
static bool   audioReady = false;

// Parameter Kalkulasi Visual (Hover State & Typography Engine)
static float audioHoverProgress = 0.0f;
static Font audioFont = { 0 };
static bool isAudioFontLoaded = false;

// =============================================================================
// Inisiasi Entitas Siklus Hidup Perangkat (Device Lifecycle Initiation)
// =============================================================================
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

void UpdateAudio_BGM(void) {
    if (audioReady) UpdateMusicStream(bgmMusic);
}

void UnloadAudio_BGM(void) {
    if (audioReady) {
        StopMusicStream(bgmMusic);
        UnloadMusicStream(bgmMusic);
    }
    if (isAudioFontLoaded) UnloadFont(audioFont);
    CloseAudioDevice();
}

/**
 * @brief Interpolasi Linear Spasial (Linear Spatial Interpolation).
 * Mengkalkulasi perpindahan transisional antara limit skalar.
 */
static float LerpBtn(float start, float end, float amount) {
    return start + amount * (end - start);
}

/** @brief Memproyeksikan titik referensi matriks bidang interaksi dinamis secara responsif. */
static Rectangle GetAudioBtnHitbox(void) {
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    float scaleX = (float)screenW / 1920.0f;
    float scaleY = (float)screenH / 1080.0f;
    float scale = (scaleX < scaleY) ? scaleX : scaleY; 
    if (scale < 0.5f) scale = 0.5f; 

    float btnWidth = 160.0f * scale;
    float btnHeight = 55.0f * scale;
    float baseX = screenW - btnWidth - (30.0f * scale); // Jarak 30px dari kanan
    float baseY = 30.0f * scale;

    return (Rectangle){ baseX, baseY, btnWidth, btnHeight };
}

// =============================================================================
// Formasi Komponen Sentuh Grafis Audio (Graphic Touch Component Rendering)
// =============================================================================
void DrawMuteButton(void) {
    if (!isAudioFontLoaded) {
        audioFont = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0);
        isAudioFontLoaded = true;
    }

    float dt = GetFrameTime();
    float time = (float)GetTime();
    Vector2 mousePoint = GetMousePosition();

    // UKURAN STATIS
    Rectangle outerRec = GetAudioBtnHitbox(); 
    bool isHovered = CheckCollisionPointRec(mousePoint, outerRec);

    // LOGIKA HOVER (Sama persis dengan back_button)
    if (isHovered) {
        audioHoverProgress += dt * 8.0f; 
    } else {
        audioHoverProgress -= dt * 6.0f; 
    }
    if (audioHoverProgress > 1.0f) audioHoverProgress = 1.0f;
    if (audioHoverProgress < 0.0f) audioHoverProgress = 0.0f;

    // Logika Klik
    if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        isMuted = !isMuted;
        if (audioReady) {
            SetMusicVolume(bgmMusic, isMuted ? 0.0f : 0.6f);
        }
    }

    float scale = outerRec.height / 55.0f;

    // --- LAYER A: BAYANGAN LEMBUT ---
    Rectangle shadowRec = { outerRec.x + (6.0f * scale), outerRec.y + (8.0f * scale), outerRec.width, outerRec.height };
    DrawRectangleRounded(shadowRec, 0.6f, 16, (Color){ 0, 0, 0, 60 });

    // --- LAYER B: OUTLINE TEBAL GLOWING ---
    float currentThick = LerpBtn(3.0f * scale, 6.0f * scale, audioHoverProgress);
    Color outColorIdle = (Color){ 30, 100, 150, 255 }; 
    Color outColorHover = (Color){ 0, 255, 255, 255 }; 
    Color currentOutCol = {
        (unsigned char)LerpBtn(outColorIdle.r, outColorHover.r, audioHoverProgress),
        (unsigned char)LerpBtn(outColorIdle.g, outColorHover.g, audioHoverProgress),
        (unsigned char)LerpBtn(outColorIdle.b, outColorHover.b, audioHoverProgress), 255
    };
    Rectangle outlineRec = { outerRec.x - currentThick, outerRec.y - currentThick, outerRec.width + (currentThick * 2), outerRec.height + (currentThick * 2) };
    DrawRectangleRounded(outlineRec, 0.6f, 24, currentOutCol);

    // --- LAYER C: SOLID OCEAN BASE & EFEK GLOSSY ---
    Color idleColor  = (Color){ 10, 45, 80, 255 };  
    Color hoverColor = (Color){ 0, 130, 180, 255 }; 
    Color baseSolidColor = (Color){
        (unsigned char)LerpBtn(idleColor.r, hoverColor.r, audioHoverProgress),
        (unsigned char)LerpBtn(idleColor.g, hoverColor.g, audioHoverProgress),
        (unsigned char)LerpBtn(idleColor.b, hoverColor.b, audioHoverProgress), 255 
    }; 
    DrawRectangleRounded(outerRec, 0.6f, 24, baseSolidColor);
    Rectangle glossRec = { outerRec.x + 2, outerRec.y + 2, outerRec.width - 4, (outerRec.height * 0.45f) };
    DrawRectangleRounded(glossRec, 0.6f, 16, (Color){ 255, 255, 255, 25 });

    // --- LAYER D: ANIMASI OMBAK & GELEMBUNG ---
    BeginScissorMode((int)outerRec.x, (int)outerRec.y, (int)outerRec.width, (int)outerRec.height);
    if (audioHoverProgress > 0.0f) {
        float waveAlpha = audioHoverProgress;
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
        for(float wx = outerRec.x + (10.0f * scale); wx < outerRec.x + outerRec.width - (10.0f * scale); wx += 4.0f * scale) {
            float wy = outerRec.y + outerRec.height - (12.0f * scale) + sinf(wx * 0.03f + time * 4.0f) * (4.0f * scale) * waveAlpha;
            DrawCircleV((Vector2){wx, wy}, 3.5f * scale, Fade((Color){ 0, 170, 220, 255 }, waveAlpha));
        }
        for(float wx = outerRec.x + (10.0f * scale); wx < outerRec.x + outerRec.width - (10.0f * scale); wx += 4.0f * scale) {
            float wy = outerRec.y + outerRec.height - (6.0f * scale) + sinf(wx * 0.04f + time * 5.0f + 2.0f) * (3.0f * scale) * waveAlpha;
            DrawCircleV((Vector2){wx, wy}, 2.5f * scale, Fade((Color){ 173, 216, 230, 255 }, waveAlpha));
        }
        float sweepProgress = fmodf(time * 1.5f, 2.0f); 
        if (sweepProgress < 1.0f) { 
            float sweepX = outerRec.x - (50.0f * scale) + (outerRec.width + (100.0f * scale)) * sweepProgress;
            Vector2 p1 = { sweepX, outerRec.y }; Vector2 p2 = { sweepX + (30.0f * scale), outerRec.y };
            Vector2 p3 = { sweepX - (10.0f * scale), outerRec.y + outerRec.height }; Vector2 p4 = { sweepX - (40.0f * scale), outerRec.y + outerRec.height };
            Color beamColor = Fade(WHITE, 0.15f * waveAlpha);
            DrawTriangle(p1, p4, p3, beamColor); DrawTriangle(p1, p3, p2, beamColor);
        }
    }
    EndScissorMode();

    // --- LAYER E: IKON SPEAKER KHUSUS ---
    float iconX = outerRec.x + (25.0f * scale);
    float iconY = outerRec.y + (outerRec.height / 2.0f);
    
    Color iconIdle = (Color){ 150, 160, 170, 255 }; 
    Color iconGold = (Color){ 255, 215,   0, 255 }; 
    Color iconCol = (Color){
        (unsigned char)LerpBtn(iconIdle.r, iconGold.r, audioHoverProgress),
        (unsigned char)LerpBtn(iconIdle.g, iconGold.g, audioHoverProgress),
        (unsigned char)LerpBtn(iconIdle.b, iconGold.b, audioHoverProgress), 255
    };

    // Menggambar Kotak Speaker (Base)
    DrawRectangle((int)(iconX - 6*scale), (int)(iconY - 5*scale), (int)(6*scale), (int)(10*scale), iconCol);
    
    // Menggambar Corong Speaker
    DrawTriangle((Vector2){iconX, iconY - 5*scale}, (Vector2){iconX, iconY + 5*scale}, (Vector2){iconX + 10*scale, iconY - 12*scale}, iconCol);
    DrawTriangle((Vector2){iconX, iconY + 5*scale}, (Vector2){iconX + 10*scale, iconY + 12*scale}, (Vector2){iconX + 10*scale, iconY - 12*scale}, iconCol);

    if (!isMuted) {
        // Efek Suara Menyala (Gelombang)
        DrawRing((Vector2){iconX + 8*scale, iconY}, 6*scale, 9*scale, -45, 45, 10, iconCol);
        DrawRing((Vector2){iconX + 8*scale, iconY}, 13*scale, 16*scale, -40, 40, 10, iconCol);
    } else {
        // Tanda Silang (X) Merah untuk Mute
        Color redCross = (Color){ 255, 60, 60, 255 };
        float cx = iconX + 18*scale;
        DrawLineEx((Vector2){cx - 6*scale, iconY - 6*scale}, (Vector2){cx + 6*scale, iconY + 6*scale}, 4*scale, redCross);
        DrawLineEx((Vector2){cx - 6*scale, iconY + 6*scale}, (Vector2){cx + 6*scale, iconY - 6*scale}, 4*scale, redCross);
    }

    // --- LAYER F: TEKS MENGGUNAKAN FREDOKA ONE ---
    Color textColor = (audioHoverProgress > 0.5f) ? WHITE : (Color){ 200, 220, 240, 255 };
    const char* text = isMuted ? "MUTE" : "AUDIO";
    float fontSize = 21.0f * scale; 
    
    float textX = iconX + (36.0f * scale); 
    Vector2 textSize = MeasureTextEx(audioFont, text, fontSize, 2.0f);
    float textY = outerRec.y + (outerRec.height / 2.0f) - (textSize.y / 2.0f);
    
    DrawTextEx(audioFont, text, (Vector2){textX, textY + 2.0f}, fontSize, 2.0f, (Color){ 10, 20, 30, 150 }); 
    DrawTextEx(audioFont, text, (Vector2){textX, textY}, fontSize, 2.0f, textColor); 
}