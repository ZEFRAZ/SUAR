@echo off
echo ===================================================
echo  Menjalankan Web Server Lokal untuk Proyek SUAR
echo ===================================================
echo Server aktif di: http://localhost:8080/
echo Buka browser dan arahkan ke alamat di atas.
echo Tekan Ctrl+C untuk menghentikan server.
echo.
python -m http.server 8080 --directory build_web
