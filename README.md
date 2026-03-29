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

## 🛠️ Cara Menjalankan (*Build*)

Proyek ini telah dikompilasi mulus di *environment* Windows memakai MSYS2 (W64Devkit). Cukup menggunakan file `Makefile.win` yang sudah disediakan:

```bash
# Untuk membersihkan file kompilasi lama
make -f Makefile.win clean

# Untuk kompilasi dan langsung menjalankan program
make -f Makefile.win run
```

## 🎮 Kontrol Navigasi Interaktif

Menyajikan menu pameran dan presentasi, Proyek ini ramah (*user friendly*).
*   **[ W ]**          : *Toggle* Mode *Wireframe* (Memunculkan rangka garis matematika layar)
*   **[ SPASI ]**      : Putar / Hentikan Animasi (*Play/Pause*)
*   **[ H ]**          : Menyembunyikan tampilan antar-muka HUD
*   **[ UP / DOWN ]**  : Mempercepat / Memperlambat kecepatan waktu semesta
*   **[ Klik Kiri ]**  : Menembak rasi bintang (*Hit Point Ray-casting*)