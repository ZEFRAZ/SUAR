#ifndef ANIMASI2D_H
#define ANIMASI2D_H

#include "raylib.h"
#include <stdbool.h>

#define MAX_STARS 250
#define MAX_CUSTOM_STARS 50
#define MAX_SHOOTING_STARS 5
#define MAX_BIRDS 8
#define MAX_CLOUDS 8
#define MAX_RAINDROPS 800
#define MAX_SHIPS 20
#define MAX_LIGHTNINGS 5

typedef enum { WEATHER_CLEAR, WEATHER_RAIN, WEATHER_STORM } WeatherType;
typedef enum { TAB_CONTROL, TAB_ARMADA, TAB_MATH } UITabType;

// --- INI YANG KEMARIN TIDAK SENGAJA TERHAPUS OLEH SAYA ---
typedef enum { 
    SHIP_CARGO, SHIP_FISHING, SHIP_CRUISE, SHIP_MILITARY, 
    SHIP_YACHT, SHIP_RESEARCH, SHIP_PIRATE, SHIP_SLOOP, SHIP_GALLEON 
} ShipType;

typedef struct { float nx, ny, size, twinkleSpeed, offset; } StarData;
typedef struct { float offset, speed, nyBase, flapSpeed, scale; } BirdData;
typedef struct { float offset, speed, nyBase, scale; int type; } CloudData;
typedef struct { float x, y, speed, length, offset; } RaindropData;
typedef struct { float x, y, vx, vy, life; bool active; } ShootingStar;
typedef struct { Vector2 segments[20]; int segCount; Vector2 branch[5]; int branchCount; int splitIdx; float life; float startX; bool active; } LightningData;

typedef struct {
    bool active;
    int id;
    ShipType type; // Dikembalikan menjadi ShipType
    int variant;
    int layer;
    float x, y;    
    float speed;
    float scale;
    float dyDx;
    float angle;   
    bool isIlluminated;
} ShipData;

typedef struct {
    bool showHUD;
    UITabType activeTab;
    bool isPlaying;
    float speedMult; 
    WeatherType weather;
    bool isWireframe;

    bool autoTime;
    float manualTime;
    bool autoLighthouse;
    float waveIntensity;
    float windSpeedMult;
    float shipSpeedMult;
    bool autoSpawnShips;

    float realTime;
    float timeOfDay;
    float visualTime; 
    float lastSpawnTime;
    int shipIdCounter;

    float skyDarken;
    float waveMult;
    float windSpeed;
    float birdMult;

    float pWave;
    float pCloud;
    float pBird;

    Vector2 mousePos;
    float beamAngle;
    float responsiveScale;

    Vector2 customStars[MAX_CUSTOM_STARS];
    int customStarCount;
    StarData stars[MAX_STARS];
    BirdData birds[MAX_BIRDS];
    CloudData clouds[MAX_CLOUDS];
    RaindropData raindrops[MAX_RAINDROPS];
    ShipData ships[MAX_SHIPS];
    ShootingStar shootingStars[MAX_SHOOTING_STARS];
    LightningData lightnings[MAX_LIGHTNINGS];

} Animasi2DScreen;

void InitAnimasi2DScreen(Animasi2DScreen *state);
void UpdateAnimasi2DScreen(Animasi2DScreen *state);
void DrawAnimasi2DScreen(Animasi2DScreen *state);
void UnloadAnimasi2DScreen(Animasi2DScreen *state);

#endif // ANIMASI2D_H