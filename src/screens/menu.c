#include "menu.h"
#include <string.h>
#include <math.h> 

#define NUM_BUTTONS 5

static CustomButton buttons[NUM_BUTTONS];
static const char* btnLabels[NUM_BUTTONS] = {"Pola 2D", "Animasi 2D", "About", "Guide", "Exit"};

static Texture2D bgTexture; 
static Font customFont; 
static Font titleFont; 

static float Lerp(float start, float end, float amount) {
    return start + amount * (end - start);
}

void InitMenu(void) {
    bgTexture = LoadTexture("background.png"); 
    
    // Pengaturan font untuk tombol utama
    customFont = LoadFontEx("FredokaOne-Regular.ttf", 64, 0, 0); 
    // Pengaturan font untuk teks judul
    titleFont = LoadFontEx("Starbim.ttf", 150, 0, 0); 

    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].text = btnLabels[i];
        buttons[i].hoverProgress = 0.0f;
    }
}

void UnloadMenu(void) {
    UnloadTexture(bgTexture);
    UnloadFont(customFont); 
    UnloadFont(titleFont); 
}

ScreenState UpdateDrawMenu(void) {
    float dt = GetFrameTime();
    float time = (float)GetTime(); 
    Vector2 mousePoint = GetMousePosition();
    ScreenState nextScreen = SCREEN_MENU;

    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    ClearBackground(RAYWHITE); 
    
    Rectangle sourceRec = { 0.0f, 0.0f, (float)bgTexture.width, (float)bgTexture.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)screenW, (float)screenH };
    DrawTexturePro(bgTexture, sourceRec, destRec, (Vector2){ 0.0f, 0.0f }, 0.0f, WHITE);

    float scaleX = (float)screenW / 1920.0f;
    float scaleY = (float)screenH / 1080.0f;
    float scale = (scaleX < scaleY) ? scaleX : scaleY; 
    if (scale < 0.4f) scale = 0.4f; 

    // ==========================================
    // Perhitungan ukuran dan posisi tiap tombol
    // ==========================================
    float btnWidth    = 540.0f * scale;
    float btnHeight   = 85.0f * scale; 
    float btnSpacing  = 105.0f * scale;
    // Pengaturan ukuran font tombol
    float btnFontSize = 50.0f * scale; 

    float rightMargin = 220.0f * scale; 
    float baseX = screenW - btnWidth - rightMargin; 

    float buttonsBlockHeight = NUM_BUTTONS * btnSpacing;
    float yOffset = 180.0f * scale; 
    float startY = ((screenH - buttonsBlockHeight) / 2.0f) + yOffset;

    // ==========================================
    // 1. Menggambar tali penghubung di belakang tombol
    // ==========================================
    float ropeLeftX = baseX + (60.0f * scale);
    float ropeRightX = baseX + btnWidth - (60.0f * scale);
    
    float ropeStartY = startY - (220.0f * scale); 
    float ropeEndY = startY + buttonsBlockHeight - btnSpacing + btnHeight + (25.0f * scale);

    // Pengaturan warna tali penghubung
    DrawLineEx((Vector2){ropeLeftX, ropeStartY}, (Vector2){ropeLeftX, ropeEndY}, 12.0f * scale, (Color){ 101, 67, 33, 255 });
    DrawLineEx((Vector2){ropeLeftX - 3.0f*scale, ropeStartY}, (Vector2){ropeLeftX - 3.0f*scale, ropeEndY}, 4.0f * scale, (Color){ 205, 133, 63, 255 });
    
    DrawLineEx((Vector2){ropeRightX, ropeStartY}, (Vector2){ropeRightX, ropeEndY}, 12.0f * scale, (Color){ 101, 67, 33, 255 });
    DrawLineEx((Vector2){ropeRightX - 3.0f*scale, ropeStartY}, (Vector2){ropeRightX - 3.0f*scale, ropeEndY}, 4.0f * scale, (Color){ 205, 133, 63, 255 });


    // ==========================================
    // 2. Menggambar kotak judul "SUAR"
    // ==========================================
    float titleBoxWidth = btnWidth + (80.0f * scale);
    float titleBoxHeight = 170.0f * scale;
    float titleBoxX = baseX - (40.0f * scale); 
    
    float gapTitleToBtn = 50.0f * scale;
    float floatingY = sinf(time * 2.0f) * (8.0f * scale); 
    float titleBoxY = startY - titleBoxHeight - gapTitleToBtn + floatingY;

    // A. Menggambar bayangan di belakang kotak judul
    DrawRectangleRounded((Rectangle){titleBoxX + 12*scale, titleBoxY + 15*scale, titleBoxWidth, titleBoxHeight}, 0.5f, 24, (Color){ 0, 0, 0, 80 });

    // B. Menggambar garis tepi (outline) kotak judul
    float tStroke = 8.0f * scale;
    DrawRectangleRounded((Rectangle){titleBoxX - tStroke, titleBoxY - tStroke, titleBoxWidth + (tStroke*2), titleBoxHeight + (tStroke*2)}, 0.5f, 24, (Color){ 101, 67, 33, 255 });

    // C. Menggambar warna dasar pada kotak judul
    DrawRectangleRounded((Rectangle){titleBoxX, titleBoxY, titleBoxWidth, titleBoxHeight}, 0.5f, 24, (Color){ 160, 30, 45, 255 });
    DrawRectangleRounded((Rectangle){titleBoxX + 5*scale, titleBoxY + 5*scale, titleBoxWidth - 10*scale, titleBoxHeight - 10*scale}, 0.4f, 24, (Color){ 180, 40, 55, 255 });

    // D. Penulisan teks judul
    const char* titleText = "S U A R"; 
    float titleFontSize = 140.0f * scale; 
    float titleLetterSpacing = 10.0f * scale;
    
    Vector2 titleSize = MeasureTextEx(titleFont, titleText, titleFontSize, titleLetterSpacing); 
    float titleTextX = titleBoxX + (titleBoxWidth / 2.0f) - (titleSize.x / 2.0f);
    float titleTextY = titleBoxY + (titleBoxHeight / 2.0f) - (titleSize.y / 2.0f) - (5.0f * scale);
    
    // Memecah teks untuk memberi warna berbeda pada huruf A
    const char* part1 = "S U ";
    const char* part2 = "A";
    const char* part3 = " R";
    
    // Menghitung jarak spasi antar potongan huruf
    float x1 = titleTextX;
    float x2 = titleTextX + MeasureTextEx(titleFont, part1, titleFontSize, titleLetterSpacing).x;
    float x3 = titleTextX + MeasureTextEx(titleFont, "S U A", titleFontSize, titleLetterSpacing).x;
    
    Color shadowCol = (Color){ 60, 10, 15, 180 };
    Color goldCol = (Color){ 255, 220, 20, 255 };

    // Menggambar efek bayangan (drop shadow) pada huruf
    DrawTextEx(titleFont, part1, (Vector2){x1 + 6*scale, titleTextY + 6*scale}, titleFontSize, titleLetterSpacing, shadowCol);
    DrawTextEx(titleFont, part2, (Vector2){x2 + 6*scale, titleTextY + 6*scale}, titleFontSize, titleLetterSpacing, shadowCol);
    DrawTextEx(titleFont, part3, (Vector2){x3 + 6*scale, titleTextY + 6*scale}, titleFontSize, titleLetterSpacing, shadowCol);
    
    // Menggambar huruf judul utama
    DrawTextEx(titleFont, part1, (Vector2){x1, titleTextY}, titleFontSize, titleLetterSpacing, WHITE);
    DrawTextEx(titleFont, part2, (Vector2){x2, titleTextY}, titleFontSize, titleLetterSpacing, goldCol);
    DrawTextEx(titleFont, part3, (Vector2){x3, titleTextY}, titleFontSize, titleLetterSpacing, WHITE);
    
    // E. Menggambar sub-judul (lencana) di bawah judul besar
    const char* subText = "PROYEK ANIMASI MERCUSUAR";
    float subFontSize = 20.0f * scale;
    Vector2 subSize = MeasureTextEx(customFont, subText, subFontSize, 2.0f);
    
    Rectangle subBg = { titleBoxX + (titleBoxWidth / 2.0f) - (subSize.x / 2.0f) - 20*scale, titleBoxY + titleBoxHeight - (15*scale), subSize.x + 40*scale, subSize.y + 16*scale };
    // Menggambar garis tepi sub-judul
    DrawRectangleRounded((Rectangle){subBg.x - 4*scale, subBg.y - 4*scale, subBg.width + 8*scale, subBg.height + 8*scale}, 0.8f, 16, (Color){ 101, 67, 33, 255 });
    // Menggambar kotak latar sub-judul
    DrawRectangleRounded(subBg, 0.8f, 16, WHITE); 
    // Menulis teks sub-judul
    DrawTextEx(customFont, subText, (Vector2){baseX + (btnWidth / 2.0f) - (subSize.x / 2.0f), subBg.y + 7*scale}, subFontSize, 2.0f, (Color){ 160, 30, 45, 255 }); 


    // ==========================================
    // 3. Proses update hover dan gambar tombol navigasi
    // ==========================================
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i].rect = (Rectangle){ baseX, startY + (i * btnSpacing), btnWidth, btnHeight };

        bool isHovered = CheckCollisionPointRec(mousePoint, buttons[i].rect);

        if (isHovered) {
            buttons[i].hoverProgress += dt * 8.0f; 
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                if (i == 0) nextScreen = SCREEN_POLA_2D;
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

        float hoverExtension = 50.0f * scale; 
        float currentWidth = Lerp(btnWidth, btnWidth + hoverExtension, buttons[i].hoverProgress);
        float currentX = Lerp(baseX, baseX - (hoverExtension / 2.0f), buttons[i].hoverProgress); 
        
        Rectangle outerRec = { currentX, buttons[i].rect.y, currentWidth, btnHeight };

        // Layer A: Bayangan tombol
        Rectangle shadowRec = { outerRec.x + 8.0f, outerRec.y + 10.0f, outerRec.width, outerRec.height };
        DrawRectangleRounded(shadowRec, 0.6f, 16, (Color){ 0, 0, 0, 60 });

        // Layer B: Garis tepi tombol yang menyala saat di-hover
        float outlineIdle = 4.0f * scale;   
        float outlineHover = 8.0f * scale;  
        float currentThick = Lerp(outlineIdle, outlineHover, buttons[i].hoverProgress);
        
        Color outColorIdle = (Color){ 30, 100, 150, 255 }; 
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

        // Layer C: Warna dasar tombol
        Color idleColor  = (Color){ 10, 45, 80, 255 };  
        Color hoverColor = (Color){ 0, 130, 180, 255 }; 
        
        unsigned char r = (unsigned char)Lerp(idleColor.r, hoverColor.r, buttons[i].hoverProgress);
        unsigned char g = (unsigned char)Lerp(idleColor.g, hoverColor.g, buttons[i].hoverProgress);
        unsigned char b = (unsigned char)Lerp(idleColor.b, hoverColor.b, buttons[i].hoverProgress);
        Color baseSolidColor = (Color){ r, g, b, 255 }; 

        DrawRectangleRounded(outerRec, 0.6f, 24, baseSolidColor);
        
        Rectangle glossRec = { outerRec.x + 2, outerRec.y + 2, outerRec.width - 4, (outerRec.height * 0.45f) };
        DrawRectangleRounded(glossRec, 0.6f, 16, (Color){ 255, 255, 255, 25 }); 

        // Layer D: Efek gelombang air dan partikel pada tombol
        BeginScissorMode((int)outerRec.x, (int)outerRec.y, (int)outerRec.width, (int)outerRec.height);
        if (buttons[i].hoverProgress > 0.0f) {
            float waveAlpha = buttons[i].hoverProgress;

            int numBubbles = 20;
            for (int b = 0; b < numBubbles; b++) {
                float seed = (float)(i * 31 + b * 17);
                float speed = 40.0f + fmodf(seed, 40.0f); 
                float bx = outerRec.x + fmodf(seed * 93.0f, outerRec.width) + sinf(time * 3.0f + seed) * (15.0f * scale);
                float by = outerRec.y + btnHeight - fmodf((time * speed * scale) + (seed * 77.0f), btnHeight + (20.0f * scale));
                float radius = (1.5f + fmodf(seed, 4.0f)) * scale;
                
                if (by > outerRec.y && by < outerRec.y + btnHeight) {
                    if (b % 3 == 0) DrawCircleV((Vector2){bx, by}, radius, Fade(WHITE, 0.25f * waveAlpha)); 
                    else DrawCircleLines((int)bx, (int)by, radius, Fade(WHITE, 0.5f * waveAlpha)); 
                }
            }

            for(float wx = outerRec.x + (20.0f * scale); wx < outerRec.x + outerRec.width - (20.0f * scale); wx += 4.0f * scale) {
                float wy = outerRec.y + btnHeight - (20.0f * scale) + sinf(wx * 0.03f + time * 4.0f) * (6.0f * scale) * waveAlpha;
                DrawCircleV((Vector2){wx, wy}, 4.5f * scale, Fade((Color){ 0, 170, 220, 255 }, waveAlpha));
            }
            for(float wx = outerRec.x + (20.0f * scale); wx < outerRec.x + outerRec.width - (20.0f * scale); wx += 4.0f * scale) {
                float wy = outerRec.y + btnHeight - (10.0f * scale) + sinf(wx * 0.04f + time * 5.0f + 2.0f) * (5.0f * scale) * waveAlpha;
                DrawCircleV((Vector2){wx, wy}, 3.0f * scale, Fade((Color){ 173, 216, 230, 255 }, waveAlpha));
            }

            float sweepProgress = fmodf(time * 1.5f + (i * 0.5f), 2.0f); 
            if (sweepProgress < 1.0f) { 
                float sweepX = outerRec.x - (100.0f * scale) + (outerRec.width + (200.0f * scale)) * sweepProgress;
                Vector2 p1 = { sweepX, outerRec.y }; Vector2 p2 = { sweepX + (40.0f * scale), outerRec.y };
                Vector2 p3 = { sweepX - (20.0f * scale), outerRec.y + btnHeight }; Vector2 p4 = { sweepX - (60.0f * scale), outerRec.y + btnHeight };
                Color beamColor = Fade(WHITE, 0.15f * waveAlpha);
                DrawTriangle(p1, p4, p3, beamColor); DrawTriangle(p1, p3, p2, beamColor);
            }
        }
        EndScissorMode();

        // Layer E: Menggambar ikon unik untuk masing-masing tombol
        float iconX = outerRec.x + (60.0f * scale);
        float iconY = outerRec.y + (btnHeight / 2.0f);
        
        Color iconIdle = (Color){ 150, 160, 170, 255 }; 
        Color iconGold = (Color){ 255, 215,   0, 255 }; 
        unsigned char ir = (unsigned char)Lerp(iconIdle.r, iconGold.r, buttons[i].hoverProgress);
        unsigned char ig = (unsigned char)Lerp(iconIdle.g, iconGold.g, buttons[i].hoverProgress);
        unsigned char ib = (unsigned char)Lerp(iconIdle.b, iconGold.b, buttons[i].hoverProgress);
        Color iconCol = (Color){ ir, ig, ib, 255 };
        float s = scale;

        switch(i) {
            case 0: 
                DrawCircleLines((int)iconX, (int)iconY, 12*s, iconCol);
                DrawCircleLines((int)iconX, (int)iconY, 8*s, iconCol);
                DrawTriangle((Vector2){iconX-4*s, iconY}, (Vector2){iconX+4*s, iconY}, (Vector2){iconX, iconY-14*s}, iconCol);
                DrawTriangle((Vector2){iconX-4*s, iconY}, (Vector2){iconX, iconY+14*s}, (Vector2){iconX+4*s, iconY}, iconCol);
                break;
            case 1: 
                DrawCircleLines((int)iconX, (int)iconY - 10*s, 4*s, iconCol); 
                DrawLineEx((Vector2){iconX - 10*s, iconY - 4*s}, (Vector2){iconX + 10*s, iconY - 4*s}, 2*s, iconCol); 
                DrawLineEx((Vector2){iconX, iconY - 6*s}, (Vector2){iconX, iconY + 12*s}, 3*s, iconCol); 
                DrawLineEx((Vector2){iconX - 12*s, iconY + 4*s}, (Vector2){iconX, iconY + 14*s}, 3*s, iconCol); 
                DrawLineEx((Vector2){iconX + 12*s, iconY + 4*s}, (Vector2){iconX, iconY + 14*s}, 3*s, iconCol);
                break;
            case 2: 
                DrawRing((Vector2){iconX, iconY}, 6*s, 12*s, 0, 360, 32, iconCol);
                DrawRectanglePro((Rectangle){iconX, iconY-12*s, 4*s, 6*s}, (Vector2){2*s, 0}, 0, (buttons[i].hoverProgress > 0.5f)?RED:(Color){200,50,50,255}); 
                DrawRectanglePro((Rectangle){iconX, iconY+6*s, 4*s, 6*s}, (Vector2){2*s, 0}, 0, (buttons[i].hoverProgress > 0.5f)?RED:(Color){200,50,50,255});
                DrawRectanglePro((Rectangle){iconX-12*s, iconY, 6*s, 4*s}, (Vector2){0, 2*s}, 0, (buttons[i].hoverProgress > 0.5f)?RED:(Color){200,50,50,255});
                DrawRectanglePro((Rectangle){iconX+6*s, iconY, 6*s, 4*s}, (Vector2){0, 2*s}, 0, (buttons[i].hoverProgress > 0.5f)?RED:(Color){200,50,50,255});
                break;
            case 3: 
                DrawCircleLines((int)iconX, (int)iconY, 9*s, iconCol);
                DrawCircle((int)iconX, (int)iconY, 3*s, iconCol);
                for(int j=0; j<8; j++) {
                    float angle = j * 45.0f * DEG2RAD;
                    DrawLineEx((Vector2){iconX, iconY}, (Vector2){iconX + cosf(angle)*14*s, iconY + sinf(angle)*14*s}, 2*s, iconCol);
                }
                break;
            case 4: 
                DrawLineEx((Vector2){iconX+2*s, iconY-12*s}, (Vector2){iconX+2*s, iconY+4*s}, 2*s, iconCol); 
                DrawRing((Vector2){iconX-2*s, iconY+4*s}, 2*s, 4*s, 0, 180, 16, iconCol); 
                DrawTriangle((Vector2){iconX-8*s, iconY+4*s}, (Vector2){iconX-4*s, iconY}, (Vector2){iconX-8*s, iconY-2*s}, iconCol);
                DrawCircleLines((int)iconX+2*s, (int)iconY-14*s, 2*s, iconCol);
                break;
        }

        // Layer F: Menulis teks label di tengah tombol
        Color textColor = (buttons[i].hoverProgress > 0.5f) ? WHITE : (Color){ 200, 220, 240, 255 };
        
        Vector2 textSize = MeasureTextEx(customFont, buttons[i].text, btnFontSize, 2.0f);
        float textX = outerRec.x + (outerRec.width / 2.0f) - (textSize.x / 2.0f);
        float textY = outerRec.y + (outerRec.height / 2.0f) - (textSize.y / 2.0f);
        
        DrawTextEx(customFont, buttons[i].text, (Vector2){textX, textY + 2.0f}, btnFontSize, 2.0f, (Color){ 10, 20, 30, 150 }); 
        DrawTextEx(customFont, buttons[i].text, (Vector2){textX, textY}, btnFontSize, 2.0f, textColor); 
    }

    return nextScreen;
}