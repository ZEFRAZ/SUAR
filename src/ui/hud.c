/**
 * @file hud.c
 * @brief Panel kontrol (HUD) untuk layar Animasi 2D.
 *
 * File ini mengurusi tampilan panel di sisi kiri layar yang berisi
 * tab Kontrol (atur cuaca, waktu, ombak) dan tab Armada (tambah kapal ke simulasi).
 */

#include "hud.h"
#include <math.h>
#include "draw_helpers.h"

// =============================================================================
// BAGIAN 1: FONT DAN VARIABEL STATUS INTERNAL
// =============================================================================
static Font hudFont        = { 0 };
static bool isHudFontLoaded = false;

/**
 * @brief Fungsi penulisan teks pada layar dengan font khusus.
 *
 * @param text Teks yang mau dicetak.
 * @param x Posisi X di layar.
 * @param y Posisi Y di layar.
 * @param fontSize Ukuran font.
 * @param color Warna teks.
 */
static void DrawHudText(const char* text, float x, float y, float fontSize, Color color) {
    if (!isHudFontLoaded) {
        DrawText(text, (int)x, (int)y, (int)fontSize, color); 
    } else {
        DrawTextEx(hudFont, text, (Vector2){x, y}, fontSize, 1.0f, color);
    }
}

/**
 * @brief Fungsi menghitung lebar suatu string (teks).
 * 
 * Digunakan untuk menghitung lebar teks dalam satuan piksel agar kita bisa
 * menaruh teks persis di tengah-tengah sebuah tombol (Text Centering).
 * 
 * @param text Teks string.
 * @param fontSize Ukuran font.
 * @return float Total lebar teks dalam piksel.
 */
static float GetHudTextWidth(const char* text, float fontSize) {
    if (!isHudFontLoaded) return (float)MeasureText(text, (int)fontSize);
    return MeasureTextEx(hudFont, text, fontSize, 1.0f).x;
}

// =============================================================================
// Konstanta Skema Pewarnaan Antarmuka (GUI Stateful Color Palette)
// =============================================================================
/**
 * @brief Fungsi pengambil warna background tombol berdasarkan statusnya.
 * 
 * Mengembalikan warna latar tombol tergantung dari apakah tombolnya sedang diklik (Active),
 * sedang ditunjuk kursor (Hover), atau mati (Disabled).
 * 
 * @param isActive Apakah tombol sedang diklik/terpilih.
 * @param isHover Apakah kursor mouse sedang berada di atas tombol.
 * @param isDisabled Apakah tombol sedang terkunci (mati).
 * @return Color Warna latar belakang tombol.
 */
static Color GetBgColor(bool isActive, bool isHover, bool isDisabled) {
    if (isDisabled) return (Color){30, 35, 40, 200}; 
    if (isActive)   return (Color){0, 130, 180, 255}; 
    if (isHover)    return (Color){0, 100, 150, 255}; 
    return (Color){10, 45, 80, 220};                  
}

/**
 * @brief Fungsi pengambil warna garis tepi pinggiran (border) tombol.
 * 
 * Berguna untuk memberikan efek neon yang menyala di tepian tombol
 * sesuai dengan status tombol tersebut (Aktif/Hover/Mati).
 * 
 * @param isActive Apakah tombol sedang diklik.
 * @param isHover Apakah kursor di atas tombol.
 * @param isDisabled Apakah tombol terkunci.
 * @return Color Warna pinggiran tombol.
 */
static Color GetBorderColor(bool isActive, bool isHover, bool isDisabled) {
    if (isDisabled) return (Color){60, 65, 70, 255};
    if (isActive)   return (Color){0, 255, 255, 255}; 
    if (isHover)    return (Color){0, 200, 255, 255}; 
    return (Color){30, 100, 150, 255};                
}

// =============================================================================
// Prosedur Vektorisasi Ikonografik (Manual Vector Iconography)
// =============================================================================
/**
 * @brief Gambar Ikon UI sederhana (Play, Pause, Matahari, dll) dengan vektor.
 * 
 * Ketimbang import banyak gambar PNG yang memperberat game, ikon di UI Hud
 * digambar secara murni menggunakan perpaduan garis, lingkaran, dan segitiga biasa.
 * 
 * @param type ID Jenis ikon (misal: 1=Mata, 2=Kunci, 5=Matahari).
 * @param cx Posisi X pusat ikon.
 * @param cy Posisi Y pusat ikon.
 * @param col Warna garis ikon.
 */
static void DrawIconShape(int type, float cx, float cy, Color col) {
    switch(type) {
        case 1: // MATA (SHOW)
            DrawEllipseLines((int)cx, (int)cy, 8, 5, col);
            DrawCircle((int)cx, (int)cy, 2, col);
            break;
        case 2: // MATA TERSILANG (HIDE)
            DrawEllipseLines((int)cx, (int)cy, 8, 5, col);
            DrawCircle((int)cx, (int)cy, 2, col);
            DrawLineEx((Vector2){cx-8, cy-6}, (Vector2){cx+8, cy+6}, 2.0f, col);
            break;
        case 3: // PLAY (Segitiga)
            FillTri_Fast((Vector2){cx-3, cy-5}, (Vector2){cx-3, cy+5}, (Vector2){cx+5, cy}, col);
            break;
        case 4: // PAUSE (Dua Garis)
            DrawRectangle((int)cx-4, (int)cy-4, 3, 9, col);
            DrawRectangle((int)cx+2, (int)cy-4, 3, 9, col);
            break;
        case 5: // CERAH (Matahari)
            DrawCircleLines((int)cx, (int)cy, 3, col);
            for(int i=0; i<8; i++) {
                float a = i * PI / 4.0f;
                DrawLineEx((Vector2){cx + cosf(a)*4, cy + sinf(a)*4}, (Vector2){cx + cosf(a)*7, cy + sinf(a)*7}, 1.5f, col);
            }
            break;
        case 6: // HUJAN (Awan + Air)
            DrawCircle((int)cx-3, (int)cy-2, 3, col); DrawCircle((int)cx+3, (int)cy-2, 3, col);
            DrawCircle((int)cx, (int)cy-4, 4, col);   DrawRectangle((int)cx-3, (int)cy-2, 7, 4, col);
            DrawLine((int)cx-3, (int)cy+3, (int)cx-4, (int)cy+6, col);
            DrawLine((int)cx, (int)cy+3, (int)cx-1, (int)cy+6, col);
            DrawLine((int)cx+3, (int)cy+3, (int)cx+2, (int)cy+6, col);
            break;
        case 7: // BADAI (Awan + Petir)
            DrawCircle((int)cx-3, (int)cy-3, 3, col); DrawCircle((int)cx+3, (int)cy-3, 3, col);
            DrawCircle((int)cx, (int)cy-5, 4, col);   DrawRectangle((int)cx-3, (int)cy-3, 7, 4, col);
            DrawLineEx((Vector2){cx, cy+1}, (Vector2){cx-2, cy+4}, 1.5f, col);
            DrawLineEx((Vector2){cx-2, cy+4}, (Vector2){cx+1, cy+4}, 1.5f, col);
            DrawLineEx((Vector2){cx+1, cy+4}, (Vector2){cx-1, cy+8}, 1.5f, col);
            break;
    }
}

// =============================================================================
// Komponen Antarmuka Pengguna Kustom (Custom User Interface Components)
// =============================================================================
/**
 * @brief Fungsi untuk menggambar tombol biasa (Push Button).
 * 
 * Menggambar tombol kotak melengkung (rounded) dan mengecek apakah 
 * titik kursor mouse berada di dalam kotak tersebut (Collision).
 * Selain kotak tombol, fungsi ini juga menggambar pantulan kaca transparan 
 * di separuh atas tombol agar terlihat seperti tombol UI modern.
 * 
 * @param rect Ukuran dan posisi kotak tombol.
 * @param text Tulisan di dalam tombol.
 * @param mousePos Posisi X dan Y mouse saat ini.
 * @param isActive Apakah tombol sedang ditekan secara aktif.
 * @param isDisabled Apakah tombol sedang dimatikan (tidak bisa diklik).
 * @return true Jika tombol diklik (Hover + klik kiri mouse).
 */
static bool DrawOceanButton(Rectangle rect, const char* text, Vector2 mousePos, bool isActive, bool isDisabled) {
    bool isHover = !isDisabled && CheckCollisionPointRec(mousePos, rect);
    Color bgCol = GetBgColor(isActive, isHover, isDisabled);
    Color borderCol = GetBorderColor(isActive, isHover, isDisabled);
    
    DrawRectangleRounded(rect, 0.4f, 10, bgCol);
    Rectangle glossRec = { rect.x + 2, rect.y + 2, rect.width - 4, rect.height * 0.45f };
    DrawRectangleRounded(glossRec, 0.4f, 10, (Color){ 255, 255, 255, 25 });
    DrawRectangleRoundedLines(rect, 0.4f, 10, borderCol);
    
    float fontSize = 12.0f;
    float textW = GetHudTextWidth(text, fontSize);
    DrawHudText(text, rect.x + rect.width/2.0f - textW/2.0f, rect.y + rect.height/2.0f - fontSize/2.0f, fontSize, isDisabled ? GRAY : WHITE);
    
    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

/**
 * @brief Fungsi untuk menggambar tombol yang memiliki ikon.
 * 
 * Mirip dengan DrawOceanButton, tapi fungsi ini juga menambahkan gambar ikon
 * dari DrawIconShape (seperti Pause, Matahari, Badai). Jika teks dikosongkan (""),
 * maka yang muncul cuma gambar ikon saja di tengah tombol.
 * 
 * @param rect Kotak batasan tombol.
 * @param iconType ID angka jenis ikon (misalnya: 3 untuk Play).
 * @param text Teks opsional di samping ikon.
 * @param mousePos Posisi mouse saat ini.
 * @param isActive Status tombol nyala terus menerus.
 * @param isDisabled Status tombol mati total.
 * @return true Jika area tombol ini berhasil diklik.
 */
static bool DrawOceanIconButton(Rectangle rect, int iconType, const char* text, Vector2 mousePos, bool isActive, bool isDisabled) {
    bool isHover = !isDisabled && CheckCollisionPointRec(mousePos, rect);
    Color bgCol = GetBgColor(isActive, isHover, isDisabled);
    Color borderCol = GetBorderColor(isActive, isHover, isDisabled);
    
    DrawRectangleRounded(rect, 0.4f, 10, bgCol);
    Rectangle glossRec = { rect.x + 2, rect.y + 2, rect.width - 4, rect.height * 0.45f };
    DrawRectangleRounded(glossRec, 0.4f, 10, (Color){ 255, 255, 255, 25 });
    DrawRectangleRoundedLines(rect, 0.4f, 10, borderCol);
    
    Color fgCol = isDisabled ? GRAY : WHITE;
    float fontSize = 13.0f;
    float textW = GetHudTextWidth(text, fontSize);

    if (iconType > 0 && textW > 0) { 
        float totalW = 12.0f + 4.0f + textW; 
        float startX = rect.x + rect.width/2.0f - totalW/2.0f;
        DrawIconShape(iconType, startX + 6.0f, rect.y + rect.height/2.0f, fgCol);
        DrawHudText(text, startX + 16.0f, rect.y + rect.height/2.0f - fontSize/2.0f, fontSize, fgCol);
    } else if (iconType > 0) { 
        DrawIconShape(iconType, rect.x + rect.width/2.0f, rect.y + rect.height/2.0f, fgCol);
    }
    
    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

/**
 * @brief Fungsi untuk menggambar tombol pemanggil (Spawner) Kapal.
 * 
 * Dipakai khusus di dalam Menu Armada (Tab Armada). Tombol ini punya bentuk 
 * dan tulisan yang sedikit berbeda, di mana ada teks Utama (Tipe Kapal) dan
 * teks kecil transparan di bawahnya (Varian Warna Kapal).
 * 
 * @param rect Ukuran dan posisi tombol kapal.
 * @param type Teks kelompok tipe kapal besar (misal: GALLEON).
 * @param variant Teks kecil subvarian kapal (misal: Red/Black).
 * @param mousePos Posisi mouse.
 * @param isDisabled Jika true, tombol menjadi mati (misal saat fitur Auto-Spawn nyala).
 * @return true Jika tombol ditekan agar memunculkan kapal tersebut ke layar.
 */
static bool DrawOceanShipButton(Rectangle rect, const char* type, const char* variant, Vector2 mousePos, bool isDisabled) {
    bool isHover = !isDisabled && CheckCollisionPointRec(mousePos, rect);
    DrawRectangleRounded(rect, 0.4f, 10, GetBgColor(false, isHover, isDisabled));
    DrawRectangleRounded((Rectangle){ rect.x + 2, rect.y + 2, rect.width - 4, rect.height * 0.45f }, 0.4f, 10, (Color){ 255, 255, 255, 20 });
    DrawRectangleRoundedLines(rect, 0.4f, 10, GetBorderColor(false, isHover, isDisabled));
    
    float t1W = GetHudTextWidth(type, 11.0f);
    float t2W = GetHudTextWidth(variant, 9.0f);
    DrawHudText(type, rect.x + rect.width/2.0f - t1W/2.0f, rect.y + 6.0f, 11.0f, isDisabled ? GRAY : WHITE);
    DrawHudText(variant, rect.x + rect.width/2.0f - t2W/2.0f, rect.y + 18.0f, 9.0f, isDisabled ? DARKGRAY : LIGHTGRAY);
    return (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON));
}

/**
 * @brief Pengendali Nilai Geser Linier (Linear Value Manipulator / Slider Drag-Bar).
 * 
 * Mengawasi pergerakan klik+tarik (Drag) pengguna secara horizontal pada selang `min` hingga `max`.
 * Apabila area terdeteksi sedang ditekan (IsMouseButtonDown), nilai dalam *pointer* referensi `value`
 * akan diinterpolasi dan dimodifikasi secara sekejap (Real-time).
 * 
 * @param rect Batasan rel atau lintasan *slider*.
 * @param label Nama properti (contoh: Laju Kapal).
 * @param value *Pointer* variabel numerik (target modifikasi memori).
 * @param min   Batas minimal ekstrim kiri ujung geser.
 * @param max   Batas maksimal ekstrim kanan ujung geser.
 * @param mousePos Penunjuk piksel tetikus.
 * @param isDisabled Sakelar mematikan komponen (Abu-abu/Tidak responsif).
 */
static void DrawOceanSlider(Rectangle rect, const char* label, float *value, float min, float max, Vector2 mousePos, bool isDisabled) {
    DrawHudText(label, rect.x, rect.y - 15.0f, 11.0f, isDisabled ? GRAY : LIGHTGRAY);
    
    if (max == 24.0f) {
        int hr = (int)(*value); int mn = (int)((*value - hr) * 60.0f);
        DrawHudText(TextFormat("%02d:%02d", hr, mn), rect.x + rect.width - 40.0f, rect.y - 15.0f, 11.0f, isDisabled ? GRAY : WHITE);
    } else {
        DrawHudText(TextFormat("%.1f", *value), rect.x + rect.width - 25.0f, rect.y - 15.0f, 11.0f, isDisabled ? GRAY : WHITE);
    }

    Rectangle hitBox = {rect.x, rect.y - 5, rect.width, rect.height + 10};
    if (!isDisabled && CheckCollisionPointRec(mousePos, hitBox) && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        float percent = (mousePos.x - rect.x) / rect.width;
        
        // Batasan interpolasi manual parameter rasio (Clamp Ratio)
        if (percent < 0.0f) percent = 0.0f; 
        if (percent > 1.0f) percent = 1.0f;
        
        *value = min + percent * (max - min);
    }

    DrawRectangleRounded(rect, 0.5f, 5, (Color){ 0, 0, 0, 150 });
    float percent = (*value - min) / (max - min);
    DrawRectangleRounded((Rectangle){ rect.x, rect.y, rect.width * percent, rect.height }, 0.5f, 5, isDisabled ? GRAY : GetBgColor(true, false, false)); 
    DrawCircle((int)(rect.x + rect.width * percent), (int)(rect.y + rect.height/2), 6.0f, isDisabled ? DARKGRAY : WHITE);
}

static void DrawOceanCheckbox(Rectangle rect, const char* text, bool *value, Vector2 mousePos) {
    bool isHover = CheckCollisionPointRec(mousePos, rect);
    if (isHover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) *value = !(*value);
    
    Rectangle box = {rect.x, rect.y, 14, 14};
    DrawRectangleRounded(box, 0.2f, 4, GetBgColor(false, isHover, false));
    DrawRectangleRoundedLines(box, 0.2f, 4, GetBorderColor(false, isHover, false));
    
    if (*value) DrawRectangleRounded((Rectangle){box.x + 3, box.y + 3, 8, 8}, 0.2f, 4, GetBorderColor(true, false, false));
    DrawHudText(text, rect.x + 22.0f, rect.y, 11.0f, WHITE);
}

static void SpawnShipManual(Animasi2DScreen *s, int type, int variant) {
    int slot = -1;
    for (int i = 0; i < MAX_SHIPS; i++) { if (!s->ships[i].active) { slot = i; break; } }
    if (slot != -1) {
        ShipData *sp = &s->ships[slot];
        sp->active = true; sp->id = s->shipIdCounter++; sp->type = type; sp->variant = variant;
        sp->x = -150.0f - (float)GetRandomValue(0, 100); 
        
        if (type == SHIP_CARGO)   { sp->layer = 3; sp->scale = 0.60f; sp->speed = 18.0f; }
        if (type == SHIP_CRUISE)  { sp->layer = 3; sp->scale = 1.30f; sp->speed = 22.0f; }
        if (type == SHIP_MILITARY){ sp->layer = 3; sp->scale = 0.80f; sp->speed = 45.0f; }
        if (type == SHIP_FISHING) { sp->layer = 2; sp->scale = 0.50f; sp->speed = 35.0f; }
        if (type == SHIP_RESEARCH){ sp->layer = 2; sp->scale = 0.70f; sp->speed = 30.0f; }
        if (type == SHIP_PIRATE)  { sp->layer = 2; sp->scale = 0.65f; sp->speed = 25.0f; }
        if (type == SHIP_GALLEON) { sp->layer = 2; sp->scale = 0.70f; sp->speed = 28.0f; }
        if (type == SHIP_YACHT)   { sp->layer = 1; sp->scale = 0.80f; sp->speed = 50.0f; }
        if (type == SHIP_SLOOP)   { sp->layer = 1; sp->scale = 0.50f; sp->speed = 40.0f; }
        
        sp->speed += (float)GetRandomValue(-200, 200) / 100.0f; 
    }
}

// =============================================================================
// Prosedur Proyeksi Panel Antarmuka Utama (Main HUD Projection Procedure)
// =============================================================================
void DrawCustomHUD(Animasi2DScreen *s) {
    if (!isHudFontLoaded) {
        hudFont = LoadFontEx("FredokaOne-Regular.ttf", 32, 0, 250); 
        isHudFontLoaded = true;
    }

    Vector2 mousePos = GetMousePosition();

    int iconMata = s->showHUD ? 1 : 2; 
    if (DrawOceanIconButton((Rectangle){20, 20, 55, 28}, iconMata, " H", mousePos, false, false)) {
        s->showHUD = !s->showHUD;
    }

    int hr = (int)s->visualTime;
    int mn = (int)((s->visualTime - hr) * 60.0f);
    Rectangle clockRec = { 85, 20, 80, 28 };
    DrawRectangleRounded(clockRec, 0.4f, 10, (Color){10, 45, 80, 220});
    DrawRectangleRoundedLines(clockRec, 0.4f, 10, (Color){30, 100, 150, 255});
    DrawHudText(TextFormat("%02d:%02d", hr, mn), 101, 25, 16.0f, WHITE);

    if (!s->showHUD) return; 
    
    int panelHeight = (s->activeTab == TAB_CONTROL) ? 350 : 400;
    Rectangle panelRec = { 20, 60, 310, panelHeight }; 
    DrawRectangleRounded(panelRec, 0.05f, 10, (Color){ 10, 15, 25, 220 });
    DrawRectangleRoundedLines(panelRec, 0.05f, 10, (Color){ 255, 255, 255, 30 }); 
    
    Rectangle tabCtrl = { 35, 75, 135, 26 };
    Rectangle tabArmd = { 175, 75, 135, 26 };
    
    if (DrawOceanButton(tabCtrl, "KONTROL", mousePos, s->activeTab == TAB_CONTROL, false)) s->activeTab = TAB_CONTROL;
    if (DrawOceanButton(tabArmd, "ARMADA", mousePos, s->activeTab == TAB_ARMADA, false)) s->activeTab = TAB_ARMADA;
    
    DrawLine(20, 115, 330, 115, (Color){ 255, 255, 255, 20 }); 

    int contentY = 130;
    
    // --- ISI KONTEN TAB KONTROL ---
    if (s->activeTab == TAB_CONTROL) {
        
        int iconPlay = s->isPlaying ? 4 : 3;
        if (DrawOceanIconButton((Rectangle){35, contentY, 40, 26}, iconPlay, "", mousePos, s->isPlaying, false)) s->isPlaying = !s->isPlaying;
        
        if (DrawOceanIconButton((Rectangle){85, contentY, 70, 26}, 5, "", mousePos, s->weather == WEATHER_CLEAR, false)) s->weather = WEATHER_CLEAR;
        if (DrawOceanIconButton((Rectangle){165, contentY, 70, 26}, 6, "", mousePos, s->weather == WEATHER_RAIN, false)) s->weather = WEATHER_RAIN;
        if (DrawOceanIconButton((Rectangle){245, contentY, 65, 26}, 7, "", mousePos, s->weather == WEATHER_STORM, false)) s->weather = WEATHER_STORM;

        contentY += 45;
        DrawHudText("Siklus Waktu", 35, contentY, 11.0f, LIGHTGRAY);
        DrawOceanCheckbox((Rectangle){255, contentY, 14, 14}, "Auto", &s->autoTime, mousePos);
        contentY += 35;

        DrawOceanSlider((Rectangle){35, contentY, 275, 6}, "Waktu Manual", &s->manualTime, 0.0f, 24.0f, mousePos, s->autoTime);
        contentY += 45;
        DrawOceanSlider((Rectangle){35, contentY, 275, 6}, "Kecepatan Global", &s->speedMult, 0.5f, 5.0f, mousePos, false);
        contentY += 40;
        DrawOceanSlider((Rectangle){35, contentY, 275, 6}, "Laju Kapal", &s->shipSpeedMult, 0.1f, 5.0f, mousePos, false);
        contentY += 40;
        DrawOceanSlider((Rectangle){35, contentY, 275, 6}, "Tinggi Ombak", &s->waveIntensity, 0.1f, 3.0f, mousePos, false);
        
        contentY += 25;

        if (DrawOceanButton((Rectangle){35, contentY, 90, 24}, "Reset Rasi", mousePos, false, false)) s->customStarCount = 0;
        DrawOceanCheckbox((Rectangle){140, contentY + 5, 14, 14}, "Wireframe", &s->isWireframe, mousePos);
    } 
    // --- ISI KONTEN TAB ARMADA ---
    else if (s->activeTab == TAB_ARMADA) {
        DrawOceanCheckbox((Rectangle){35, contentY, 14, 14}, "Auto Spawn Kapal", &s->autoSpawnShips, mousePos);
        
        if (DrawOceanButton((Rectangle){220, contentY - 4, 90, 24}, "Hapus Kapal", mousePos, false, false)) {
            for (int i = 0; i < MAX_SHIPS; i++) s->ships[i].active = false; 
        }

        contentY += 30;
        DrawLine(35, contentY, 310, contentY, (Color){ 255, 255, 255, 10 }); 
        contentY += 10;

        bool isDis = s->autoSpawnShips; 
        int colX[3] = { 35, 130, 225 }; 
        
        // --- LAYER 3 ---
        DrawHudText("LAYER 3 (Latar Belakang)", 35, contentY, 11.0f, GRAY);
        contentY += 18;
        if(DrawOceanShipButton((Rectangle){colX[0], contentY, 85, 32}, "CARGO", "(Giant)", mousePos, isDis)) SpawnShipManual(s, SHIP_CARGO, 0);
        if(DrawOceanShipButton((Rectangle){colX[1], contentY, 85, 32}, "CRUISE", "(Luxury)", mousePos, isDis)) SpawnShipManual(s, SHIP_CRUISE, 0);
        if(DrawOceanShipButton((Rectangle){colX[2], contentY, 85, 32}, "WARSHIP", "(Military)", mousePos, isDis)) SpawnShipManual(s, SHIP_MILITARY, 0);
        
        // --- LAYER 2 ---
        contentY += 45;
        DrawHudText("LAYER 2 (Tengah)", 35, contentY, 11.0f, GRAY);
        contentY += 18;
        if(DrawOceanShipButton((Rectangle){colX[0], contentY, 85, 32}, "FISHING", "(Boat)", mousePos, isDis)) SpawnShipManual(s, SHIP_FISHING, 0);
        if(DrawOceanShipButton((Rectangle){colX[1], contentY, 85, 32}, "RESEARCH", "(Ship)", mousePos, isDis)) SpawnShipManual(s, SHIP_RESEARCH, 0);
        if(DrawOceanShipButton((Rectangle){colX[2], contentY, 85, 32}, "PIRATE", "(Brown)", mousePos, isDis)) SpawnShipManual(s, SHIP_PIRATE, 0);
        contentY += 38;
        if(DrawOceanShipButton((Rectangle){colX[0], contentY, 85, 32}, "PIRATE", "(Obsidian)", mousePos, isDis)) SpawnShipManual(s, SHIP_PIRATE, 1);
        if(DrawOceanShipButton((Rectangle){colX[1], contentY, 85, 32}, "PIRATE", "(Crimson)", mousePos, isDis)) SpawnShipManual(s, SHIP_PIRATE, 2);
        if(DrawOceanShipButton((Rectangle){colX[2], contentY, 85, 32}, "GALLEON", "(Western)", mousePos, isDis)) SpawnShipManual(s, SHIP_GALLEON, 0);
        
        // --- LAYER 1 ---
        contentY += 45;
        DrawHudText("LAYER 1 (Depan)", 35, contentY, 11.0f, GRAY);
        contentY += 18;
        if(DrawOceanShipButton((Rectangle){colX[0], contentY, 85, 32}, "YACHT", "(Luxury)", mousePos, isDis)) SpawnShipManual(s, SHIP_YACHT, 0);
        if(DrawOceanShipButton((Rectangle){colX[1], contentY, 85, 32}, "SLOOP", "(Classic)", mousePos, isDis)) SpawnShipManual(s, SHIP_SLOOP, 0);
        if(DrawOceanShipButton((Rectangle){colX[2], contentY, 85, 32}, "SLOOP", "(Red Trim)", mousePos, isDis)) SpawnShipManual(s, SHIP_SLOOP, 1);
        contentY += 38;
        if(DrawOceanShipButton((Rectangle){colX[0], contentY, 85, 32}, "SLOOP", "(Gold Hull)", mousePos, isDis)) SpawnShipManual(s, SHIP_SLOOP, 2);
    }
}