#ifndef ANIMASI2D_H
#define ANIMASI2D_H

#include "raylib.h"
#include <stdbool.h>

/**
 * @file animasi2d.h
 * @brief Deklarasi tipe data dan fungsi untuk layar Animasi 2D.
 *
 * File ini mendefinisikan semua konstanta, tipe data, dan struktur yang
 * dibutuhkan untuk menjalankan simulasi animasi laut lengkap dengan kapal,
 * cuaca, siklus siang-malam, dan efek pencahayaan mercusuar.
 */

// =============================================================================
// BAGIAN 1: KONSTANTA KAPASITAS MAKSIMAL
// =============================================================================
#define MAX_STARS          250  // Jumlah maksimal bintang di langit
#define MAX_CUSTOM_STARS    50  // Jumlah maksimal bintang rasi yang dibuat pengguna
#define MAX_SHOOTING_STARS   5  // Jumlah maksimal bintang jatuh sekaligus
#define MAX_BIRDS            8  // Jumlah maksimal burung di layar
#define MAX_CLOUDS           8  // Jumlah maksimal awan di langit
#define MAX_RAINDROPS      800  // Jumlah maksimal butir hujan sekaligus
#define MAX_SHIPS           20  // Jumlah maksimal kapal aktif di layar
#define MAX_LIGHTNINGS       5  // Jumlah maksimal petir sekaligus
#define MAX_DOLPHINS         4  // Jumlah maksimal lumba-lumba melompat

// =============================================================================
// BAGIAN 2: TIPE ENUMERASI
// =============================================================================

// Kondisi cuaca yang bisa dipilih
typedef enum { WEATHER_CLEAR, WEATHER_RAIN, WEATHER_STORM, WEATHER_FOG } WeatherType;

// Tab panel HUD yang sedang aktif
typedef enum { TAB_CONTROL, TAB_ARMADA, TAB_STATISTIK, TAB_MATH } UITabType;

// Jenis kapal yang bisa muncul di simulasi
typedef enum {
    SHIP_CARGO,    // Kapal kargo besar
    SHIP_FISHING,  // Kapal nelayan
    SHIP_CRUISE,   // Kapal pesiar
    SHIP_MILITARY, // Kapal perang
    SHIP_YACHT,    // Kapal yacht pribadi
    SHIP_RESEARCH, // Kapal penelitian
    SHIP_PIRATE,   // Kapal bajak laut
    SHIP_SLOOP,    // Kapal sloop ringan
    SHIP_GALLEON   // Kapal galleon besar
} ShipType;

// =============================================================================
// BAGIAN 3: STRUKTUR DATA ELEMEN SIMULASI
// =============================================================================

// Data posisi dan animasi satu bintang
typedef struct { float nx, ny, size, twinkleSpeed, offset; } StarData;

// Data posisi dan kecepatan satu burung
typedef struct { float offset, speed, nyBase, flapSpeed, scale; } BirdData;

// Data posisi dan ukuran satu awan
typedef struct { float offset, speed, nyBase, scale; int type; } CloudData;

// Data posisi dan kecepatan satu butir hujan
typedef struct { float x, y, speed, length, offset; } RaindropData;

// Data posisi dan umur satu bintang jatuh
typedef struct { float x, y, vx, vy, life; bool active; } ShootingStar;

// Data segmen dan cabang satu kilatan petir
typedef struct {
    Vector2 segments[20];  // Titik-titik yang membentuk jalur petir
    int     segCount;
    Vector2 branch[5];     // Titik-titik cabang petir
    int     branchCount;
    int     splitIdx;
    float   life;
    float   startX;
    bool    active;
} LightningData;

// Data satu ekor lumba-lumba melompat di laut
typedef struct {
    float x, y;
    float speed;
    float jumpPhase;
    float scale;
    bool active;
} DolphinData;

// Data lengkap satu kapal yang aktif di simulasi
typedef struct {
    bool     active;         // Apakah kapal ini sedang tampil di layar
    int      id;             // ID unik kapal
    ShipType type;           // Jenis kapal
    int      variant;        // Varian warna/model kapal
    int      layer;          // Lapisan kedalaman (1=dekat, 3=jauh)
    float    x, y;           // Posisi kapal di layar
    float    speed;          // Kecepatan gerak horizontal
    float    scale;          // Ukuran kapal
    float    dyDx;           // Kemiringan permukaan ombak di bawah kapal
    float    angle;          // Sudut putar kapal mengikuti ombak
    bool     isIlluminated;  // Apakah kapal sedang terkena sorot lampu mercusuar
} ShipData;

// =============================================================================
// BAGIAN 4: STRUKTUR STATUS LAYAR ANIMASI 2D
// =============================================================================

// Seluruh data status dan konfigurasi layar Animasi 2D
typedef struct {
    // --- Status UI ---
    bool      showHUD;       // Apakah panel HUD sedang ditampilkan
    UITabType activeTab;     // Tab HUD yang sedang aktif
    bool      isPlaying;     // Apakah simulasi sedang berjalan
    float     speedMult;     // Pengali kecepatan global simulasi
    WeatherType weather;     // Kondisi cuaca saat ini
    bool      isWireframe;   // Apakah mode wireframe sedang aktif

    // --- Pengaturan Simulasi ---
    bool  autoTime;          // Apakah waktu berjalan otomatis
    float manualTime;        // Waktu manual yang diatur pengguna (0.0–24.0)
    bool  autoLighthouse;    // Apakah lampu mercusuar otomatis menyala saat malam
    float waveIntensity;     // Pengali intensitas ombak dari slider HUD
    float windSpeedMult;     // Pengali kecepatan angin
    float shipSpeedMult;     // Pengali kecepatan kapal
    bool  autoSpawnShips;    // Apakah kapal muncul otomatis

    // --- Pewaktu Internal ---
    float realTime;          // Waktu total animasi berjalan (detik)
    float timeOfDay;         // Waktu hari target (0.0–24.0)
    float visualTime;        // Waktu hari yang ditampilkan (bergerak mulus ke target)
    float lastSpawnTime;     // Waktu terakhir kapal baru muncul
    int   shipIdCounter;     // Penghitung ID unik kapal
    int   shipsPassed;       // Jumlah total kapal yang berhasil melintasi layar
    double sessionStartTime; // Jam dunia nyata sejak simulasi dibuka (dihitung via GetTime())

    // --- Nilai Fisika Cuaca ---
    float skyDarken;         // Tingkat kegelapan langit (0.0 = terang, 1.0 = gelap)
    float waveMult;          // Pengali tinggi ombak saat ini
    float windSpeed;         // Kecepatan angin saat ini
    float birdMult;          // Pengali kecepatan kepakan burung

    // --- Penghitung Fase Animasi ---
    float pWave;             // Fase animasi ombak
    float pCloud;            // Fase pergerakan awan
    float pBird;             // Fase pergerakan burung

    // --- Data Kursor dan Kamera ---
    Vector2 mousePos;        // Posisi kursor di layar
    float   beamAngle;       // Sudut rotasi sorot lampu mercusuar (radian)
    float   responsiveScale; // Faktor skala layar agar tampilan responsif

    // --- Data Elemen Simulasi ---
    Vector2    customStars[MAX_CUSTOM_STARS];   // Bintang rasi buatan pengguna
    int        customStarCount;                  // Jumlah bintang rasi saat ini
    StarData   stars[MAX_STARS];                 // Data semua bintang langit
    BirdData   birds[MAX_BIRDS];                 // Data semua burung
    CloudData  clouds[MAX_CLOUDS];               // Data semua awan
    RaindropData raindrops[MAX_RAINDROPS];       // Data semua butir hujan
    ShipData   ships[MAX_SHIPS];                 // Data semua kapal aktif
    ShootingStar  shootingStars[MAX_SHOOTING_STARS]; // Data bintang jatuh
    LightningData lightnings[MAX_LIGHTNINGS];    // Data kilatan petir
    DolphinData dolphins[MAX_DOLPHINS];          // Data sekawanan lumba-lumba

} Animasi2DScreen;

// =============================================================================
// BAGIAN 5: DEKLARASI FUNGSI
// =============================================================================

/**
 * @brief Menginisialisasi semua nilai awal layar Animasi 2D.
 * @param state Pointer ke data status layar yang akan diinisialisasi.
 */
void InitAnimasi2DScreen(Animasi2DScreen *state);

/**
 * @brief Memperbarui fisika dan logika simulasi setiap frame.
 * @param state Pointer ke data status layar.
 */
void UpdateAnimasi2DScreen(Animasi2DScreen *state);

/**
 * @brief Menggambar seluruh objek layar Animasi 2D ke layar.
 * @param state Pointer ke data status layar.
 */
void DrawAnimasi2DScreen(Animasi2DScreen *state);

/**
 * @brief Melepaskan semua sumber daya layar Animasi 2D dari memori.
 * @param state Pointer ke data status layar.
 */
void UnloadAnimasi2DScreen(Animasi2DScreen *state);

#endif // ANIMASI2D_H