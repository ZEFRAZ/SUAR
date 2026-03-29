#include "animasi2d.h"
#include "pola2d.h"
#include "../ui/draw_helpers.h"
#include "../objects/kapal.h"
#include "../objects/alam.h"
#include "../objects/bangunan.h"
#include "../ui/hud.h"
#include <math.h>
#include <stdlib.h>
#include "raymath.h"
#include "menu.h"

extern float previewTime;


#ifndef MAX_CUSTOM_STARS
#define MAX_CUSTOM_STARS 100
#endif

// --- HELPER WARNA & LERP ---
static Color LerpColor(Color a, Color b, float t) {
    if (t < 0.0f) t = 0.0f; 
    if (t > 1.0f) t = 1.0f;
    return (Color){
        (unsigned char)(a.r + (b.r - a.r) * t),
        (unsigned char)(a.g + (b.g - a.g) * t),
        (unsigned char)(a.b + (b.b - a.b) * t),
        (unsigned char)(a.a + (b.a - a.a) * t)
    };
}

static void GetSkyColor(float time, Color *top, Color *bot) {
    struct Stop { float t; Color top; Color bot; };
    struct Stop stops[] = {
        { 0.0f,  (Color){2, 1, 17, 255},       (Color){32, 18, 77, 255} },
        { 5.0f,  (Color){2, 1, 17, 255},       (Color){32, 18, 77, 255} },
        { 6.5f,  (Color){40, 60, 120, 255},    (Color){255, 150, 100, 255} }, 
        { 8.0f,  (Color){135, 206, 252, 255},  (Color){224, 246, 255, 255} }, 
        { 16.0f, (Color){135, 206, 252, 255},  (Color){224, 246, 255, 255} }, 
        { 18.0f, (Color){60, 40, 100, 255},    (Color){255, 100, 50, 255} },  
        { 19.5f, (Color){2, 1, 17, 255},       (Color){32, 18, 77, 255} },    
        { 24.0f, (Color){2, 1, 17, 255},       (Color){32, 18, 77, 255} }
    };
    
    struct Stop s1 = stops[0], s2 = stops[1];
    for (int i = 0; i < 7; i++) {
        if (time >= stops[i].t && time <= stops[i+1].t) {
            s1 = stops[i]; s2 = stops[i+1]; break;
        }
    }
    
    float progress = (time - s1.t) / (s2.t - s1.t);
    *top = LerpColor(s1.top, s2.top, progress);
    *bot = LerpColor(s1.bot, s2.bot, progress);
}

// ============================================================================
// 1. INISIALISASI
// ============================================================================
void InitAnimasi2DScreen(Animasi2DScreen *s) {
    s->showHUD = true; s->activeTab = TAB_CONTROL;
    s->isPlaying = true; s->speedMult = 1.0f;
    s->weather = WEATHER_CLEAR; s->isWireframe = false;

    s->autoTime = true; s->manualTime = 12.0f; s->autoLighthouse = true;
    s->waveIntensity = 1.0f; s->windSpeedMult = 1.0f; s->shipSpeedMult = 1.0f;
    s->autoSpawnShips = true;

    s->realTime = 0.0f; s->timeOfDay = 6.0f; s->visualTime = 6.0f; 
    s->skyDarken = 0.0f; s->waveMult = 1.0f; s->windSpeed = 0.0f; s->birdMult = 1.0f;
    s->pWave = 0.0f; s->pCloud = 0.0f; s->pBird = 0.0f;
    s->lastSpawnTime = 0.0f;
    s->shipIdCounter = 0;
    
    s->customStarCount = 0;
    

    for(int i = 0; i < MAX_STARS; i++) {
        s->stars[i].nx = (float)GetRandomValue(0, 1000) / 1000.0f;
        s->stars[i].ny = (float)GetRandomValue(0, 600) / 1000.0f; 
        s->stars[i].size = (float)GetRandomValue(5, 15) / 10.0f;
        s->stars[i].twinkleSpeed = (float)GetRandomValue(10, 40) / 10.0f;
        s->stars[i].offset = (float)GetRandomValue(0, 628) / 100.0f;
    }
    for(int i = 0; i < MAX_CLOUDS; i++) {
        s->clouds[i].offset = (float)GetRandomValue(0, 2500);
        s->clouds[i].speed = (float)GetRandomValue(10, 20);
        s->clouds[i].nyBase = (float)GetRandomValue(30, 150) / 1000.0f; 
        s->clouds[i].scale = (float)GetRandomValue(8, 20) / 10.0f; 
        s->clouds[i].type = GetRandomValue(0, 3);
    }
    for(int i = 0; i < MAX_BIRDS; i++) {
        s->birds[i].offset = (float)GetRandomValue(0, 1000);
        s->birds[i].speed = (float)GetRandomValue(40, 100);
        s->birds[i].nyBase = (float)GetRandomValue(100, 300) / 1000.0f;
        s->birds[i].flapSpeed = (float)GetRandomValue(8, 13);
        s->birds[i].scale = (float)GetRandomValue(4, 8) / 10.0f;
    }
    
    for (int i = 0; i < MAX_SHIPS; i++) {
        s->ships[i].active = false;
    }

    SetPolaAlgorithm(false); 
}

// ============================================================================
// 2. UPDATE LOGIKA & FISIKA
// ============================================================================
void UpdateAnimasi2DScreen(Animasi2DScreen *s) {
    float rawDt = GetFrameTime();
    float uiDt = (rawDt > 0.1f) ? 0.1f : rawDt; 
    
    float dt = s->isPlaying ? uiDt * s->speedMult : 0.0f;

    if (s->isPlaying) {
        s->realTime += dt;
        if (s->autoTime) {
            s->timeOfDay += dt * 0.05f; 
            if (s->timeOfDay >= 24.0f) s->timeOfDay -= 24.0f;
        } else {
            s->timeOfDay = s->manualTime;
        }
    } else if (!s->autoTime) {
        s->timeOfDay = s->manualTime;
    }

    float timeDiff = s->timeOfDay - s->visualTime;
    if (timeDiff > 12.0f) timeDiff -= 24.0f;
    if (timeDiff < -12.0f) timeDiff += 24.0f;
    s->visualTime += timeDiff * (5.0f * uiDt);
    if (s->visualTime < 0.0f) s->visualTime += 24.0f;
    if (s->visualTime >= 24.0f) s->visualTime -= 24.0f;

    float tDarken = 0, tWaveM = 1.0f * s->waveIntensity, tWind = 0, tBird = 1.0f;
    if (s->weather == WEATHER_RAIN) {
        tDarken = 0.4f; tWaveM = 1.5f * s->waveIntensity; tWind = 5.0f * s->windSpeedMult; tBird = 1.5f;
    } else if (s->weather == WEATHER_STORM) {
        tDarken = 0.7f; tWaveM = 2.5f * s->waveIntensity; tWind = 15.0f * s->windSpeedMult; tBird = 2.5f;
    }

    float wLerp = 0.8f * uiDt;
    s->skyDarken += (tDarken - s->skyDarken) * wLerp;
    s->waveMult += (tWaveM - s->waveMult) * wLerp;
    s->windSpeed += (tWind - s->windSpeed) * wLerp;
    s->birdMult += (tBird - s->birdMult) * wLerp;

    s->pWave += dt * s->waveMult;
    s->pCloud += dt * s->windSpeedMult;
    s->pBird += dt * s->birdMult;

    if (IsKeyPressed(KEY_H)) s->showHUD = !s->showHUD;
    if (IsKeyPressed(KEY_SPACE)) s->isPlaying = !s->isPlaying;
    if (IsKeyPressed(KEY_W)) s->isWireframe = !s->isWireframe;
    if (IsKeyPressed(KEY_UP)) { 
        s->speedMult += 0.5f; 
        if(s->speedMult > 5.0f) s->speedMult = 5.0f; // Kunci maksimal di 5.0
    }
    if (IsKeyPressed(KEY_DOWN)) { 
        s->speedMult -= 0.5f; 
        if(s->speedMult < 0.5f) s->speedMult = 0.5f; // Kunci minimal di 0.5
    }

    previewTime = s->realTime;
    s->beamAngle = (PI - 0.6f) + sinf(s->realTime * 0.5f) * 0.6f;

    float W_log = GetScreenWidth() / s->responsiveScale;
    float H_log = GetScreenHeight() / s->responsiveScale;
    float seaY_log = H_log * 0.68f;

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        s->customStarCount = 0; 
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePosScreen = GetMousePosition();
        
        bool occludedByUI = false;
        if (s->showHUD) {
            Rectangle hudRec = { 20 * s->responsiveScale, 60 * s->responsiveScale, 310 * s->responsiveScale, 480 * s->responsiveScale }; 
            if (CheckCollisionPointRec(mousePosScreen, hudRec)) occludedByUI = true;
        }
        Rectangle toggleRec = { 20 * s->responsiveScale, 20 * s->responsiveScale, 90 * s->responsiveScale, 28 * s->responsiveScale };
        if (CheckCollisionPointRec(mousePosScreen, toggleRec)) occludedByUI = true;

        if (!occludedByUI) {
            float worldX = mousePosScreen.x / s->responsiveScale;
            float worldY = mousePosScreen.y / s->responsiveScale;
            bool isNight = (s->visualTime >= 18.0f || s->visualTime < 6.0f);

            // Bintang hanya bisa digambar jika isNight = true
            if (worldY < seaY_log && isNight && s->weather == WEATHER_CLEAR) {
                if (s->customStarCount < MAX_CUSTOM_STARS) {
                    s->customStars[s->customStarCount].x = worldX; 
                    s->customStars[s->customStarCount].y = worldY;
                    s->customStarCount++;
                }
            } 
        }
    }
    
    if (s->isPlaying) {
        // --- UPDATE PERGERAKAN KAPAL & EMISI ASAP ---
        for (int i = 0; i < MAX_SHIPS; i++) {
            if (s->ships[i].active) {
                s->ships[i].x += s->ships[i].speed * s->shipSpeedMult * dt;
                
                if (s->ships[i].x > W_log + 600.0f) {
                    s->ships[i].active = false;
                }
            }
        }

        // --- SPAWNER KAPAL ---
        if (s->autoSpawnShips && (s->realTime - s->lastSpawnTime > 5.0f)) {
            int countL3 = 0, countL2 = 0, countL1 = 0;
            bool clearL3 = true, clearL2 = true, clearL1 = true;
            
            float tebingScale = 1.0f; 
            float cliffX = W_log - (210.0f * tebingScale);
            float lhX = cliffX + (10.0f * tebingScale);

            bool activeVar[10][5] = {false};

            for (int i = 0; i < MAX_SHIPS; i++) {
                if (s->ships[i].active) {
                    if (s->ships[i].layer == 3) { countL3++; if (s->ships[i].x <= lhX + 150.0f) clearL3 = false; }
                    if (s->ships[i].layer == 2) { countL2++; if (s->ships[i].x <= 300.0f) clearL2 = false; }
                    if (s->ships[i].layer == 1) { countL1++; if (s->ships[i].x <= 250.0f) clearL1 = false; }
                    
                    if (s->ships[i].x < lhX + 300.0f) {
                        activeVar[s->ships[i].type][s->ships[i].variant] = true;
                    }
                }
            }

            int allowedTypes[9]; int allowedCount = 0;
            if (countL3 == 0 || (countL3 < 1 && clearL3)) { allowedTypes[allowedCount++] = SHIP_CARGO; allowedTypes[allowedCount++] = SHIP_CRUISE; allowedTypes[allowedCount++] = SHIP_MILITARY; }
            if (countL2 < 3 && clearL2) { allowedTypes[allowedCount++] = SHIP_FISHING; allowedTypes[allowedCount++] = SHIP_RESEARCH; allowedTypes[allowedCount++] = SHIP_PIRATE; allowedTypes[allowedCount++] = SHIP_GALLEON; }
            if (countL1 < 3 && clearL1) { allowedTypes[allowedCount++] = SHIP_YACHT; allowedTypes[allowedCount++] = SHIP_SLOOP; }

            if (allowedCount > 0) {
                int randType = allowedTypes[GetRandomValue(0, allowedCount - 1)];
                int maxVariants = 1;
                if (randType == SHIP_PIRATE || randType == SHIP_SLOOP) maxVariants = 3;
                if (randType == SHIP_GALLEON) maxVariants = 2;
                
                int availableVariants[5]; int availVarCount = 0;
                for (int v = 0; v < maxVariants; v++) {
                    if (!activeVar[randType][v]) availableVariants[availVarCount++] = v;
                }
                
                if (availVarCount > 0) {
                    int slot = -1;
                    for (int i = 0; i < MAX_SHIPS; i++) { if (!s->ships[i].active) { slot = i; break; } }
                    if (slot != -1) {
                        int chosenVariant = availableVariants[GetRandomValue(0, availVarCount - 1)];
                        ShipData *sp = &s->ships[slot];
                        sp->active = true; sp->id = s->shipIdCounter++; 
                        sp->type = randType; sp->variant = chosenVariant; 
                        sp->x = -150.0f - (float)GetRandomValue(0, 100);
                        
                        if (randType == SHIP_CARGO)   { sp->layer = 3; sp->scale = 0.90f; sp->speed = 18.0f; }
                        if (randType == SHIP_CRUISE)  { sp->layer = 3; sp->scale = 1.40f; sp->speed = 22.0f; }
                        if (randType == SHIP_MILITARY){ sp->layer = 3; sp->scale = 0.90f; sp->speed = 45.0f; }
                        if (randType == SHIP_FISHING) { sp->layer = 2; sp->scale = 0.60f; sp->speed = 35.0f; }
                        if (randType == SHIP_RESEARCH){ sp->layer = 2; sp->scale = 0.80f; sp->speed = 30.0f; }
                        if (randType == SHIP_PIRATE)  { sp->layer = 2; sp->scale = 0.85f; sp->speed = 25.0f; }
                        if (randType == SHIP_GALLEON) { sp->layer = 2; sp->scale = 0.80f; sp->speed = 28.0f; }
                        if (randType == SHIP_YACHT)   { sp->layer = 1; sp->scale = 0.60f; sp->speed = 50.0f; }
                        if (randType == SHIP_SLOOP)   { sp->layer = 1; sp->scale = 0.70f; sp->speed = 40.0f; }
                        
                        sp->speed += (float)GetRandomValue(-400, 400) / 100.0f; 
                        s->lastSpawnTime = s->realTime;
                    }
                }
            }
        }
    } 
}

// ============================================================================
// 3. RENDER VISUAL (DRAWING ENGINE)
// ============================================================================
void DrawAnimasi2DScreen(Animasi2DScreen *s) {
    int logW = GetScreenWidth(); 
    int logH = GetScreenHeight();
    
    float baseW = 1440.0f;
    float scale = logW / baseW;
    if (scale < 0.3f) scale = 0.3f;
    if (scale > 1.5f) scale = 1.5f;
    s->responsiveScale = scale;

    float W = logW / scale;
    float H = logH / scale;

    Camera2D camera = { 0 };
    camera.zoom = scale;
    BeginMode2D(camera);

    // --- 1. MENGGAMBAR LANGIT ---
    Color topC, botC;
    GetSkyColor(s->visualTime, &topC, &botC);
    
    topC.r = (unsigned char)(topC.r * (1.0f - s->skyDarken));
    topC.g = (unsigned char)(topC.g * (1.0f - s->skyDarken));
    topC.b = (unsigned char)(topC.b * (1.0f - s->skyDarken));
    
    botC.r = (unsigned char)(botC.r * (1.0f - s->skyDarken));
    botC.g = (unsigned char)(botC.g * (1.0f - s->skyDarken));
    botC.b = (unsigned char)(botC.b * (1.0f - s->skyDarken));

    if (!s->isWireframe) DrawRectangleGradientV(0, 0, W, H, topC, botC);
    else ClearBackground(BLACK);

    // --- 2. MATAHARI & BULAN ---
    float t = s->visualTime;
    float daylight = fmaxf(0.0f, fminf(1.0f, sinf((t - 6.0f) * PI / 12.0f)));
    float nightlight = 1.0f - daylight;
    bool isNight = (t >= 18.0f || t < 6.0f);

    float sunAngle = (t - 6.0f) * PI / 12.0f;
    float sunX = W / 2.0f - cosf(sunAngle) * (W * 0.4f);
    float sunY = H * 0.8f - sinf(sunAngle) * (H * 0.6f);

    float moonAngle = (t + 6.0f) * PI / 12.0f;
    float moonX = W / 2.0f - cosf(moonAngle) * (W * 0.4f);
    float moonY = H * 0.8f - sinf(moonAngle) * (H * 0.6f);

    if (daylight > 0 && s->weather == WEATHER_CLEAR) {
        DrawMatahari_Animasi(sunX, sunY, 1.0f, daylight, s->isWireframe);
    }
    if (nightlight > 0) {
        DrawBulan_Animasi(moonX, moonY, 1.0f, nightlight, s->isWireframe);
    }

    // --- 3. BINTANG MALAM & RASI KUSTOM ---
    if (isNight) { 
        for(int i = 0; i < MAX_STARS; i++) {
            float twinkle = 0.5f + 0.5f * sinf(s->realTime * s->stars[i].twinkleSpeed + s->stars[i].offset);
            float alpha = nightlight * twinkle;
            if (s->isWireframe) {
                DrawCircleLinesV((Vector2){s->stars[i].nx * W, s->stars[i].ny * H}, s->stars[i].size, GREEN);
            } else {
                Color starCol = (Color){255, 255, 255, (unsigned char)(255.0f * alpha)};
                DrawCircleV((Vector2){s->stars[i].nx * W, s->stars[i].ny * H}, s->stars[i].size, starCol);
            }
        }

        if (s->weather == WEATHER_CLEAR && s->customStarCount > 0) {
            // Memanggil fungsi menggambar rasi dengan argumen presisi
            DrawRasiBintang_Animasi(s->customStars, s->customStarCount, nightlight, s->isWireframe);
        }
    }

    // --- 4. AWAN ---
    float cloudBase = 255.0f - (s->skyDarken * 150.0f);
    unsigned char cR = (unsigned char)(cloudBase * (1.0f - nightlight * 0.7f));
    unsigned char cG = (unsigned char)(cloudBase * (1.0f - nightlight * 0.7f));
    float bVal = cloudBase + 20.0f;
    if (bVal > 255.0f) bVal = 255.0f;
    unsigned char cB = (unsigned char)(bVal * (1.0f - nightlight * 0.6f));
    unsigned char cA = (unsigned char)((0.8f - s->skyDarken * 0.2f) * 255.0f);
    Color cloudCol = s->isWireframe ? GREEN : (Color){cR, cG, cB, cA};

    for (int i = 0; i < MAX_CLOUDS; i++) {
        float cx = fmodf((s->pCloud * s->clouds[i].speed + s->clouds[i].offset), W + 600.0f) - 300.0f;
        float cy = s->clouds[i].nyBase * H;
        DrawAwan_Pola(s->clouds[i].type, cx, cy, s->clouds[i].scale, cloudCol, s->isWireframe);
    }

    // --- 5. GUNUNG LATAR BELAKANG ---
    float seaY = H * 0.68f; 
    Color mountCol = LerpColor((Color){100, 120, 150, 255}, (Color){20, 25, 40, 255}, s->skyDarken);
    float mountBaseY = H * 0.55f; 
    float mountHeight = 800.0f; 
    DrawGunung_Pola(W / 2.0f, mountBaseY, W, mountHeight, mountCol, s->isWireframe);

    // --- 6. KABUT ATMOSFER ---
    float fogStart = H * 0.45f;
    float fogEnd = H * 0.75f;
    float fogAlpha = 0.15f + (s->skyDarken * 0.9f);
    if (!s->isWireframe) {
        Color fTop = (Color){210, 225, 240, 0};
        Color fBot = (Color){210, 225, 240, (unsigned char)(fogAlpha * 255.0f)};
        DrawRectangleGradientV(0, (int)fogStart, (int)W, (int)(fogEnd - fogStart), fTop, fBot);
    }

    // --- 7. BURUNG CAMAR ---
    if (daylight > 0.0f && s->weather == WEATHER_CLEAR) {
        for (int i = 0; i < MAX_BIRDS; i++) {
            float bx = fmodf((s->pBird * s->birds[i].speed + s->birds[i].offset), W + 200.0f) - 100.0f;
            float by = (s->birds[i].nyBase * H) + (40.0f * s->waveMult) * sinf(bx * 0.005f);
            float flap = sinf(s->pBird * s->birds[i].flapSpeed);
            Color birdCol = s->isWireframe ? GREEN : (Color){34, 34, 34, (unsigned char)(255.0f * daylight * (1.0f - s->skyDarken))};
            DrawBurung_Pola(bx, by, s->birds[i].scale, flap, birdCol, s->isWireframe);
        }
    }

    // ==========================================
    // 8. KALKULUS PRESISI VISUAL MERCUSUAR
    // ==========================================
    float tebingScale = 1.0f;      
    float mercusuarScale = 1.5f;   
    
    float cliffX = W - (210.0f * tebingScale); 
    float cliffY = seaY - 70.0f; 

    float lhX = cliffX + (10.0f * tebingScale); 
    
    float lhY = cliffY + (0.0f * tebingScale) - (70.0f * mercusuarScale); 

    float lightSourceY = lhY + (70.0f * mercusuarScale) - (197.0f * mercusuarScale);

    for (int i = 0; i < MAX_SHIPS; i++) {
        if (!s->ships[i].active) continue;
        ShipData *sp = &s->ships[i];
        
        float waveA, waveK, waveW, offsetY;
        if (sp->layer == 3) { waveA = 15; waveK = 0.005; waveW = 1.5; offsetY = -40; } 
        else if (sp->layer == 2) { waveA = 20; waveK = 0.008; waveW = 2.0; offsetY = -10; }
        else { waveA = 25; waveK = 0.010; waveW = 2.5; offsetY = 30; }
        
        waveA *= s->waveMult;
        float baseSpeed = waveW; 

        sp->y = seaY + offsetY + waveA * sinf(sp->x * waveK + s->pWave * baseSpeed);
        sp->dyDx = waveA * waveK * cosf(sp->x * waveK + s->pWave * baseSpeed);
        sp->angle = atanf(sp->dyDx);

        sp->isIlluminated = false;
        if (isNight) {
            float angleToShip = atan2f(sp->y - lightSourceY, sp->x - lhX);
            float diff = angleToShip - s->beamAngle;
            while (diff > PI) diff -= PI * 2.0f;
            while (diff < -PI) diff += PI * 2.0f;
            if (fabsf(diff) < 0.6f / 2.0f) sp->isIlluminated = true;
        }
    }

    float flap = sinf(s->realTime * 4.0f) * 8.0f * s->waveMult;

    // --- 9. RENDER LAYER 3 (BELAKANG) ---
    int r3 = (int)((10 + daylight*20)*(1.0f-s->skyDarken));
    int g3 = (int)((40 + daylight*60)*(1.0f-s->skyDarken));
    int b3 = (int)((80 + daylight*100)*(1.0f-s->skyDarken));
    Color cTop3 = (Color){r3, g3, b3, 255};
    Color cBot3 = (Color){(unsigned char)(r3*0.4f), (unsigned char)(g3*0.5f), (unsigned char)(b3*0.6f), 255};
    
    DrawOmbak_Lapisan(0, W, H + 50.0f, seaY, 15 * s->waveMult, 0.005f, 1.5f, cTop3, cBot3, -40, true, s->pWave, daylight, s->isWireframe);

    for (int i = 0; i < MAX_SHIPS; i++) {
        if (s->ships[i].active && s->ships[i].layer == 3) {
            Color hull = s->ships[i].isIlluminated ? (Color){255,230,120,255} : (Color){80,60,40,255}; 
            Color detail = s->ships[i].isIlluminated ? (Color){255,255,200,255} : (Color){200,200,200,255};
            if(s->ships[i].type == SHIP_CARGO) DrawShip_Cargo(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, hull, detail, s->isWireframe);
            if(s->ships[i].type == SHIP_CRUISE) DrawShip_Cruise(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, hull, detail, s->isWireframe);
            if(s->ships[i].type == SHIP_MILITARY) DrawShip_Military(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, s->ships[i].isIlluminated ? hull : GRAY, DARKGRAY, s->isWireframe);
        }
    }

    // --- 10. RENDER LAYER 2 (TENGAH) ---
    int r2 = (int)((15 + daylight*30)*(1.0f-s->skyDarken));
    int g2 = (int)((50 + daylight*80)*(1.0f-s->skyDarken));
    int b2 = (int)((100 + daylight*120)*(1.0f-s->skyDarken));
    Color cTop2 = (Color){r2, g2, b2, 255};
    Color cBot2 = (Color){(unsigned char)(r2*0.4f), (unsigned char)(g2*0.5f), (unsigned char)(b2*0.6f), 255};

    DrawOmbak_Lapisan(0, W, H + 50.0f, seaY, 20 * s->waveMult, 0.008f, 2.0f, cTop2, cBot2, -10, true, s->pWave, daylight, s->isWireframe);
    
    for (int i = 0; i < MAX_SHIPS; i++) {
        if (s->ships[i].active && s->ships[i].layer == 2) {
            Color hull = s->ships[i].isIlluminated ? (Color){255,230,120,255} : (Color){80,60,40,255}; 
            if(s->ships[i].type == SHIP_FISHING) DrawShip_Fishing(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, hull, ORANGE, s->isWireframe);
            if(s->ships[i].type == SHIP_RESEARCH) DrawShip_Research(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, s->ships[i].isIlluminated ? hull : RED, WHITE, s->isWireframe);
            if(s->ships[i].type == SHIP_PIRATE) DrawShip_Pirate_Variant(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, flap, s->ships[i].variant, s->isWireframe);
            if(s->ships[i].type == SHIP_GALLEON) DrawShip_Galleon(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, flap, s->ships[i].variant, s->isWireframe);
        }
    }

    // --- 10.5 TEBING & MERCUSUAR ---
    Color rockCol = LerpColor((Color){60, 60, 70, 255}, (Color){15, 15, 20, 255}, s->skyDarken);
    Color grassCol = LerpColor((Color){34, 139, 34, 255}, (Color){10, 40, 10, 255}, s->skyDarken);
    
    DrawTebing_Pola(cliffX, cliffY, tebingScale, 0.0f, rockCol, grassCol, s->isWireframe);
    
    DrawMercusuar_Pola(lhX, lhY, mercusuarScale, 1, s->isWireframe);

    // --- 11. RENDER LAYER 1 (DEPAN) & PARTIKEL ASAP ---
    int r1 = (int)((20 + daylight*40)*(1.0f-s->skyDarken));
    int g1 = (int)((60 + daylight*100)*(1.0f-s->skyDarken));
    int b1 = (int)((120 + daylight*135)*(1.0f-s->skyDarken));
    Color cTop1 = (Color){r1, g1, b1, 255};
    Color cBot1 = (Color){(unsigned char)(r1*0.4f), (unsigned char)(g1*0.5f), (unsigned char)(b1*0.6f), 255};

    DrawOmbak_Lapisan(0, W, H + 50.0f, seaY, 25 * s->waveMult, 0.010f, 2.5f, cTop1, cBot1, 30, true, s->pWave, daylight, s->isWireframe);

    for (int i = 0; i < MAX_SHIPS; i++) {
        if (s->ships[i].active && s->ships[i].layer == 1) {
            Color detail = s->ships[i].isIlluminated ? (Color){255,255,200,255} : (Color){200,200,200,255};
            Color bodyYacht = s->ships[i].isIlluminated ? (Color){255,230,120,255} : WHITE;
            if(s->ships[i].type == SHIP_YACHT) DrawShip_Yacht(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, bodyYacht, detail, s->isWireframe);
            if(s->ships[i].type == SHIP_SLOOP) DrawShip_Sloop_Variant(s->ships[i].x, s->ships[i].y, s->ships[i].scale, s->ships[i].angle, flap, s->ships[i].variant, s->isWireframe);
        }
    }

    // --- 11.2 EFEK MALAM & SAKELAR MERCUSUAR ---
    if (nightlight > 0.0f && !s->isWireframe) {
        // Layar digelapkan secara halus / smooth
        DrawRectangle(0, 0, W, H, (Color){5, 10, 20, (unsigned char)(nightlight * 140.0f)});
    }

    if (isNight && !s->isWireframe) {
        // Sinar dan Bohlam Mercusuar (Mati jam 06.00, Nyala jam 18.00)
        Vector2 lCenter = { lhX, lightSourceY };
        float spread = 0.3f;
        float beamLength = 3000.0f * mercusuarScale;

        Vector2 p1 = { lCenter.x + cosf(s->beamAngle - spread) * beamLength, lCenter.y + sinf(s->beamAngle - spread) * beamLength };
        Vector2 p2 = { lCenter.x + cosf(s->beamAngle + spread) * beamLength, lCenter.y + sinf(s->beamAngle + spread) * beamLength };

        float minX = lhX - 3000.0f;
        float maxX = lhX + 3000.0f;
        float minY = lhY - 463.0f; 
        float maxY = lhY + 3000.0f; 

        // Lampu menyala dengan kekuatan putih stabil
        DrawClippedBeamTriangle(lCenter, p1, p2, minX, minY, maxX, maxY, (Color){255, 255, 150, 40});
        
        Vector2 p1_in = { lCenter.x + cosf(s->beamAngle - spread*0.3f) * beamLength, lCenter.y + sinf(s->beamAngle - spread*0.3f) * beamLength };
        Vector2 p2_in = { lCenter.x + cosf(s->beamAngle + spread*0.3f) * beamLength, lCenter.y + sinf(s->beamAngle + spread*0.3f) * beamLength };
        DrawClippedBeamTriangle(lCenter, p1_in, p2_in, minX, minY, maxX, maxY, (Color){255, 255, 200, 80});

        DrawCircleV(lCenter, 6.0f * mercusuarScale, (Color){255, 255, 200, 255});
        DrawCircleV(lCenter, 12.0f * mercusuarScale, (Color){255, 255, 100, 100});
    }

    // --- 12. EFEK CUACA HUJAN & PETIR ---
    if (s->weather != WEATHER_CLEAR) {
        bool isStorm = (s->weather == WEATHER_STORM);
        DrawCuaca_HujanBadai(W / 2.0f, H / 2.0f, W, H, isStorm, s->isWireframe);
    }

    EndMode2D();

    DrawCustomHUD(s);
}

void UnloadAnimasi2DScreen(Animasi2DScreen *s) {}