@echo off
echo ===================================================
echo  Mengompilasi Proyek SUAR (Raylib C) ke WebAssembly
echo ===================================================
call C:\raylib\emsdk\emsdk_env.bat
if not exist build_web mkdir build_web
emcc -Os -Wall -DPLATFORM_WEB ^
  -I. -IC:/raylib/raylib/src -Isrc/screens -Isrc/ui -Isrc/algo -Isrc/objects ^
  -o build_web/index.html ^
  main.c ^
  src/screens/menu.c ^
  src/screens/animasi2d.c ^
  src/screens/about.c ^
  src/screens/guide.c ^
  src/screens/pola2d.c ^
  src/objects/kapal.c ^
  src/objects/alam.c ^
  src/objects/bangunan.c ^
  src/ui/audio.c ^
  src/ui/draw_helpers.c ^
  src/ui/back_button.c ^
  src/algo/bresenham.c ^
  src/algo/dda.c ^
  src/algo/midcircle.c ^
  src/ui/hud.c ^
  C:/raylib/raylib/src/libraylib.web.a ^
  -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s TOTAL_MEMORY=67108864 ^
  --preload-file FredokaOne-Regular.ttf ^
  --preload-file Starbim.ttf ^
  --preload-file background.png ^
  --preload-file background_pola2d.png ^
  --preload-file icon.png ^
  --preload-file bgm.mp3 ^
  --shell-file shell_web.html

if %ERRORLEVEL% EQU 0 (
    copy /Y icon.png build_web\icon.png >nul
    copy /Y icon.ico build_web\favicon.ico >nul
    copy /Y icon.ico build_web\icon.ico >nul
    echo.
    echo [SUKSES] Kompilasi WebAssembly berhasil!
    echo File tersimpan di folder: build_web\
    echo.
    echo Untuk menjalankan di browser, jalankan: run_web.bat
) else (
    echo.
    echo [GAGAL] Terjadi kesalahan saat kompilasi.
)
