<div align="center">
  <img src="icon.png" width="140" alt="Logo SUAR"/>
  <h1>SUAR</h1>
  <p><b>Aplikasi Simulasi Grafika 2D Mercusuar dan Kapal Laut Berbasis C &amp; Raylib</b></p>
  <p><i>Implementasi Algoritma DDA, Bresenham, dan Midpoint Circle dalam Grafika Komputer</i></p>

  <p>
    <a href="https://github.com/ZEFRAZ/SUAR"><img src="https://img.shields.io/badge/GitHub-SUAR-blue?logo=github" alt="Repository SUAR"></a>
    <img src="https://img.shields.io/badge/Platform-Windows%20%7C%20WebAssembly-green" alt="Platform">
    <img src="https://img.shields.io/badge/Language-C%20%2F%20Raylib-orange" alt="Language">
  </p>
</div>

<br/>

## 📖 Tentang Aplikasi SUAR

**SUAR** adalah aplikasi simulasi grafika komputer interaktif berlatarkan pesisir laut dan mercusuar di malam hari yang dibangun menggunakan bahasa **C** dan pustaka **Raylib**. Aplikasi ini menyajikan visualisasi maritim dinamis lengkap dengan fisika gelombang, animasi kapal prosedural, efek atmosferik, serta pengontrolan interaktif secara *real-time*.

Aplikasi **SUAR** tersedia dalam dua mode:
1. **Desktop Native (Windows):** Berkas eksekusi langsung `SUAR.exe` yang di-compile menggunakan MinGW/W64devkit.
2. **Web Browser (WebAssembly/HTML5):** Berjalan mulus di peramban web modern melalui kompilasi Emscripten.

---

## ✨ Fitur Utama (Technical Highlight)

* 📐 **Custom Algoritma Primitif Grafis:** Menggunakan implementasi mandiri dari algoritma **DDA (Digital Differential Analyzer)**, **Bresenham Line**, dan **Midpoint Circle** sebagai fondasi rendering.
* 🖌️ **Rasterisasi Scanline Kustom (`FillTri`):** Pewarnaan bidang (poligon) menggunakan algoritma sapuan garis mikroskopis yang disusun sejajar.
* 🌊 **Fisika Gelombang Lautan Sinus (Double-Sine Wave):** Simulasi air dinamis dengan tumpukan fungsi gelombang ganda berperiode acak dengan kecepatan *real-time*.
* 🚢 **Generasi Prosedural Kapal (Bezier Curve):** Lengkungan tiang layar kapal dan bentuk ombak digambar dengan rumus *Cubic Bezier Interpolation*, diinterpolasi menjadi garis-garis pendek yang digambar dengan algoritma DDA.
* 💨 **Sistem Partikel Asap:** Asap cerobong kapal dihitung berbasis osilasi trigonometri acak, resistansi angin, dan peluruhan transparansi (*Alpha Decay*).
* 🔦 **Shader Kamera & Blending Optik Prisma:** Sorotan lampu mercusuar menerapkan kerucut cahaya transparan (*Cone Beam*) adaptif terhadap suasana malam.
* 🐬 **Fauna Laut Prosedural (Lumba-lumba):** Animasi melompat mengikuti kurva parabola dinamis dengan anatomi poligon kustom (`DrawCuaca_Dolphin`).
* 🌫️ **Sistem Cuaca Kabut Tebal & Atmosferik:** Simulasi kabut atmosferik berlapis yang merespons perubahan cuaca secara dinamis (`WEATHER_FOG`).
* 📊 **Dashboard Metrik & Statistik:** Panel analitik real-time yang mencatat durasi simulasi (uptime dunia nyata) dan jumlah armada kapal yang sukses melintas.
* 🌐 **Dukungan WebAssembly (HTML5/Wasm):** Dilengkapi *custom responsive shell* modern dengan logo, favicon, dan mode layar penuh (*Fullscreen*).

---

## 🛠️ Cara Menjalankan (*Build*)

### 1. Kloning Repositori
```bash
git clone https://github.com/ZEFRAZ/SUAR.git
cd SUAR
```

### 2. Versi Desktop Windows (Native)
Menggunakan MinGW / W64Devkit dan file `Makefile.win` yang sudah disediakan:
```bash
# Bersihkan dan jalankan program desktop
make -f Makefile.win clean
make -f Makefile.win run
```
*Atau klik ganda langsung pada berkas **`SUAR.exe`**.*

### 3. Versi Web Browser (HTML5 / WebAssembly)
Dapat dikompilasi menggunakan Emscripten (emsdk) dengan skrip otomatis:
```bash
# Kompilasi ke WebAssembly
.\build_web.bat

# Jalankan web server lokal
.\run_web.bat
```
Lalu buka browser Anda di: `http://localhost:8080/`

---

## 🎮 Kontrol Navigasi Interaktif

* **[ W ]**         : *Toggle* Mode *Wireframe* (Memunculkan rangka garis matematika layar)
* **[ SPASI ]**     : Putar / Hentikan Animasi (*Play/Pause*)
* **[ H ]**         : Menyembunyikan / Menampilkan tampilan antarmuka HUD
* **[ T ]**         : Mengganti tema / pencahayaan visual
* **[ C ]**         : Mengganti kondisi cuaca (Cerah, Hujan, Kabut Tebal)
* **[ UP / DOWN ]** : Mempercepat / Memperlambat kecepatan simulasi
* **[ Klik Kiri ]** : Menembak rasi bintang (*Hit Point Ray-casting*) / Berinteraksi dengan tombol UI
* **[ BACKSPACE ]** : Kembali ke Menu Utama dari layar Animasi