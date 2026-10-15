<div align="center">
  <img src="icon.png" width="150" alt="Lighthouse Logo"/>
  <h1>🚢 Proyek Simulasi 2D Mercusuar dan Kapal Laut 🌊</h1>
  <p><i>Implementasi Algoritma DDA dan Bresenham dalam Simulasi Grafika 2D</i></p>
</div>

<br/>

## 📖 Deskripsi Singkat
Proyek ini adalah pelabuhan laut interaktif berlatarkan malam hari yang dibangun menggunakan bahasa **C** dan library **Raylib**. Dikembangkan khusus sebagai penugasan / Ujian Tengah Semester mata kuliah Komputer Grafis Praktik.

## ✨ Fitur Utama (Technical Highlight)

*   📐 **Custom Algoritma Primitif Grafis:** Menggunakan algoritma **DDA (Digital Differential Analyzer)** dan **Bresenham** buatan sendiri untuk fondasi penarikan garis.
*   🖌️ **Rasterisasi Scanline Kustom (`FillTri`):** Pewarnaan bidang (poligon) tidak menggunakan cat instan, melainkan ribuan garis mikroskopis yang digesek sejajar.
*   🌊 **Fisika Gelombang Lautan Sinus (Double-Sine Wave):** Simulasi air yang tidak statis, dihidupkan melalui tumpukan dua fungsi ombak berperiode acak dengan kecepatan *real-time*.
*   🚢 **Generasi Prosedural Kapal (Bezier Curve):** Lengkungan tiang layar kapal dan bentuk ombak digambar dengan rumus *Cubic Bezier Interpolation*. Komputer akan memecah belokan tersebut menjadi puluhan garis lurus pendek, yang kemudian akan disambung/digambar titik demi titik menggunakan algoritma DDA.
*   💨 **Sistem Partikel Asap:** Asap hasil pembakaran kapal digenerate berdasarkan hitungan acak `sin()`, hukum resistansi angin asimetris, hingga akhirnya meluruh secara transparan (*Alpha Decay*).
*   🔦 **Shader Kamera & Blending Optik Prisma:** Lampu Mercusuar menerapkan perhitungan tabrakan sudut transparan (*Cone Beam*) dan merespon pada efek malam gelap bergradasi.

*   🐬 **Fauna Laut Prosedural (Lumba-lumba):** Animasi melompat kurva parabola dinamis dengan anatomi poligon kustom (`DrawCuaca_Dolphin`).
*   🌫️ **Sistem Cuaca Kabut Tebal & Atmosferik:** Simulasi kabut atmosferik berlapis gradasi yang merespon perubahan cuaca secara dinamis (`WEATHER_FOG`).
*   📊 **Dashboard Metrik & Statistik:** Panel analitik real-time yang mencatat durasi simulasi (uptime jam dunia nyata) dan jumlah armada kapal yang telah sukses melintas.
*   🌐 **Dukungan Penuh WebAssembly (HTML5/Wasm):** Dapat dimainkan langsung di web browser secara lancar via Emscripten.

## 🛠️ Cara Menjalankan (*Build*)

### 1. Versi Desktop Windows (Native)
Menggunakan MinGW / W64Devkit dan file `Makefile.win` yang sudah disediakan:
```bash
# Bersihkan dan jalankan program desktop
make -f Makefile.win clean
make -f Makefile.win run
```
*Atau klik ganda langsung pada file `SUAR.exe`.*

### 2. Versi Web Browser (HTML5 / WebAssembly)
Dapat dikompilasi menggunakan Emscripten (emsdk) dengan skrip otomatis:
```bash
# Kompilasi ke WebAssembly
.\build_web.bat

# Jalankan web server lokal
.\run_web.bat
```
Lalu buka browser Anda di: `http://localhost:8080/`

## 🎮 Kontrol Navigasi Interaktif

Menyajikan menu pameran dan presentasi yang ramah pengguna (*user friendly*):
*   **[ W ]**          : *Toggle* Mode *Wireframe* (Memunculkan rangka garis matematika layar)
*   **[ SPASI ]**      : Putar / Hentikan Animasi (*Play/Pause*)
*   **[ H ]**          : Menyembunyikan / Menampilkan tampilan antarmuka HUD
*   **[ UP / DOWN ]**  : Mempercepat / Memperlambat kecepatan waktu semesta
*   **[ Klik Kiri ]**  : Menembak rasi bintang (*Hit Point Ray-casting*) / Berinteraksi dengan tombol UI
*   **[ BACKSPACE ]**  : Kembali ke Menu Utama dari layar Animasi